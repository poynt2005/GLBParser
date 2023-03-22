#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>

#include <string>
#include <vector>

#include "Parser.h"
#include "ParserStream.h"
#include "GLBAnalyze.h"

GLBAnalyze::GLBAnalyze(const std::string &inputFilename)
    : filename(inputFilename)
{
    auto streamReader = std::make_shared<StreamReader>(std::move(inputFilename));
    auto glbStream = streamReader->GetInputStream();
    glbStream->seekg(0, std::ios::end);
    auto glbSize = glbStream->tellg();
    glbStream->seekg(std::ios::beg);
    auto glbResourceReader = std::make_shared<Microsoft::glTF::GLBResourceReader>(streamReader, glbStream);
    std::string manifest = std::move(glbResourceReader->GetJson());
}

GLBAnalyze::~GLBAnalyze() {}
