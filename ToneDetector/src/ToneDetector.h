/***************************************************************************
 *
 * Library that detects a tone at a given frequency
 * 
 * ????
 * 
 ***************************************************************************/

#pragma once

#include <Goertzel.h>


class ToneDetector {
public:
  ToneDetector(const float frequency, const uint32_t numSideTones, const uint32_t numSamples);

  bool detect();
  void setThreshold(const float threshold);

protected:
  float _frequency;
  uint32_t _numSideTones;
  uint32_t _numSamples;
  float _threshold;
  int *_samples;

  Goertzel *_tone;
  Goertzel *_notTones[];
};

#include "ToneDetector.hpp"
