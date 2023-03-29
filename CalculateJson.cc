#include "CalculateJson.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>

void CalculateJson(const std::string &inJson, const int totalBufferLength, const std::map<int, BufferIndexInfo> &bufferIndexMapping, const std::map<int, int> &imageMapping, std::string &outJson)
{

    auto jsonObj = nlohmann::json::parse(inJson);

    const auto originalBufferViews = std::move(jsonObj["bufferViews"]);
    jsonObj["bufferViews"].clear();

    auto &images = jsonObj["images"];
    nlohmann::json bufferViews;
    auto indexMappingIter = bufferIndexMapping.begin();
    for (; indexMappingIter != bufferIndexMapping.end(); ++indexMappingIter)
    {
        if ((indexMappingIter->second).size != 0)
        {
            const auto &mappingTarget = originalBufferViews[indexMappingIter->first];
            nlohmann::json bufferView;
            bufferView["buffer"] = 0;
            bufferView["byteOffset"] = (indexMappingIter->second).offset;
            bufferView["byteLength"] = (indexMappingIter->second).size;
            if (mappingTarget.contains("target"))
            {
                bufferView["target"] = mappingTarget["target"];
            }

            bufferViews.emplace_back(std::move(bufferView));
        }
    }
    jsonObj["bufferViews"] = std::move(bufferViews);
    auto &accessors = jsonObj["accessors"];
    for (auto &accessor : accessors)
    {
        const int originBufferViewIndex = accessor["bufferView"];
        accessor["bufferView"] = bufferIndexMapping.at(originBufferViewIndex).index;
    }

    const auto originalImages = jsonObj["images"];

    for (int i = 0; i < originalImages.size(); i++)
    {
        const auto &mappingImageTarget = originalImages[imageMapping.at(i)];
        const int bufferViewIndex = bufferIndexMapping.at(mappingImageTarget["bufferView"]).index;
        jsonObj["images"][i]["bufferView"] = bufferViewIndex;
        jsonObj["images"][i]["mimeType"] = mappingImageTarget["mimeType"];
        jsonObj["images"][i]["name"] = mappingImageTarget["name"];
    }

    jsonObj["buffers"][0]["byteLength"] = totalBufferLength;

    outJson = std::move(jsonObj.dump());
}