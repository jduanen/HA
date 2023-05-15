/********************************************************************************
 * 
 * KittyCam: Application that detects if cat is in his house and streams video
 *
 * N.B.
 *  - This is for the XIAO ESP32-S3 Sense board-pair
 *  - Must enable external storage -- 'PSRAM: "OPI PSRAM"'
 *  - Can (optionally) save images to uSD card on arrival (i.e., initial detect)
 * 
 ********************************************************************************/

#include "FS.h"
#include "SD.h"
#include "SPI.h"

/*
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM

#include "esp_camera.h"
#include "camera_pins.h"
*/

#define SD_CS_PIN           21
#define PIR_DETECT_PIN      4


bool setupPIR() {
  pinMode(PIR_DETECT_PIN, INPUT);
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

//// TODO make this all part of the PIRSensor library
namespace PIRSensor {
  enum PIR_STATE {
    UNDEF,  // undefined
    L,      // no detection
    L2H,    // initial detection
    H,      // active detection
    H2L     // no longer detected
  };

  PIR_STATE _pirVal = UNDEF;

  PIR_STATE detect() {
    int val = digitalRead(PIR_DETECT_PIN);

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
  }
};

void setup() {
  delay(500);
  Serial.begin(115200);
  while (!Serial) {;};
  Serial.println("KittyCam: STARTING...");

  if (setupPIR()) {
    Serial.println("PIR Detector Ready");
  }

/*
  if (setupSDcard(SD_CS_PIN)) {
    Serial.println("INFO: SD card ready");
  } else {
    while(1) {};    //// FIXME
  }
*/

/*
  if (setupCamera(&cameraConfig)) {
    Serial.println("Camera ready");
  } else {
    while(1) {};    //// FIXME
  }
*/

  Serial.println("KittyCam: RUN");
}

void loop() {
  unsigned long now = millis();

  switch (PIRSensor::detect()) {
  case PIRSensor::L2H:
    Serial.println("Detected: " + String(now));
    break;
  case PIRSensor::H2L:
    Serial.println("No Longer Detected: " + String(now));
    break;
  }
/*
  PIRSensor::PIR_STATE val = PIRSensor::detect();
  Serial.println(((val == PIRSensor::H) || (val == PIRSensor::L2H)) ? 1 : 0);
*/
}