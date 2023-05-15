/********************************************************************************
 * 
 * (Anna's) Hummingbird Detector
 *
 * N.B.
 *  - This is for the XIAO ESP32-S3 Sense board-pair
 *  - Must enable external storage -- 'PSRAM: "OPI PSRAM"'
 * 
 ********************************************************************************/

#include <I2S.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

/*
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM

#include "esp_camera.h"
#include "camera_pins.h"
*/


#define SD_CS_PIN           21

#define I2S_SAMPLE_RATE     16000
#define NUM_SAMPLES         (I2S_SAMPLE_RATE / 1)  // 1sec //// 10) 100msec
#define BITS_PER_SAMPLE     16

#define BATTERY_ADC_PIN     A0

#define TONE_FREQ           43.1  // (+/- 1.4Hz) Calypte anna //// TODO add citation
#define TONE_THRESHOLD      0.1   // 10% above average of adjascent tones
#define NUM_ADJACENT_TONES  4     // two on either side


#ifdef abs
#undef abs
#endif
#define abs(x)  ((x) > 0 ? (x) : -(x))


unsigned long lastSampled = 0;

float toneAvg;

ToneDetector detector(TONE_FREQ, TONE_THRESHOLD);

int samples[NUM_SAMPLES];


bool setupMic() {
  I2S.setAllPins(-1, 42, 41, -1, -1);  //// FIXME
  if (!I2S.begin(PDM_MONO_MODE, I2S_SAMPLE_RATE, BITS_PER_SAMPLE)) {
    Serial.println("ERROR: Failed to initialize I2S!");
    return(false);
  }
  return(true);
}

bool setupSDcard(uint32_t csPin) {
  if (!SD.begin(csPin)){
    Serial.println("ERROR: Card Mount Failed");
    return(false);
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE){
    Serial.println("ERROR: No SD card attached");
    return(false);
  }

  Serial.print("INFO: SD Card Type: ");
  if (cardType == CARD_MMC){
    Serial.println("MMC");
  } else if (cardType == CARD_SD){
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  return(true);
}

void writeFile(fs::FS &fs, const char *path, uint8_t *data, size_t len){
  Serial.printf("INFO: Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file){
    Serial.println("INFO: Failed to open file for writing");
    return;
  }
  if (file.write(data, len) == len){
    Serial.println("INFO: File written");
  } else {
    Serial.println("INFO: Write failed");
  }
  file.close();
}

/*
exists()
mkdir()
remove()
rmdir()
name()
available()
read()
isDirectory()
*/

/*
bool setupCamera(camera_config_t *config) {
  // if PSRAM IC present, init to UXGA and higher JPEG quality
  if(config->pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config->jpeg_quality = 10;
      config->fb_count = 2;
      config->grab_mode = CAMERA_GRAB_LATEST;
    } else {
      //if PSRAM not present, reduce size of image to SVGA
      config->frame_size = FRAMESIZE_SVGA;
      config->fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config->frame_size = FRAMESIZE_240X240;
#if cameraConfig_IDF_TARGET_ESP32S3
    config->fb_count = 2;
#endif
  }

  // camera init
  esp_err_t err = esp_camera_init(config);
  if (err != ESP_OK) {
    Serial.printf("ERROR: Camera init failed with error 0x%x", err);
    return(false);
  }
  return(true);
}
*/

float batteryAvgVolts(int count=16) {
  uint32_t vBatt = 0;
  for(int i = 0; i < count; i++) {
    vBatt = vBatt + analogReadMilliVolts(BATTERY_ADC_PIN);
  }
  return(2.0 * vBatt / count / 1000.0);     // attenuation ratio 1/2, mV --> V
}

float goertzel(uint32_t numSamples=NUM_SAMPLES, float toneFreq=TONE_FREQ) {
//  int samples[numSamples];  //// FIXME alloc in external PSRAM, not on stack

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
  while (!Serial) {;};
  Serial.println("HBSensor Starting...");

  pinMode(LED_BUILTIN, OUTPUT);    // USER_LED
  pinMode(BATTERY_ADC_PIN, INPUT); // ADC

  if (setupMic()) {
    Serial.println("INFO: Microphone ready");
  } else {
    while(1) {};    //// FIXME
  }

  if (setupSDcard(SD_CS_PIN)) {
    Serial.println("INFO: SD card ready");
  } else {
    while(1) {};    //// FIXME
  }

/*
  if (setupCamera(&cameraConfig)) {
    Serial.println("Camera ready");
  } else {
    while(1) {};    //// FIXME
  }
*/

  digitalWrite(LED_BUILTIN, LOW);
  delay(3000);
  (void)getToneAverage(10);  // discard first second of grabs
  toneAvg = getToneAverage(TONE_GRABS);
  Serial.println("TA: " + String(toneAvg));
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("INFO: tone detector initialized");
}

void loop() {
  unsigned long now = millis();

#ifdef BATTERY_MONITOR
#define SAMPLE_INTERVAL     1000

  if (now >= (lastSampled + SAMPLE_INTERVAL)) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Volts: " + String(batteryAvgVolts(), 3));
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    lastSampled = now;
  }
#endif

  float toneMag = goertzel();
  Serial.println(toneMag);
  if (abs(toneMag - toneAvg) > (toneAvg * TONE_THRESHOLD)) {
//    Serial.println("TD: " + String(abs(toneMag - toneAvg)) + ", TT:" + String(toneAvg * TONE_THRESHOLD));
    //// TODO capture image and write it to file
//    Serial.println("Trigger image capture(s)");
  }
}

/***************************************************************
unsigned long lastCaptureTime = 0; // Last shooting time
int imageCount = 1;                // File Counter
bool camera_sign = false;          // Check camera status
bool sd_sign = false;              // Check sd status

// Save pictures to SD card
void photo_save(const char * fileName) {
  // Take a photo
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to get camera frame buffer");
    return;
  }
  // Save photo to file
  writeFile(SD, fileName, fb->buf, fb->len);
  
  // Release image buffer
  esp_camera_fb_return(fb);

  Serial.println("Photo saved to file");
}

void loop() {
  // Camera & SD available, start taking pictures
  if(camera_sign && sd_sign){
    // Get the current time
    unsigned long now = millis();
  
    //If it has been more than 1 minute since the last shot, take a picture and save it to the SD card
    if ((now - lastCaptureTime) >= 60000) {
      char filename[32];
      sprintf(filename, "/image%d.jpg", imageCount);
      photo_save(filename);
      Serial.printf("Saved picture：%s\n", filename);
      Serial.println("Photos will begin in one minute, please be ready.");
      imageCount++;
      lastCaptureTime = now;
    }
  }
}
***************************************************************/
