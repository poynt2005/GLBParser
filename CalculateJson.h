#ifndef __CALCULATE_JSON_H__
#define __CALCULATE_JSON_H__

#include <string>
#include <vector>
#include <map>

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

void CalculateJson(const std::string &, const int, const std::vector<std::pair<int, int>> &, const std::map<int, int> &, const std::vector<std::string> &, std::string &);

#endif