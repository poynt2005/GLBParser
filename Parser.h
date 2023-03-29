#ifndef __PARSER_H__
#define __PARSER_H__

#ifdef BUILDPARSERAPI
#define EXPORTPARSERAPI __declspec(dllexport)
#else
#define EXPORTPARSERAPI __declspec(dllimport)
#endif

#include <map>
#include <vector>
#include <string>

#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>

#include "GLBTypes.h"

class EXPORTPARSERAPI Parser
{
public:
    Parser();
    ~Parser();
    int CalculatePadding(const size_t);
    void CalculateDulpicatedImage(const Microsoft::glTF::Document &, const Microsoft::glTF::GLBResourceReader &);
    void CalculateBufferOffset(const Microsoft::glTF::Document &, const Microsoft::glTF::GLBResourceReader &);
    void ReadNonImageBuffer(const Microsoft::glTF::Document &, const Microsoft::glTF::GLBResourceReader &);

    void ReConstructBuffer();
    void CalculateJsonChunk(const std::string &);
    void CalculateImageMemorySize();

    /** File IO*/
    void writeFile(const std::string &);
    void readFile(const std::string &);

    void CompressImage(const double);
    void CompressImageByUpperBound(const size_t, const size_t);

    void GetDuplicatedInfo(std::vector<DuplicatedImageInfo> &infos);
    void GetDownscaledInfo(std::vector<DownscaleInfo> &infos);
    void GetImageMemInfo(std::vector<std::pair<int, size_t>> &infos);
    void GetOutputManifest(std::string &);

private:
    /** structures */
    using OutImageInfo = struct outImageInfo
    {
        size_t size;
        int padding;
        int bufferViewIndex;
        std::vector<uint8_t> data;
        std::string mimeType;
    };

    int outBufferSize;
    std::vector<OutImageInfo> outImageChunk;

    /** Offsets */
    int originalImageStartOffset;
    int originalImageEndOffset;
    int outImageEndOffset;
    int bufferStartOffset;

    /** BufferView Index*/
    int originalImageStartBufferViewIndex;
    int originalImageEndBufferViewIndex;

    /** sizes*/
    int originalBufferSize = 0;
    int outImageBufferSize = 0;
    int outputBufferSize = 0;
    int prevBufferSize = 0;
    int postBufferSize = 0;

    /** mapping*/
    std::map<int, int> imageMapping;
    // std::vector<std::pair<int, int>> bufferMapping;
    std::map<int, BufferIndexInfo> bufferIndexMapping;

    /** buffer*/
    std::unique_ptr<uint8_t[]> outBuffer;

    std::vector<std::vector<uint8_t>> prevBuffer;
    std::vector<std::vector<uint8_t>> postBuffer;

    /** misc*/
    std::string outJson;

    /** infos*/
    std::vector<DuplicatedImageInfo> duplicatedImageInfos;
    std::vector<DownscaleInfo> downscaleInfos;
    std::vector<std::pair<int, size_t>> imageMemoryInfos;
};

#endif