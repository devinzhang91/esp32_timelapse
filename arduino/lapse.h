#pragma once

void setEnableFlashLight(bool enable);
void setInterval(unsigned long delta);
bool startLapse();
bool stopLapse();
bool processLapse(unsigned long dt);
