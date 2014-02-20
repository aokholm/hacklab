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

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

static jmethodID onReturnedStringID;
static jobject handler = NULL;
static JavaVM *jvm;
static ASensorManager* sensorManager;
static const ASensor* magSensor;
static ASensorEventQueue* sensorEventQueue;

int magCounter = 0;
int64_t lastMagTime = 0;


void setTheString(jstring str);
static int get_sensor_events(int fd, int events, void* data);
static void sendEvent(ASensorEvent event);

JNIEXPORT jstring JNICALL
Java_com_vaavud_magnative2_MainActivity_getSensorName( JNIEnv* env,
                                                  jobject thiz )
{
    const char* sensorName = ASensor_getName(magSensor);
	jstring jstrSensorName = (*env)->NewStringUTF(env, sensorName);

	return jstrSensorName;
}


JNIEXPORT void JNICALL
Java_com_vaavud_magnative2_MainActivity_nativeInit( JNIEnv* env,
                                                  jobject thiz ) {

	LOGI("INIT");

	// SETUP CALLBACK
	jclass handlerClass = (*env)->FindClass(env, "com/vaavud/magnative2/MainActivity");
	if (handlerClass == NULL) {LOGI("Could not find class");}
	onReturnedStringID = (*env)->GetMethodID(env, handlerClass, "onReturnedString", "(Ljava/lang/String;)V");
	if (onReturnedStringID == NULL) { LOGI("Could not find method");}

    handler = (*env)->NewGlobalRef(env, thiz);

	jint rs = (*env)->GetJavaVM(env, &jvm);
	assert (rs == JNI_OK);

    // SETUP SENSORS
    sensorManager = ASensorManager_getInstance();
    magSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
}



JNIEXPORT void JNICALL
Java_com_vaavud_magnative2_MainActivity_startSensors( JNIEnv* env,
                                                  jobject thiz ) {

	LOGI("startSensors()");
    ASensorEvent event;
    int events, ident;
    void* sensor_data = malloc(1000);

    LOGI("startSensors() - ALooper_forThread()");

    ALooper* looper = ALooper_forThread();

    if(looper == NULL)
    {
        looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    }

    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, 3, get_sensor_events, sensor_data);

    ASensorEventQueue_enableSensor(sensorEventQueue, magSensor);

    //Sampling rate: 100Hz
    int minDelay = ASensor_getMinDelay(magSensor);
    LOGI("min-delay: %d", minDelay);
    ASensorEventQueue_setEventRate(sensorEventQueue, magSensor, 10000);

    LOGI("sensorValue() - START");

}

JNIEXPORT void JNICALL
Java_com_vaavud_magnative2_MainActivity_stopSensors(JNIEnv* env, jobject thiz) {
	ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
}


static int get_sensor_events(int fd, int events, void* data) {
  ASensorEvent event;
  //ASensorEventQueue* sensorEventQueue;
  while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
        if(event.type == ASENSOR_TYPE_MAGNETIC_FIELD) {
        	sendEvent(event);

			//LOGI("accl(x,y,z,t): %f %f %f %lld", event.acceleration.x, event.acceleration.y, event.acceleration.z, event.timestamp);
			if(magCounter == 0 || magCounter == 100)
				{
				 LOGI("Mag-Time: %lld (%f)", event.timestamp,((double)(event.timestamp-lastMagTime))/1000000000.0);
				 lastMagTime = event.timestamp;
				 magCounter = 0;
				}

			magCounter++;
        }

  }
  //should return 1 to continue receiving callbacks, or 0 to unregister
  return 1;
}

static void sendEvent(ASensorEvent event) {
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

	char buffer[128];
	snprintf( buffer, sizeof(buffer), "accl(x,y,z,t): %f %f %f %lld", event.acceleration.x, event.acceleration.y, event.acceleration.z, event.timestamp);

	jstring str = (*env)->NewStringUTF(env, buffer);

	(*env)->CallVoidMethod(env, handler, onReturnedStringID, str);

	if (isAttached) (*jvm)->DetachCurrentThread(jvm);
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
Java_com_vaavud_magnative2_MainActivity_setString( JNIEnv* env,
                                                  jobject thiz,
                                                  jstring str) {
	LOGI("setString");
	setTheString(str);
}

