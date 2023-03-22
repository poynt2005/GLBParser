#ifndef __PARSER_STREAM_H__
#define __PARSER_STREAM_H__

#include <GLTFSDK/IStreamWriter.h>
#include <GLTFSDK/IStreamReader.h>
#include <GLTFSDK/GLTF.h>
#include <filesystem>
#include <fstream>

class StreamReader : public Microsoft::glTF::IStreamReader
{
public:
    StreamReader(std::string &&);
    std::shared_ptr<std::istream> GetInputStream(const std::string &) const override;
    std::shared_ptr<std::istream> GetInputStream() const;
    int StreamReader::GetFileSize() const;

private:
    std::string inputFileName;
    std::filesystem::path basePath;
};

class StreamWriter : public Microsoft::glTF::IStreamWriter
{
public:
    StreamWriter(std::string &&);
    std::shared_ptr<std::ostream> GetOutputStream(const std::string &) const override;
    std::shared_ptr<std::ostream> GetOutputStream() const;

private:
    std::string outputFileName;
    std::filesystem::path basePath;
};

#endif