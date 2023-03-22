#include "ParserStream.h"

StreamReader::StreamReader(std::string &&filename)
    : inputFileName(filename)
{
    basePath = std::move(std::filesystem::path(filename).parent_path());
}

std::shared_ptr<std::istream> StreamReader::GetInputStream(const std::string &filename) const
{
    auto streamPath = std::move(std::filesystem::absolute(inputFileName));
    auto stream = std::make_shared<std::ifstream>(streamPath, std::ios_base::binary);
    if (!stream || !(*stream))
    {
        throw std::runtime_error("Unable to create a valid input stream for uri: " + filename);
    }
    return stream;
}

std::shared_ptr<std::istream> StreamReader::GetInputStream() const
{
    return GetInputStream("");
}

StreamWriter::StreamWriter(std::string &&filename)
    : outputFileName(filename)
{
    basePath = std::move(std::filesystem::path(filename).parent_path());
}

std::shared_ptr<std::ostream> StreamWriter::GetOutputStream(const std::string &filename) const
{
    auto streamPath = std::move(std::filesystem::absolute(outputFileName));
    auto stream = std::make_shared<std::ofstream>(streamPath, std::ios_base::binary);
    if (!stream || !(*stream))
    {
        throw std::runtime_error("Unable to create a valid input stream for uri: " + filename);
    }
    return stream;
}

std::shared_ptr<std::ostream> StreamWriter::GetOutputStream() const
{
    return GetOutputStream("");
}
