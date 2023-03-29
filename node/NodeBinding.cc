#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>

#include <string>
#include <map>
#include <unordered_map>

#include <vector>
#include <memory>
#include <iostream>

#include <windows.h>

#include <Napi.h>

#include "../ParserStream.h"
#include "../Parser.h"
#include "../GLBTypes.h"

void GenerateUUID(std::string &uuidOut)
{
    UUID uuid;
    UuidCreate(&uuid);
    char *str;
    UuidToStringA(&uuid, (RPC_CSTR *)&str);
    uuidOut = std::string(str);
    RpcStringFreeA((RPC_CSTR *)&str);
}

using SDKObjects = struct sdkObjects
{
    Microsoft::glTF::Document document;
    std::unique_ptr<Microsoft::glTF::GLBResourceReader> resourceReader;
    std::string manifest;
    std::unique_ptr<Parser> GLBParser;
};

std::unordered_map<std::string, std::shared_ptr<SDKObjects>> sdkObjMapping;

Napi::Value Node_ParserConstruct(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string uuid;
    GenerateUUID(uuid);

    auto filename = info[0].As<Napi::String>().Utf8Value();
    auto streamReader = std::make_shared<StreamReader>(std::move(filename));
    auto glbStream = streamReader->GetInputStream();
    auto glbResourceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(streamReader, glbStream);
    std::string manifest = std::move(glbResourceReader->GetJson());
    auto document = Microsoft::glTF::Deserialize(manifest);

    auto GLBParser = std::make_unique<Parser>();

    GLBParser->CalculateBufferOffset(document, *(glbResourceReader.get()));
    auto sdk = std::make_shared<SDKObjects>(SDKObjects{std::move(document),
                                                       std::move(glbResourceReader),
                                                       std::move(manifest),
                                                       std::move(GLBParser)});

    sdkObjMapping.insert(std::pair<std::string, std::shared_ptr<SDKObjects>>(uuid, sdk));

    return Napi::String::New(env, uuid);
}

Napi::Value Node_CalculateDulpicatedImage(const Napi::CallbackInfo &info)
{

    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();

    auto sdk = sdkObjMapping[objUUID];

    sdk->GLBParser->CalculateDulpicatedImage(sdk->document, *((sdk->resourceReader).get()));
    sdk->GLBParser->ReadNonImageBuffer(sdk->document, *((sdk->resourceReader).get()));
    return env.Null();
}

Napi::Value Node_CompressImage(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong arguments, excepted one string and one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 1 excepted a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto downscaleRatio = info[1].As<Napi::Number>().DoubleValue();
    auto sdk = sdkObjMapping[objUUID];

    sdk->GLBParser->CompressImage(downscaleRatio);

    return env.Null();
}

Napi::Value Node_CompressImageByUpperBound(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong arguments, excepted one string and one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 1 excepted a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];
    auto upperBound = info[1].As<Napi::Number>().Int64Value();

    auto jsonLength = (sdk->manifest).length();

    sdk->GLBParser->CompressImageByUpperBound(upperBound, jsonLength);

    return env.Null();
}

Napi::Value Node_ReConstructBuffer(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];

    sdk->GLBParser->ReConstructBuffer();

    return env.Null();
}

Napi::Value Node_CalculateJsonChunk(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];

    sdk->GLBParser->CalculateJsonChunk(sdk->manifest);

    return env.Null();
}

Napi::Value Node_GetDuplicatedInfo(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];

    std::vector<DuplicatedImageInfo> infos;
    sdk->GLBParser->GetDuplicatedInfo(infos);

    auto jsArr = Napi::Array::New(env, infos.size());

    for (int i = 0; i < infos.size(); i++)
    {
        auto info = std::move(infos[i]);
        auto jsObj = Napi::Object::New(env);
        jsObj.Set("bufferViewIndex", Napi::Number::New(env, info.bufferViewIndex));
        jsObj.Set("originalSize", Napi::Number::New(env, info.originalSize));
        jsObj.Set("hashStr", Napi::String::New(env, std::move(info.hashStr)));

        jsArr.Set(i, std::move(jsObj));
    }

    return jsArr;
}

Napi::Value Node_GetDownscaledInfo(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];

    std::vector<DownscaleInfo> infos;
    sdk->GLBParser->GetDownscaledInfo(infos);

    auto jsArr = Napi::Array::New(env, infos.size());

    for (int i = 0; i < infos.size(); i++)
    {
        auto info = std::move(infos[i]);
        auto jsObj = Napi::Object::New(env);
        jsObj.Set("bufferViewIndex", Napi::Number::New(env, info.bufferViewIndex));
        jsObj.Set("originalSize", Napi::Number::New(env, info.originalSize));
        jsObj.Set("compressSize", Napi::Number::New(env, info.compressSize));
        jsObj.Set("scaleRatio", Napi::Number::New(env, info.scaleRatio));
        jsObj.Set("cvMatSize", Napi::Number::New(env, info.cvMatSize));
        jsObj.Set("cvMatResizedSize", Napi::Number::New(env, info.cvMatResizedSize));
        jsArr.Set(i, std::move(jsObj));
    }

    return jsArr;
}

Napi::Value Node_CalculateImageMemory(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];

    sdk->GLBParser->CalculateImageMemorySize();

    return env.Null();
}

Napi::Value Node_GetImageMemoryInfo(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];

    std::vector<std::pair<int, size_t>> infos;
    sdk->GLBParser->GetImageMemInfo(infos);

    auto jsArr = Napi::Array::New(env, infos.size());

    for (int i = 0; i < infos.size(); i++)
    {
        auto info = std::move(infos[i]);
        auto jsObj = Napi::Object::New(env);
        jsObj.Set("bufferViewIndex", Napi::Number::New(env, info.first));
        jsObj.Set("cvMatSize", Napi::Number::New(env, info.second));
        jsArr.Set(i, std::move(jsObj));
    }

    return jsArr;
}

Napi::Value Node_ReleaseSDK(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    sdkObjMapping.erase(objUUID);

    return env.Null();
}

Napi::Value Node_GetOutManifest(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];

    std::string outManifestJson;

    sdk->GLBParser->GetOutputManifest(outManifestJson);

    if (outManifestJson.empty() || !outManifestJson.length())
    {
        Napi::Error::New(env, "Output manifest is empty!").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto jsonModule = env.Global().Get("JSON");

    if (!jsonModule.IsObject())
    {
        Napi::Error::New(env, "Cannot get JSON module").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto jsonParseFunction = jsonModule.As<Napi::Object>().Get("parse");

    if (!jsonParseFunction.IsFunction())
    {
        Napi::Error::New(env, "Cannot get JSON.parse function").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto manifestObj = jsonParseFunction.As<Napi::Function>().Call(env.Global(), {Napi::String::New(env, outManifestJson)});
    return manifestObj;
}

Napi::Value Node_WriteGLBFile(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong arguments, excepted two strings").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments, position 1 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto objUUID = info[0].As<Napi::String>().Utf8Value();
    auto outGlbFilename = info[1].As<Napi::String>().Utf8Value();
    auto sdk = sdkObjMapping[objUUID];

    sdk->GLBParser->writeFile(std::move(outGlbFilename));

    return env.Null();
}

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    exports.Set(
        Napi::String::New(env, "ParserConstruct"),
        Napi::Function::New(env, Node_ParserConstruct));
    exports.Set(
        Napi::String::New(env, "CalculateDulpicatedImage"),
        Napi::Function::New(env, Node_CalculateDulpicatedImage));
    exports.Set(
        Napi::String::New(env, "CompressImage"),
        Napi::Function::New(env, Node_CompressImage));
    exports.Set(
        Napi::String::New(env, "CompressImageByUpperBound"),
        Napi::Function::New(env, Node_CompressImageByUpperBound));
    exports.Set(
        Napi::String::New(env, "ReConstructBuffer"),
        Napi::Function::New(env, Node_ReConstructBuffer));
    exports.Set(
        Napi::String::New(env, "CalculateJsonChunk"),
        Napi::Function::New(env, Node_CalculateJsonChunk));
    exports.Set(
        Napi::String::New(env, "CalculateImageMemory"),
        Napi::Function::New(env, Node_CalculateImageMemory));
    exports.Set(
        Napi::String::New(env, "GetDownscaledInfo"),
        Napi::Function::New(env, Node_GetDownscaledInfo));
    exports.Set(
        Napi::String::New(env, "GetDuplicatedInfo"),
        Napi::Function::New(env, Node_GetDuplicatedInfo));
    exports.Set(
        Napi::String::New(env, "GetImageMemoryInfo"),
        Napi::Function::New(env, Node_GetImageMemoryInfo));
    exports.Set(
        Napi::String::New(env, "GetOutManifest"),
        Napi::Function::New(env, Node_GetOutManifest));
    exports.Set(
        Napi::String::New(env, "ReleaseSDK"),
        Napi::Function::New(env, Node_ReleaseSDK));
    exports.Set(
        Napi::String::New(env, "WriteGLBFile"),
        Napi::Function::New(env, Node_WriteGLBFile));
    return exports;
}

NODE_API_MODULE(GLBParser, Initialize)