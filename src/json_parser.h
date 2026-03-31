#pragma once

#include "data_model.h"

// Parse the beans/recipes MQTT payload into DisplayData.
bool parseDisplayData(const char *recipesJson, DisplayData &out);
