/***************************************************************************
 *
 * Library that detects a tone at a given frequency
 * 
 ***************************************************************************/

ToneDetector::ToneDetector(float frequency) {
  _frequency = frequency;

  _tone = new Goertzel(_frequency);

  for (uint32_t t = 0; (t < NUM_SIDE_TONES); t++) {
    _sideTones[t] = new Goertzel(_frequency);  //// FIXME split sidetones above and below the main tone
  }
}

bool ToneDetector::detect(int samples[], uint32_t numSamples) {
  float mag;
  float mags[NUM_TONES];

  mag = _tone->Mag(samples, numSamples);
  Serial.print(mag);
  for (int t = 0; (t < NUM_SIDE_TONES); t++) {
    mags[t] = _sideTones[t]->Mag(samples, numSamples);
    Serial.print(", ");
    Serial.print(mags[t]);
  }
  Serial.println();
  return(false);
}

void ToneDetector::setThreshold(float threshold) {
  _threshold = threshold;
}
