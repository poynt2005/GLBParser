#include "CompressImage.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

bool CompressImageByCV(const std::vector<uint8_t> &inBuffer, const double downScale, std::vector<uint8_t> &outBuffer)
{
    return CompressImageByCV(inBuffer, downScale, "image/jpeg", outBuffer);
}

bool CompressImageByCV(const std::vector<uint8_t> &inBuffer, const double downScale, const std::string &mimeType, std::vector<uint8_t> &outBuffer)
{
    SizeStruct ss;
    return CompressImageByCV(inBuffer, downScale, "image/jpeg", ss, outBuffer);
}

bool CompressImageByCV(const std::vector<uint8_t> &inBuffer, const double downScale, const std::string &mimeType, SizeStruct &ss, std::vector<uint8_t> &outBuffer)
{

    cv::Mat inputMatImage = cv::imdecode(cv::Mat(inBuffer), 1);
    ss.originalSize = inputMatImage.total() * inputMatImage.elemSize();

    cv::Mat outImage;
    cv::resize(inputMatImage, outImage, cv::Size(), downScale, downScale);
    ss.resizedSize = outImage.total() * outImage.elemSize();

    bool isEncodeSuccess;

    if (mimeType == "image/jpeg")
    {
        isEncodeSuccess = cv::imencode(".jpeg", outImage, outBuffer);
    }
    else if (mimeType == "image/png")
    {
        isEncodeSuccess = cv::imencode(".png", outImage, outBuffer);
    }

    inputMatImage.release();
    outImage.release();

    return isEncodeSuccess;
}

void CalculateImageMemSizeByCV(const std::vector<uint8_t> &inBuffer, SizeStruct &ss)
{
    cv::Mat inputMatImage = cv::imdecode(cv::Mat(inBuffer), 1);
    ss.originalSize = inputMatImage.total() * inputMatImage.elemSize();
    ss.resizedSize = 0;
    inputMatImage.release();
}