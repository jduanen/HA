/*
 * (Anna's) Hummingbird Detector
 */

#include <Goertzel.h>
#include <I2S.h>


#define I2S_SAMPLE_RATE   16000
#define NUM_SAMPLES       (I2S_SAMPLE_RATE / 10)  // 100msec
#define BITS_PER_SAMPLE   16

#define BATTERY_ADC_PIN   A0

//// TODO add citation
#define TONE_FREQ         43.1    // (+/- 1.4Hz) Calypte anna
#define TONE_GRABS        30      // set avg base on first 3sec of sound

#define SAMPLE_INTERVAL   1000

#define TONE_THRESHOLD    0.1   // 10% above average

#ifdef abs
#undef abs
#endif
#define abs(x)  ((x) > 0 ? (x) : -(x))


unsigned long lastSampled = 0;

float toneAvg;

Goertzel *ToneDetector = NULL;


float batteryAvgVolts(int count=16) {
  uint32_t vBatt = 0;
  for(int i = 0; i < count; i++) {
    vBatt = vBatt + analogReadMilliVolts(BATTERY_ADC_PIN);
  }
  return(2.0 * vBatt / count / 1000.0);     // attenuation ratio 1/2, mV --> V
}

float goertzel(uint32_t numSamples=NUM_SAMPLES, float toneFreq=TONE_FREQ) {
  int samples[numSamples];  //// FIXME alloc in external PSRAM, not on stack

  if ((ToneDetector == NULL) ||
      ((ToneDetector != NULL) && (ToneDetector->FREQ != toneFreq))) {
    ToneDetector = new Goertzel(toneFreq);
  }

  for (int i = 0; (i < numSamples); i++) {
    samples[i] = I2S.read();
  }

  return(ToneDetector->Mag(samples, numSamples));
}

float getToneAverage(uint32_t numToneGrabs) {
  float toneMags = 0;
  for (uint32_t i = 0; (i < numToneGrabs); i++) {
    toneMags += goertzel();
  }
  return(toneMags / numToneGrabs);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; //// FIXME
  }

  pinMode(LED_BUILTIN, OUTPUT); // USER_LED
  pinMode(A0, INPUT);           // ADC

  I2S.setAllPins(-1, 42, 41, -1, -1);  //// FIXME
  if (!I2S.begin(PDM_MONO_MODE, I2S_SAMPLE_RATE, BITS_PER_SAMPLE)) {
    Serial.println("Failed to initialize I2S!");
    while (1); //// FIXME
  }

  digitalWrite(LED_BUILTIN, LOW);
  delay(3000);
  (void)getToneAverage(10);  // discard first second of grabs
  toneAvg = getToneAverage(TONE_GRABS);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  unsigned long now = millis();

#ifdef UNDEF
  if (now >= (lastSampled + SAMPLE_INTERVAL)) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Volts: " + String(batteryAvgVolts(), 3));
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    lastSampled = now;
  }
#endif

  float toneMag = goertzel();
  if (abs(toneMag - toneAvg) > (toneAvg * TONE_THRESHOLD)) {
    //// TODO capture image and write it to file
    Serial.println("Trigger image capture(s)");
  }
}
