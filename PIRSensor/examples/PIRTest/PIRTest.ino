/********************************************************************************
 * 
 * PIR Sensor Library Test Application
 * 
 ********************************************************************************/

#include <PIRSensor.h>


#define TEST_NUM					LEVEL_TEST

#define TRANSITION_TEST		0
#define LEVEL_TEST				1

#define PIR_SENSOR_PIN		4


PIRSensor pir(PIR_SENSOR_PIN);


void setup() {
  delay(500);
  Serial.begin(115200);
  while (!Serial) {;};
  Serial.println("PIRTest: STARTING...");

  Serial.println("PIRTest: RUN");
};


void loop() {
  unsigned long now = millis();
  PIRSensor::PIR_STATE pirState = pir.detect();

  switch (TEST_NUM) {
  case TRANSITION_TEST:
	  switch (pirState) {
	  case PIRSensor::L2H:
    	Serial.print("Detected: "); Serial.println(now);
	    break;
  	case PIRSensor::H2L:
    	Serial.print("No Longer Detected: "); Serial.println(now);
    	break;
    }
    break;
  case LEVEL_TEST:
    Serial.print(((pirState == PIRSensor::H) || (pirState == PIRSensor::L2H)) ? "^" : "v");
  	break;
  }
};
