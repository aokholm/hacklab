package com.vaavud.sensor.test.internal;

import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;

import com.vaavud.sensor.test.TestSensorConfig;

public class EventGeneratorFreq extends EventGenerator {

    private class Point {
        public final int timeUs;
        public final double y;
        
        public Point(double x, double y) {
            this.timeUs = (int) (x * 1_000_000);
            this.y = y;
        }
    }
    
    private List<Point> freqPoints;
    
    public EventGeneratorFreq(TestSensorConfig testSensorConfig) {
        super(testSensorConfig);
        freqPoints = new ArrayList<>();
        
        freqPoints.add(new Point(0, 3.5)); // first time should be 0
        freqPoints.add(new Point(5, 3.5));
        freqPoints.add(new Point(0.3, 21));
        freqPoints.add(new Point(1, 10));
        freqPoints.add(new Point(1, 6));
        freqPoints.add(new Point(1, 3));
        freqPoints.add(new Point(1, 2));
        freqPoints.add(new Point(3, 1));
        freqPoints.add(new Point(5, 0.5));
        freqPoints.add(new Point(6, 1.5));
        
        
    }

    @Override
    public void start() {
        
        int rateUs = (int) (1/sf * 1_000_000);
        
        ListIterator<Point> iterator = freqPoints.listIterator();
        Point prev = null;
        Point current = iterator.next();
        long currentEndUs = current.timeUs;
        double angle = 0; // radians
        
        for (int timeUs = 0; timeUs < currentEndUs || iterator.hasNext(); timeUs+=rateUs) {
            
            while (timeUs >= currentEndUs) {
                prev = current;
                current = iterator.next();
                currentEndUs = currentEndUs + current.timeUs;
            }
            
            // find relative position relative to current
            double relPos = (currentEndUs - timeUs) / (double) current.timeUs;
            double freq = (prev.y-current.y)*relPos + current.y;
            newFreqEvent(timeUs, freq);
            
            angle = angle + Math.PI*2 * freq * rateUs / 1_000_000.0;
            
            newEvent(timeUs, angle);
        }
        
    }
}
