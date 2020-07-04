#pragma once

#include <Arduino.h>

#include "Global.h"

namespace Scenario {
void reinit();

void init();

void process(StringQueue* events);

void enableBlock(size_t num, boolean enable);

bool isBlockEnabled(size_t num);

}  // namespace Scenario