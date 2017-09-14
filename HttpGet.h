#pragma once

#include <stdint.h>
#include <vector>

bool HttpGet(const std::string &strURL, std::vector<uint8_t> &data);
