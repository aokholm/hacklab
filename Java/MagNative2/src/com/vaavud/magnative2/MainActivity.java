package com.vaavud.magnative2;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.widget.TextView;

import com.example.magnative2.R;
import com.vaavud.sensor.jni.Sensors;
import com.vaavud.sensor.jni.SensorsListener;

public class MainActivity extends Activity implements SensorsListener {

	TextView  tv;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
//		Sensors sensors = new Sensors();
		Sensors.setListener(this);
//		nativeInit();
//		setContentView(R.layout.activity_main);

        /* Create a TextView and set its content.
         * the text is retrieved by calling a native
         * function.
         */
        tv = new TextView(this);
        tv.setText( Sensors.getSensorName() );
        setContentView(tv);
        
        Sensors.setString("Freaking Awesome!");
        
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
		Sensors.stopSensors();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		Sensors.startSensors();
	}
	
	
//	private native void nativeInit();
	
	
    public void onReturnedString(String str) 
    { 
        /* Do something with the string */
    	tv.setText(str);
    }
    
    public void onReturnedSensorValue(long timestamp, float x, float y, float z) {
    	tv.setText(String.valueOf(x));
    }
    
}
