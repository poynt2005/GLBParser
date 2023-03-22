#ifndef __COMPRESS_IMAGE_H__
#define __COMPRESS_IMAGE_H__

#include <vector>
#include <string>

using SizeStruct = struct sizeStruct
{
    size_t originalSize;
    size_t resizedSize;
};

bool CompressImageByCV(const std::vector<uint8_t> &, const double, std::vector<uint8_t> &);
bool CompressImageByCV(const std::vector<uint8_t> &, const double, const std::string &, std::vector<uint8_t> &);
bool CompressImageByCV(const std::vector<uint8_t> &, const double, const std::string &, SizeStruct &, std::vector<uint8_t> &);

void CalculateImageMemSizeByCV(const std::vector<uint8_t> &, SizeStruct &);
#endif