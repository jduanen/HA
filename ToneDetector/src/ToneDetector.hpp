/***************************************************************************
 *
 * Library that detects a tone at a given frequency
 * 
 ***************************************************************************/

ToneDetector::ToneDetector(const float frequency, const uint32_t numSideTones, const uint32_t numSamples) {
  Serial.println("TD: CREATE");
  _frequency = frequency;
  _numSideTones = numSideTones;
  _numSamples = numSamples;

  _samples = (int *)malloc(sizeof(int) * numSamples);

  _tone = new Goertzel(_frequency);
  for (uint32_t i = 0; (i < _numSideTones); i++) {
    _notTones[i] = new Goertzel(_frequency);  //// FIXME split sidetones above and below the main tone
  }
  Serial.println("TD: DONE");
}

bool ToneDetector::detect() {
  //// FIXME
  Serial.println("DETECT");
  return(false);
}

void ToneDetector::setThreshold(const float threshold) {
  _threshold = threshold;
}

/*
  for (int i = 0; (i < numSamples); i++) {
    samples[i] = I2S.read();
  }

  return(ToneDetector->Mag(samples, numSamples));
}
*/
