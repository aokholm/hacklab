/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android/looper.h>
#include <assert.h>
#include <pthread.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-sensors", __VA_ARGS__))


static jmethodID onReturnedStringID;
static jmethodID onReturnedSensorValueID;
static jobject handler = NULL;
static JavaVM *jvm;
static ASensorManager* sensorManager;
static const ASensor* magSensor;
static ASensorEventQueue* sensorEventQueue;

int magCounter = 0;
int64_t lastMagTime = 0;
int64_t lastlastMagTime = 0;
static int32_t updateRateUs = 15625; // 64 Hz / could also be 100 Hz
static int32_t updateRateNs = 0;
static int32_t updateRateNs2 = 0;

ALooper* looper;


#define BufferSize 100

volatile int isUpdating = 0;
ASensorEvent* BUFFER;
int BufferIndex=0;

// http://www.dailyfreecode.com/code/producer-consumer-problem-thread-1105.aspx
pthread_cond_t Buffer_Not_Full=PTHREAD_COND_INITIALIZER;
pthread_cond_t Buffer_Not_Empty=PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

pthread_t ctid;


/* global mutex for our program. assignment initializes it. */
/* note that we use a RECURSIVE mutex, since a handler      */
/* thread might try to lock it twice consecutively.         */
pthread_mutex_t event_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER; // PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP; //PTHREAD_MUTEX_INITIALIZER;


/* global condition variable for our program. assignment initializes it. */
pthread_cond_t  got_event   = PTHREAD_COND_INITIALIZER;




static int get_sensor_events(int fd, int events, void* data);
static void sendEventAsValues(ASensorEvent event);




/* Very simple queue
 * These are FIFO queues which discard the new data when full.
 *
 * Queue is empty when in == out.
 * If in != out, then
 *  - items are placed into in before incrementing in
 *  - items are removed from out before incrementing out
 * Queue is full when in == (out-1 + QUEUE_SIZE) % QUEUE_SIZE;
 *
 * The queue will hold QUEUE_ELEMENTS number of items before the
 * calls to QueuePut fail.
 */

/* Queue structure */
#define QUEUE_ELEMENTS 127
#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)
int Queue[QUEUE_SIZE];
int QueueIn, QueueOut;

void QueueInit(void)
{
    QueueIn = QueueOut = 0;
}

int QueuePut(int new)
{
    if(QueueIn == (( QueueOut - 1 + QUEUE_SIZE) % QUEUE_SIZE))
    {
        return -1; /* Queue Full*/
    }

    Queue[QueueIn] = new;

    QueueIn = (QueueIn + 1) % QUEUE_SIZE;

    return 0; // No errors
}

int QueueGet(int *old)
{

	if(QueueIn == QueueOut)
    {
        return -1; /* Queue Empty - nothing to get*/
    }
    *old = Queue[QueueOut];
    QueueOut = (QueueOut + 1) % QUEUE_SIZE;

    return 0; // No errors
}







/*
 * Consumer thread
 * @Param ()
 * return void
 */

void *Consumer()
{
	int rc;
	while(1) // continue uploading
    {
        int value;

		if (QueueGet(&value) == 0) {
//			LOGI("consumed new value: %d", value);
		} else {
		    rc = pthread_cond_wait(&got_event, &event_mutex);
		    /* and after we return from pthread_cond_wait, the mutex  */
		    /* is locked again, so we don't need to lock it ourselves */

		    /* unlock mutex */
		    rc = pthread_mutex_unlock(&event_mutex);
		}

		if (isUpdating == 0) {
			LOGI("Exit Thread");
			pthread_exit(0);
		}

    }
}


/*
 * setListener
 * @param jobject listenerobject
 * return void
 * Initializes Sensors and sets callback object and methods
 */

JNIEXPORT void JNICALL
Java_com_vaavud_sensor_jni_Sensors_setListener( JNIEnv* env,
                                                  jclass cls, jobject listenerObject ) {

	LOGI("setListener (Init)");

	// SETUP CALLBACK
	jclass handlerClass = (*env)->GetObjectClass(env, listenerObject);
	if (handlerClass == NULL) {LOGI("Could not find class");}
	onReturnedStringID = (*env)->GetMethodID(env, handlerClass, "onReturnedString", "(Ljava/lang/String;)V");
	onReturnedSensorValueID = (*env)->GetMethodID(env, handlerClass, "onReturnedSensorValue", "(JFFF)V");

	if (onReturnedStringID == NULL) { LOGI("Could not find method");}

    handler = (*env)->NewGlobalRef(env, listenerObject);

	jint rs = (*env)->GetJavaVM(env, &jvm);
	assert (rs == JNI_OK);

    // SETUP SENSORS
    sensorManager = ASensorManager_getInstance();
    magSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
    int minDelay = ASensor_getMinDelay(magSensor);
    LOGI("min-delay: %d", minDelay);

    updateRateNs2 = updateRateUs*2200;
    updateRateNs = updateRateUs*1000;


//    BUFFER=(char *) malloc(sizeof(char) * BufferSize);
    BUFFER = malloc(sizeof(ASensorEvent) * BufferSize);

    looper = ALooper_forThread();
    if(looper == NULL) {
        looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    }

}


/*
 * startSensors
 * @param ()
 * return void
 * Start the queue
 */

JNIEXPORT void JNICALL
Java_com_vaavud_sensor_jni_Sensors_startSensors( JNIEnv* env,
                                                  jclass cls ) {

    // create upload thread - dies when parrent thread dies
    isUpdating = 1;

    // Initialize queue
    QueueInit();

//    LOGI("Initialize Mutex(%u)", &lock);
//	pthread_mutexattr_t attr;
//	pthread_mutexattr_init(&attr);
//	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
//	int ii_p = pthread_mutex_init(&lock, &attr);
////	pthread_mutex_init(&lock, &attr);
//	pthread_mutexattr_destroy(&attr);
//	LOGI("Initialize Mutex(%u) Returned %d", &lock, ii_p);



    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, 3, get_sensor_events, NULL);

    ASensorEventQueue_enableSensor(sensorEventQueue, magSensor);
    ASensorEventQueue_setEventRate(sensorEventQueue, magSensor, updateRateUs);

    LOGI("sensorValue() - START");

//    pthread_create(&ctid,NULL,Consumer,NULL);




}

JNIEXPORT void JNICALL
Java_com_vaavud_sensor_jni_Sensors_stopSensors(JNIEnv* env, jclass cls) {
	isUpdating = 0; // kill uploader thread
	ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
	pthread_cond_signal(&got_event); // unlock consumer to kill the thread

}


static int get_sensor_events(int fd, int events, void* data) {
  ASensorEvent event;
  while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {

        if(event.type == ASENSOR_TYPE_MAGNETIC_FIELD) {

//        	int ecode = pthread_mutex_lock(&lock);
//            LOGI("produce lock: %d \n",ecode);
//
//        	if(BufferIndex==BufferSize)
//			{
//			  LOGI("conWAIT");
//			  pthread_cond_wait(&Buffer_Not_Full,&lock);
//			}
//        	sendEventAsValues(event);
//			BUFFER[BufferIndex++]=event;
//			LOGI("Produced : %d \n",BufferIndex);
//        	ecode = pthread_mutex_unlock(&lock);
//        	LOGI("produce unlock: %d \n",ecode);
//
//        	LOGI("buffer not empty signaled, Bufferindex: %d \n", BufferIndex);
//        	pthread_cond_signal(&Buffer_Not_Empty);


//        	if (QueuePut(magCounter) == -1) {
//        		LOGI("Queue Full. Measurement Lost");
//        	}
//        	pthread_cond_signal(&got_event);
        	sendEventAsValues(event);

        	int timediff = (event.timestamp-lastlastMagTime);
        	if (timediff > updateRateNs2) {
        		 LOGI("Measurements lost: %f", (float) timediff/(float) updateRateNs);
        	}
        	lastlastMagTime = event.timestamp;
			if(magCounter == 0 || magCounter == 100)
				{
				 //LOGI("Mag-Time: %lld (%f)", event.timestamp,(100000000000.0/(double) (event.timestamp-lastMagTime)));
				 lastMagTime = event.timestamp;

				 magCounter = 0;
				}

			magCounter++;
        }

  }
  //should return 1 to continue receiving callbacks, or 0 to unregister
  return 1;
}


static void sendEventAsValues(ASensorEvent event) {
	int status;
	JNIEnv *env;
	int isAttached = 0;

	if (!handler) return;

	if ((status = (*jvm)->GetEnv(jvm, (void**)&env, JNI_VERSION_1_6)) < 0) {
		if ((status = (*jvm)->AttachCurrentThread(jvm, &env, NULL)) < 0) {
			return;
		}
		isAttached = 1;
	}

	(*env)->CallVoidMethod(env, handler, onReturnedSensorValueID, (jlong) event.timestamp, (jfloat) event.acceleration.x, (jfloat) event.acceleration.y, (jfloat) event.acceleration.y);

	if (isAttached) (*jvm)->DetachCurrentThread(jvm);
}

/*
 *  GET SENSOR NAME
 *  @param ()
 *  Returns jstring
 *  Need magSensor to be initialized
 */

JNIEXPORT jstring JNICALL
Java_com_vaavud_sensor_jni_Sensors_getSensorName( JNIEnv* env,
                                                  jclass cls )
{
    const char* sensorName = ASensor_getName(magSensor);
	jstring jstrSensorName = (*env)->NewStringUTF(env, sensorName);

	return jstrSensorName;
}



void setTheString(jstring str) {

	LOGI("setTheString");
	int status;
	JNIEnv *env;
	int isAttached = 0;

	if (!handler) return;

	if ((status = (*jvm)->GetEnv(jvm, (void**)&env, JNI_VERSION_1_6)) < 0) {
		if ((status = (*jvm)->AttachCurrentThread(jvm, &env, NULL)) < 0) {
			return;
		}
		isAttached = 1;
	}

	(*env)->CallVoidMethod(env, handler, onReturnedStringID, str);

	if (isAttached) (*jvm)->DetachCurrentThread(jvm);
}

JNIEXPORT void JNICALL
Java_com_vaavud_sensor_jni_Sensors_setString( JNIEnv* env,
                                                  jclass cls,
                                                  jstring str) {
	LOGI("setString");
	setTheString(str);
}




