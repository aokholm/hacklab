package com.vaavud.sensor.test;

public class TestSensorConfig {
    
    private Double freq;
    private Double sf;
    private Double noiseLevel;
    private TestType testType;
    
    public TestSensorConfig(String freqString, String sfString, String noiseLevelString,
            String testTypeString ) {
        
        freq = 11d;
        sf = 100d;
        noiseLevel = 0d;
        testType = TestType.PLAIN;
        
        if (freqString != null){
            freq = Double.valueOf(freqString);
        }
        
        if (sfString != null) {
            sf = Double.valueOf(sfString);
        }
        
        if (noiseLevelString != null) {
            noiseLevel = Double.valueOf(noiseLevelString);
        }
        
        if (testTypeString != null) {
            testType = TestType.valueOf(testTypeString);
        }
        
    }
    
    public Double getFreq() {
        return freq;
    }
    
    public Double getNoiseLevel() {
        return noiseLevel;
    }
    
    public Double getSf() {
        return sf;
    }
    
    public TestType getTestType() {
        return testType;
    }
    
}
