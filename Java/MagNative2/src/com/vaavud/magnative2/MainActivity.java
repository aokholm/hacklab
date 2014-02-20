package com.vaavud.magnative2;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.widget.TextView;

import com.example.magnative2.R;

public class MainActivity extends Activity {

	TextView  tv;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		nativeInit();
//		setContentView(R.layout.activity_main);

        /* Create a TextView and set its content.
         * the text is retrieved by calling a native
         * function.
         */
        tv = new TextView(this);
        tv.setText( getSensorName() );
        setContentView(tv);
        
        setString("Freaking Awesome!");
        
    }

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		stopSensors();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		startSensors();
	}
	
	
    /* A native method that is implemented by the
     * 'hello-jni' native library, which is packaged
     * with this application.
     */
    private native String getSensorName();
	private native void nativeInit();
	private native void startSensors();
	private native void stopSensors();
	private native void setString(String str);
	
    
    public void onReturnedString(String str) 
    { 
        /* Do something with the string */
    	tv.setText(str);
    }
    
    public void onReturnedSensorValue(long timestamp, float x, float y, float z) {
    	tv.setText(String.valueOf(x));
    }
    
	static {
        System.loadLibrary("sensors");
    }
    
    
    
}
