#pragma once

#include <Arduino.h>

void initAnimation();
void startAnimation(const String& model);
void stopAnimation();
bool isAnimationRunning();
void runAnimation();

extern bool animationRunning ;
extern uint32_t animationLast;
extern uint16_t animationStep;

const uint32_t HOLD_TIME = 3000;
const uint32_t FADE_TIME = 2000;

extern uint8_t targetColor[4];