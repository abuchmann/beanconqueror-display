#pragma once

#include "data_model.h"

// Parse raw MQTT payloads into DisplayData.
// Groups recent brews by bean_name, picks the most recent per bean,
// and resolves roaster from the active beans list.
bool parseDisplayData(const char *brewsJson, const char *beansJson, DisplayData &out);
