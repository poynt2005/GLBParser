#include "CalculateJson.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>

void CalculateJson(const std::string &inJson, const int totalBufferLength, const std::vector<std::pair<int, int>> &bufferViewInfo, const std::map<int, int> &imageMapping, const std::vector<std::string> &mimeTypes, std::string &outJson)
{

    auto jsonObj = nlohmann::json::parse(inJson);

    auto &bufferViews = jsonObj["bufferViews"];
    auto &images = jsonObj["images"];

    for (int i = 0; i < bufferViews.size(); i++)
    {
        bufferViews[i]["byteOffset"] = bufferViewInfo[i].first;
        bufferViews[i]["byteLength"] = bufferViewInfo[i].second;
    }

    for (int i = 0; i < images.size(); i++)
    {
        auto &image = images[i];

        auto it = imageMapping.find(i);

        image["mimeType"] = std::move(mimeTypes[i]);

        if (it != imageMapping.end())
        {
            image["name"] = images[it->second]["name"];
            image["bufferView"] = images[it->second]["bufferView"];
            image["mimeType"] = images[it->second]["mimeType"];
        }
    }

    jsonObj["buffers"][0]["byteLength"] = totalBufferLength;

    outJson = std::move(jsonObj.dump());
}