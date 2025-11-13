/***************************************************************************
 *
 * Library that detects a tone at a given frequency
 * 
 ***************************************************************************/

ToneDetector::ToneDetector(float frequency, float minFrequency, float maxFrequency) {
  float freq;
  _frequency = frequency;
  _minFreq = minFrequency;
  _maxFreq = maxFrequency;

  _tone = new Goertzel(_frequency);

  for (uint32_t t = 0; (t < NUM_SIDE_TONES); t++) {
    _sideToneFreqs[t] = (_minFreq + (((_maxFreq - _minFreq) * t) / (NUM_SIDE_TONES - 1)));
    _sideTones[t] = new Goertzel(_sideToneFreqs[t]);
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

void ToneDetector::printSideTones() {
  for (int t = 0; (t < NUM_SIDE_TONES); t++) {
    if (t > 0) {
      Serial.print(", ");
    }
    Serial.print("S" + String(_sideToneFreqs[t]));
  }
}
