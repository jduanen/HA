/********************************************************************************
 * 
 * PIR Sensor Library
 * 
 * PIR sensor connected to an input pin, tracks and returns state and state transitions.
 *
 ********************************************************************************/

#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include <Arduino.h>


class PIRSensor {
public:
    enum PIR_STATE {
        UNDEF,  // undefined
        L,      // no detection
        L2H,    // initial detection
        H,      // active detection
        H2L     // no longer detected
    };

    PIRSensor(int pin);

    PIR_STATE detect();
protected:
    uint8_t _pin;
    PIR_STATE _pirVal = UNDEF;
};

#endif  /*PIR_SENSOR_H*/
