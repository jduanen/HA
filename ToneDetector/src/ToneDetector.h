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
  ToneDetector(float frequency);

  bool detect(int samples[], uint32_t numSamples);
  void setThreshold(float threshold);

protected:
  float _frequency;
  float _threshold;

  Goertzel *_tone;
  Goertzel *_sideTones[NUM_SIDE_TONES];
};

#include "ToneDetector.hpp"
