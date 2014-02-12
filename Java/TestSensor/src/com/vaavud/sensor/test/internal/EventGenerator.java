package com.vaavud.sensor.test.internal;

import com.vaavud.sensor.Sensor;
import com.vaavud.sensor.Sensor.Type;
import com.vaavud.sensor.SensorEvent3D;
import com.vaavud.sensor.SensorListener;
import com.vaavud.sensor.test.TestSensorConfig;
import com.vaavud.sensor.test.TestType;

public abstract class EventGenerator {
    
    private final Sensor sensor = new Sensor(Type.MAGNETIC_FIELD, "TestGenerator");
    private SensorListener listener;
    protected double amp = 1d;
    protected TestType testType;
    protected double freq;
    protected double sf;
    private double noiseLevel;
    
    public EventGenerator(TestSensorConfig testSensorConfig) {
        freq = testSensorConfig.getFreq();
        sf = testSensorConfig.getSf();
        testType = testSensorConfig.getTestType();
        noiseLevel = testSensorConfig.getNoiseLevel();
    }
    
    public abstract void start();
    
    private void addNoise(double[] values) {
        for(int i=0; i < values.length; i ++) {
            values[i] = values[i] + noiseLevel * amp * (Math.random() * 2.0 - 1.0);
        }
    }
    
    protected void newEvent(long timeUS, double angle) {
        
        double[] magVals = new double[3]; 
        magVals[0] = amp * Math.sin(angle);
        magVals[1] = amp * Math.sin(angle+Math.PI/2.0);
        magVals[2] = amp * Math.sin(angle+3*Math.PI/4.0);
        
        addNoise(magVals);
        listener.newEvent(new SensorEvent3D(sensor, timeUS, magVals));
    }
    
    public void setListener(SensorListener listener) {
        this.listener = listener;
    }
}
