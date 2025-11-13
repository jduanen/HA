/***************************************************************************
 *
 * Library that detects a tone at a given frequency
 * 
 * ????
 * 
 ***************************************************************************/

#pragma once

#include <Goertzel.h>


#define NUM_SIDE_TONES      4
#define NUM_TONES           (NUM_SIDE_TONES + 1)


class ToneDetector {
public:
  ToneDetector(float frequency, float minFrequency, float maxFrequency);

  bool detect(int samples[], uint32_t numSamples);
  void setThreshold(float threshold);
  void printSideTones();

protected:
  float _frequency;
  float _threshold;
  float _minFreq;
  float _maxFreq;

  float _sideToneFreqs[NUM_SIDE_TONES];

  Goertzel *_tone;
  Goertzel *_sideTones[NUM_SIDE_TONES];
};

#include "ToneDetector.hpp"
