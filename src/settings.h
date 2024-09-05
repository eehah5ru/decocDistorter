#pragma once
//
// settings and params are here
//
#include "ofColor.h"
#include <cstddef>

constexpr const bool APP_ASYNC_MODE = false;

constexpr const std::size_t APP_WIDTH {1920};
constexpr const std::size_t APP_HEIGHT {1080};

// CIRCLE RADIUS FOR Circle Video Source
// in pixels
constexpr const int CIRCLE_RADIUS = 200;
const ofColor CIRCLE_COLOR = ofColor::red;
const ofColor CIRCLE_BKG_COLOR = ofColor::wheat;

// in percents 0..1
constexpr const float CIRCLE_SPEED = 0.001;
