#ifndef __CALCULATE_JSON_H__
#define __CALCULATE_JSON_H__

#include <string>
#include <vector>
#include <map>

#include "GLBTypes.h"

// using OutImageInfoJson = struct outImageInfoJson
// {
//     int size;
//     int padding;
//     bool isFake;
// };

// using BufferInfo = struct bufferInfo
// {
//     int startIndex;
//     int endIndex;
//     int outBufferSize;
// };

void CalculateJson(const std::string &, const int, const std::map<int, BufferIndexInfo> &, const std::map<int, int> &, std::string &);

#endif