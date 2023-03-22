#ifndef __GLB_TYPES_H__
#define __GLB_TYPES_H__

#include <string>

using DuplicatedImageInfo = struct duplicatedImageInfo
{
    int bufferViewIndex;
    size_t originalSize;
    std::string hashStr;
};

using DownscaleInfo = struct downscaleInfo
{
    int bufferViewIndex;
    size_t originalSize;
    size_t compressSize;
    double scaleRatio;

    size_t cvMatSize;
    size_t cvMatResizedSize;
};

#endif
