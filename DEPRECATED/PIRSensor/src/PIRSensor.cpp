/********************************************************************************
 * 
 * PIR Sensor Library
 * 
 * PIR sensor connected to an input pin, tracks and returns state and state transitions.
 *
 ********************************************************************************/

#include <PIRSensor.h>


PIRSensor::PIRSensor(int pin) {
	_pin = pin;
	pinMode(_pin, INPUT);
};

PIRSensor::PIR_STATE PIRSensor::detect() {
    int val = digitalRead(_pin);

    switch (_pirVal) {
    case UNDEF:
        _pirVal = val ? L : H;
        break;
    case H2L:
    case L:
        _pirVal = val ? L2H : L;
        break;
    case L2H:
    case H:
        _pirVal = val ? H : H2L;
        break;
    }
    return(_pirVal);
};
