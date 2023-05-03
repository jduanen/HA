/***************************************************************************
 *
 * Test program for the ToneDetector library
 * 
 ***************************************************************************/

#include <ToneDetector.h>


#define FREQ						40
#define NUM_SIDE_TONES	4
#define NUM_SAMPLES			1024


ToneDetector *td;


void setup() {
  Serial.begin(115200);
  while (!Serial) {;};
  Serial.println("ToneDetectorTest Starting...");

  td = new ToneDetector(FREQ, NUM_SIDE_TONES, NUM_SAMPLES);
  Serial.println("TDT: BEGIN");
}

void loop() {
	Serial.println("LOOP");
	td->detect();
	delay(1000);
}
