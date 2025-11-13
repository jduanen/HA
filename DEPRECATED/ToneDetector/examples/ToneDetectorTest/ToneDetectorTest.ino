/***************************************************************************
 *
 * Test program for the ToneDetector library
 * 
 ***************************************************************************/

#include <I2S.h>
#include <ToneDetector.h>

//// TODO use logging

#define SCK_PIN             -1  // Clock
#define FS_PIN              42  // Frame Sync
#define SD_PIN              41  // Data
#define OUT_SD_PIN          -1  // Data Input
#define IN_SD_PIN           -1  // Data Output

#define I2S_SAMPLE_RATE     16000
#define BITS_PER_SAMPLE     16

#define FREQ                43.1
#define NUM_SAMPLES         512 // 1024
//#define NUM_SAMPLES         (I2S_SAMPLE_RATE / 1)  // 1sec //// 10) 100msec

#define I2S_BUF_SIZE        NUM_SAMPLES

#define MIN_FREQ            60.0
#define MAX_FREQ            120.0


ToneDetector *td;


bool setupMic() {  //// FIXME take pins as parameters
  I2S.setAllPins(SCK_PIN, FS_PIN, SD_PIN, OUT_SD_PIN, IN_SD_PIN);
//  I2S.setAllPins(-1, 42, 41, -1, -1);  //// FIXME
  if (!I2S.begin(PDM_MONO_MODE, I2S_SAMPLE_RATE, BITS_PER_SAMPLE)) {  // Master Mode
    Serial.println("ERROR: Failed to initialize I2S!");
    return(false);
  }
  if (!I2S.setBufferSize(I2S_BUF_SIZE)) {
    Serial.println("ERROR: Failed to set I2S buffer to " + String(I2S_BUF_SIZE) + " Bytes");
    return(false);
  }
//  I2S.read();
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
    Serial.println(samples[i]);
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

  td = new ToneDetector(FREQ, MIN_FREQ, MAX_FREQ);
  Serial.print("T" + String(FREQ) + ", ");
  td->printSideTones();
  Serial.println("");
//  Serial.println("ToneDetectorTest: BEGIN");
//  Serial.println("T, S0, S1, S2, S3");
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
  printSamples(samples, NUM_SAMPLES);
  td->detect(samples, NUM_SAMPLES);
  delay(10);
}
