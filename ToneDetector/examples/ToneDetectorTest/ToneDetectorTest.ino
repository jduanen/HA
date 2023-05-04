/***************************************************************************
 *
 * Test program for the ToneDetector library
 * 
 ***************************************************************************/

#include <I2S.h>
#include <ToneDetector.h>

//// TODO use logging

#define I2S_SAMPLE_RATE     16000
#define BITS_PER_SAMPLE     16

#define FREQ								43.1
#define NUM_SAMPLES					128 // 1024
//#define NUM_SAMPLES         (I2S_SAMPLE_RATE / 1)  // 1sec //// 10) 100msec


ToneDetector *td;


bool setupMic() {  //// FIXME take pins as parameters
  I2S.setAllPins(-1, 42, 41, -1, -1);  //// FIXME
  if (!I2S.begin(PDM_MONO_MODE, I2S_SAMPLE_RATE, BITS_PER_SAMPLE)) {
    Serial.println("ERROR: Failed to initialize I2S!");
    return(false);
  }
//	I2S.read();
  return(true);
}

/*
void readMic(int *samples, int numSamples) {
	int read;
	int available;
	int toRead = numSamples;

  while (toRead > 0) {
  	available = I2S.available();
    read = (available >= toRead) ? toRead : available;
  	toRead -= I2S.read(samples, read);
  }
}
*/
void readMic(int *samples, int numSamples) {
	for (int i = 0; (i < numSamples); i++) {
		samples[i] = I2S.read();
//		Serial.println(samples[i]);
	}
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {;};
  Serial.println("ToneDetectorTest Starting...");

  if (setupMic()) {
    Serial.println("INFO: Microphone ready");
  } else {
    while(1) {};    //// FIXME
  }

  td = new ToneDetector(FREQ);
  Serial.println("ToneDetectorTest: BEGIN");
}

void printSamples(int samples[], int numSamples) {
  Serial.print(">");
  for (int i = 0; (i < numSamples); i++) {
    Serial.print(String(samples[i]) + ", ");
  }
  Serial.println("");
}

void loop() {
	int samples[NUM_SAMPLES];
	readMic(samples, NUM_SAMPLES);
//	printSamples(samples, NUM_SAMPLES);
	td->detect(samples, NUM_SAMPLES);
	delay(1000);
}
