#include "person.h"
#include "esphome/core/log.h"

namespace esphome {
namespace person_sensor {

static const char *const TAG = "person_sensor.sensor";

void PersonSensor::update() { this->read_data_(); }

void PersonSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "Person:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with Person failed!");
  }
  ESP_LOGI(TAG, "Person: %s", this->is_failed() ? "FAILED" : "OK");
  LOG_UPDATE_INTERVAL(this);
}

void PersonSensor::read_data_() {
  person_sensor_results_t results;
  this->read_bytes(PERSON_SENSOR_I2C_ADDRESS, (uint8_t *) &results, sizeof(results));
  ESP_LOGD(TAG, "Person: %d faces detected", results.num_faces);
  this->publish_state(results.num_faces);
  if (results.num_faces == 1) {
    ESP_LOGD(TAG, "Person: is facing towards camera: %d", results.faces[0].is_facing);
  }
}

}  // namespace person_sensor
}  // namespace esphome
