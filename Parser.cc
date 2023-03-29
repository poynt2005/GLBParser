#include "SerializeBinary.h"

#include <iostream>
#include <unordered_map>
#include <map>
#include <set>
#include <bitset>
#include <math.h>

#include "Parser.h"
#include "CalculateHash.h"
#include "CalculateJson.h"
#include "ParserStream.h"
#include "CompressImage.h"

Parser::Parser() {}

Parser::~Parser() {}

void Parser::CalculateDulpicatedImage(const Microsoft::glTF::Document &document, const Microsoft::glTF::GLBResourceReader &resourceReader)
{

    std::unordered_map<std::string, int> hashStringMapping;
    std::set<std::string> hashStringSet;
    for (int i = 0; i < document.images.Elements().size(); i++)
    {
        auto image = std::move(document.images.Elements()[i]);
        int currentImageBufferIndex = std::stoi(image.bufferViewId);

        if (i == 0)
        {
            originalImageStartOffset = document.bufferViews[currentImageBufferIndex].byteOffset;
            originalImageStartBufferViewIndex = currentImageBufferIndex;
        }

        if (i == document.images.Elements().size() - 1)
        {
            originalImageEndOffset = document.bufferViews[currentImageBufferIndex + 1].byteOffset;
            originalImageEndBufferViewIndex = currentImageBufferIndex;
        }

        std::string sha512String;
        auto data = resourceReader.ReadBinaryData(document, image);
        GetSHA512StringValue(data.data(), data.size(), sha512String);

        if (!hashStringSet.count(sha512String))
        {
            hashStringSet.insert(sha512String);
            hashStringMapping.insert(std::pair<std::string, int>(sha512String, i));

            int padding = CalculatePadding(data.size());

            auto imgBufferSize = data.size();

            OutImageInfo imgInfo{
                imgBufferSize,
                padding,
                currentImageBufferIndex,
                std::move(data),
                std::move(image.mimeType)};

            imageMapping.insert(std::pair<int, int>(i, i));

            outImageChunk.emplace_back(std::move(imgInfo));

            outImageBufferSize += (padding + imgBufferSize);
        }
        else
        {
            const int mapTargetIdx = hashStringMapping[sha512String];

            /********* with padding begin*********/
            OutImageInfo imgInfo{
                0,
                4,
                currentImageBufferIndex,
                std::vector<uint8_t>{},
                std::move(document.images.Elements()[mapTargetIdx].mimeType)};

            outImageChunk.emplace_back(std::move(imgInfo));

            // outImageBufferSize += (4);
            /*********** with padding end************/

            imageMapping.insert(std::pair<int, int>(i, mapTargetIdx));

            DuplicatedImageInfo dInfo{
                currentImageBufferIndex,
                data.size(),
                sha512String};

            duplicatedImageInfos.emplace_back(std::move(dInfo));
        }
    }
}

void Parser::CalculateBufferOffset(const Microsoft::glTF::Document &document, const Microsoft::glTF::GLBResourceReader &resourceReader)
{
    bufferStartOffset = resourceReader.GetBinaryStreamPos(Microsoft::glTF::Buffer());
    auto lastBufferView = document.bufferViews[document.bufferViews.Size() - 1];
    originalBufferSize = lastBufferView.byteOffset + lastBufferView.byteLength;
}

void Parser::ReadNonImageBuffer(const Microsoft::glTF::Document &document, const Microsoft::glTF::GLBResourceReader &resourceReader)
{
    int prevBufferNum = 0, postBufferNum = 0;
    for (int i = 0; i < document.bufferViews.Size(); i++)
    {
        if (i < originalImageStartBufferViewIndex)
        {
            prevBufferSize += document.bufferViews[i].byteLength;
            prevBufferNum++;
        }
        else if (i > originalImageEndBufferViewIndex)
        {
            postBufferSize += document.bufferViews[i].byteLength;
            postBufferNum++;
        }
    }

    prevBuffer.resize(prevBufferNum);
    postBuffer.resize(postBufferNum);
    for (int i = 0; i < document.bufferViews.Size(); i++)
    {
        if (i < originalImageStartBufferViewIndex)
        {
            auto buf = resourceReader.ReadBinaryData<uint8_t>(document, std::move(document.bufferViews[i]));
            auto bufSize = buf.size();
            prevBuffer[i] = std::move(buf);
        }
        else if (i > originalImageEndBufferViewIndex)
        {
            auto buf = resourceReader.ReadBinaryData<uint8_t>(document, std::move(document.bufferViews[i]));
            auto bufSize = buf.size();
            postBuffer[i - originalImageEndBufferViewIndex - 1] = std::move(buf);
        }
    }
}

void Parser::ReConstructBuffer()
{
    outputBufferSize = prevBufferSize + postBufferSize + outImageBufferSize;
    outBuffer = std::make_unique<uint8_t[]>(outputBufferSize);
    size_t currentOffset = 0;

    int currentBufferViewIndex = 0;
    int originalBufferViexIndex = 0;

    for (const auto &prevBuf : prevBuffer)
    {
        BufferIndexInfo bufIdxInfo{
            currentBufferViewIndex,
            currentOffset,
            prevBuf.size()};
        bufferIndexMapping.insert(std::pair<int, BufferIndexInfo>(originalBufferViexIndex, std::move(bufIdxInfo)));
        memcpy(&(outBuffer.get()[currentOffset]), prevBuf.data(), prevBuf.size());

        currentOffset += prevBuf.size();

        currentBufferViewIndex++;
        originalBufferViexIndex++;
    }

    for (const auto &chunk : outImageChunk)
    {

        if (chunk.size == 0)
        {
            BufferIndexInfo bufIdxInfo{
                0,
                0,
                0};
            bufferIndexMapping.insert(std::pair<int, BufferIndexInfo>(originalBufferViexIndex, std::move(bufIdxInfo)));
            originalBufferViexIndex++;
            continue;
        }

        BufferIndexInfo bufIdxInfo{
            currentBufferViewIndex,
            currentOffset,
            chunk.size};
        bufferIndexMapping.insert(std::pair<int, BufferIndexInfo>(originalBufferViexIndex, std::move(bufIdxInfo)));

        memcpy(
            &(outBuffer.get()[currentOffset]),
            chunk.data.data(),
            chunk.size);

        currentOffset += chunk.size;

        if (chunk.padding > 0)
        {
            std::vector<uint8_t> paddingZero(chunk.padding, static_cast<uint8_t>(0));
            memcpy(
                &(outBuffer.get()[currentOffset]),
                paddingZero.data(),
                chunk.padding);

            currentOffset += chunk.padding;
        }

        currentBufferViewIndex++;
        originalBufferViexIndex++;
    }

    for (const auto &postBuf : postBuffer)
    {
        BufferIndexInfo bufIdxInfo{
            currentBufferViewIndex,
            currentOffset,
            postBuf.size()};
        bufferIndexMapping.insert(std::pair<int, BufferIndexInfo>(originalBufferViexIndex, std::move(bufIdxInfo)));
        memcpy(&(outBuffer.get()[currentOffset]), postBuf.data(), postBuf.size());
        currentOffset += postBuf.size();

        currentBufferViewIndex++;
        originalBufferViexIndex++;
    }
}

int Parser::CalculatePadding(const size_t byteLength)
{
    const int alignmentSize = Microsoft::glTF::GLB_CHUNK_ALIGNMENT_SIZE;
    const auto padAlign = byteLength % alignmentSize;
    const auto pad = padAlign ? alignmentSize - padAlign : 0;
    return pad;
}

void Parser::CalculateJsonChunk(const std::string &jsonStr)
{
    CalculateJson(jsonStr, outputBufferSize, bufferIndexMapping, imageMapping, outJson);
}

void Parser::CalculateImageMemorySize()
{
    for (const auto &chunk : outImageChunk)
    {
        SizeStruct ss;
        if (chunk.size)
        {
            CalculateImageMemSizeByCV(chunk.data, ss);
            std::pair<int, size_t> memSize(chunk.bufferViewIndex, ss.originalSize);
            imageMemoryInfos.emplace_back(std::move(memSize));
        }
    }
}

void Parser::CompressImage(const double downscaleRatio)
{
    if (downscaleRatio >= 1)
    {
        return;
    }

    for (auto &chunk : outImageChunk)
    {
        std::vector<uint8_t> compressedImage;

        SizeStruct cvSize;

        if (chunk.size > 0 && CompressImageByCV(chunk.data, downscaleRatio, chunk.mimeType, cvSize, compressedImage))
        {
            const auto compressedImageSize = compressedImage.size();
            const auto compressedImageSizePadding = CalculatePadding(compressedImageSize);
            const auto originalImageSize = chunk.size;
            outImageBufferSize -= (chunk.size + chunk.padding);

            chunk.size = compressedImageSize;
            chunk.padding = compressedImageSizePadding;
            chunk.data = std::move(compressedImage);
            // chunk.mimeType = "image/jpeg";

            outImageBufferSize += (chunk.size + chunk.padding);

            DownscaleInfo dInfo{
                chunk.bufferViewIndex,
                originalImageSize,
                chunk.size,
                downscaleRatio,
                cvSize.originalSize,
                cvSize.resizedSize};

            downscaleInfos.emplace_back(std::move(dInfo));
        }
    }
}

void Parser::CompressImageByUpperBound(const size_t upperBound, const size_t jsonManifestLength)
{
    if (imageMemoryInfos.empty())
    {
        return;
    }

    if (upperBound <= 0)
    {
        return;
    }
    const auto tolerableImageSize = upperBound - (prevBufferSize + postBufferSize) - jsonManifestLength;
    size_t currentImageMemoryTotalSize = 0;

    std::map<int, double> bufferViewMapSize;

    for (const auto &info : imageMemoryInfos)
    {
        currentImageMemoryTotalSize += info.second;
    }

    for (const auto &info : imageMemoryInfos)
    {
        const auto imageMemorySize = (static_cast<double>(info.second) / static_cast<double>(currentImageMemoryTotalSize)) * static_cast<double>(tolerableImageSize);
        const double downscaleRatio = std::round(std::sqrt(imageMemorySize / info.second) * 100) / 100;
        bufferViewMapSize.insert(std::pair<int, double>(info.first, downscaleRatio));
    }

    for (auto &chunk : outImageChunk)
    {
        std::vector<uint8_t> compressedImage;

        SizeStruct cvSize;

        auto it = bufferViewMapSize.find(chunk.bufferViewIndex);

        if (chunk.size > 0 && CompressImageByCV(chunk.data, it->second, chunk.mimeType, cvSize, compressedImage))
        {
            const auto compressedImageSize = compressedImage.size();
            const auto compressedImageSizePadding = CalculatePadding(compressedImageSize);
            const auto originalImageSize = chunk.size;
            outImageBufferSize -= (chunk.size + chunk.padding);

            chunk.size = compressedImageSize;
            chunk.padding = compressedImageSizePadding;
            chunk.data = std::move(compressedImage);
            // chunk.mimeType = "image/jpeg";

            outImageBufferSize += (chunk.size + chunk.padding);

            DownscaleInfo dInfo{
                chunk.bufferViewIndex,
                originalImageSize,
                chunk.size,
                it->second,
                cvSize.originalSize,
                cvSize.resizedSize};

            downscaleInfos.emplace_back(std::move(dInfo));
        }
    }
}

void Parser::writeFile(const std::string &filename)
{
    std::ofstream outGlb(filename, std::ios::binary);

    /** calculate json chunk padding*/
    auto jsonChunkLength = outJson.length();
    auto jsonPaddingLength = CalculatePadding(jsonChunkLength);
    jsonChunkLength += jsonPaddingLength;

    /** calculate binary chunk padding*/
    auto binaryChunkLength = outputBufferSize;
    auto binaryPaddingLength = CalculatePadding(outputBufferSize);
    binaryChunkLength += binaryPaddingLength;

    /** calculate glb length*/
    uint32_t glbLength = Microsoft::glTF::GLB_HEADER_BYTE_SIZE + jsonChunkLength + sizeof(binaryChunkLength) + Microsoft::glTF::GLB_CHUNK_TYPE_SIZE + binaryChunkLength;

    /** Chunk 0, headers 12 bytes*/
    outGlb.write(Microsoft::glTF::GLB_HEADER_MAGIC_STRING, Microsoft::glTF::GLB_CHUNK_TYPE_SIZE);
    auto GLB_HEADER_VERSION_2 = Microsoft::glTF::GLB_HEADER_VERSION_2;
    outGlb.write(reinterpret_cast<char *>(&GLB_HEADER_VERSION_2), Microsoft::glTF::GLB_CHUNK_TYPE_SIZE);
    outGlb.write(reinterpret_cast<char *>(&glbLength), Microsoft::glTF::GLB_CHUNK_TYPE_SIZE);

    /** Chunk 1, json length, json identifier string*/
    outGlb.write(reinterpret_cast<char *>(&jsonChunkLength), Microsoft::glTF::GLB_CHUNK_TYPE_SIZE);
    outGlb.write(Microsoft::glTF::GLB_CHUNK_TYPE_JSON, Microsoft::glTF::GLB_CHUNK_TYPE_SIZE);

    /** Chunk 1, json chunk content*/
    outGlb.write(outJson.data(), outJson.length());

    /** Chunk 1, padding aligement of 4*/
    if (jsonPaddingLength > 0)
    {
        outGlb.write(std::string(jsonPaddingLength, ' ').data(), jsonPaddingLength);
    }

    /** Chunk 2, binary chunk length, Glb bin identidier string*/
    outGlb.write(reinterpret_cast<char *>(&binaryChunkLength), Microsoft::glTF::GLB_CHUNK_TYPE_SIZE);
    outGlb.write(Microsoft::glTF::GLB_CHUNK_TYPE_BIN, Microsoft::glTF::GLB_CHUNK_TYPE_SIZE);

    /** Chunk 2, binary chunk content*/
    outGlb.write(reinterpret_cast<char *>(outBuffer.get()), outputBufferSize);

    /** Chunk 2: Once again, chunk length padding aligment*/
    if (binaryPaddingLength > 0)
    {
        std::vector<uint8_t> binaryPaddingData(binaryPaddingLength, static_cast<uint8_t>(0));
        outGlb.write(reinterpret_cast<char *>(binaryPaddingData.data()), binaryChunkLength);
    }

    outGlb.close();
}

void Parser::GetDuplicatedInfo(std::vector<DuplicatedImageInfo> &infos)
{
    infos = std::vector<DuplicatedImageInfo>(duplicatedImageInfos.begin(), duplicatedImageInfos.end());
}

void Parser::GetDownscaledInfo(std::vector<DownscaleInfo> &infos)
{
    infos = std::vector<DownscaleInfo>(downscaleInfos.begin(), downscaleInfos.end());
}

void Parser::GetImageMemInfo(std::vector<std::pair<int, size_t>> &infos)
{
    infos = std::vector<std::pair<int, size_t>>(imageMemoryInfos.begin(), imageMemoryInfos.end());
}

void Parser::GetOutputManifest(std::string &outManifest)
{
    outManifest = outJson;
}

// int main()
// {

//     auto streamReader = std::make_shared<StreamReader>("./test3.glb");
//     auto glbStream = streamReader->GetInputStream();
//     glbStream->seekg(0, std::ios::end);
//     auto glbSize = glbStream->tellg();
//     glbStream->seekg(std::ios::beg);
//     auto glbResourceReader = std::make_shared<Microsoft::glTF::GLBResourceReader>(streamReader, glbStream);
//     std::string manifest = std::move(glbResourceReader->GetJson());

//     auto startAt = glbResourceReader->GetBinaryStreamPos(Microsoft::glTF::Buffer());

//     auto document = Microsoft::glTF::Deserialize(manifest);

//     Parser p;

//     p.CalculateDulpicatedImage(document, *(glbResourceReader.get()));
//     p.CalculateBufferOffset(document, *(glbResourceReader.get()));
//     p.ReadNonImageBuffer(document, *(glbResourceReader.get()));
//     p.CompressImage(0.1);
//     p.ReConstructBuffer();
//     p.CalculateJsonChunk(manifest);
//     p.writeFile("outglb235.glb");

//     std::cout << "End\n";
//     return 0;
// }