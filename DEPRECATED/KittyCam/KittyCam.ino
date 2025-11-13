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

#include <PIRSensor.h>


/*
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM

#include "esp_camera.h"
#include "camera_pins.h"
*/

#define SD_CS_PIN           21
#define LED_PIN             2
#define PIR_DETECT_PIN      4

#define TRANSITION_TEST     0
#define LEVEL_TEST          1
#define TEST_NUM            TRANSITION_TEST  // LEVEL_TEST


PIRSensor pir(PIR_DETECT_PIN);


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

bool setupCamera(camera_config_t *config) {
  // if PSRAM IC present, init to UXGA and higher JPEG quality
  if(config->pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      Serial.println("PSRAM Found"); //// TMP TMP TMP
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

void setup() {
  delay(500);
  Serial.begin(115200);
  while (!Serial) {;};
  Serial.println("KittyCam: STARTING...");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);

/*
  if (setupSDcard(SD_CS_PIN)) {
    Serial.println("INFO: SD card ready");
  } else {
    while(1) {};    //// FIXME
  }
*/

  if (setupCamera(&cameraConfig)) {
    Serial.println("Camera ready");
  } else {
    while(1) {};    //// FIXME
  }

  Serial.println("KittyCam: RUN");
}

void loop() {
  unsigned long now = millis();
  PIRSensor::PIR_STATE pirState = pir.detect();

  switch (TEST_NUM) {
  case TRANSITION_TEST:
    switch (pirState) {
    case PIRSensor::L2H:
      Serial.print("Detected: "); Serial.println(now);
      digitalWrite(LED_PIN, HIGH);
      break;
    case PIRSensor::H2L:
      Serial.print("No Longer Detected: "); Serial.println(now);
      digitalWrite(LED_PIN, LOW);
      break;
    }
    break;
  case LEVEL_TEST:
    if (true) {
      Serial.print(((pirState == PIRSensor::H) || (pirState == PIRSensor::L2H)) ? "^" : "v");
      digitalWrite(LED_PIN, ((pirState == PIRSensor::H) || (pirState == PIRSensor::L2H)));
    } else {
      Serial.print(((pirState == PIRSensor::H) || (pirState == PIRSensor::L2H)) ? "1" : "0");
      digitalWrite(LED_PIN, ((pirState == PIRSensor::H) || (pirState == PIRSensor::L2H)));
    }
    break;
  }
}