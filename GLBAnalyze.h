#ifndef __GLB_ANALYZE_H__
#define __GLB_ANALYZE_H__

#ifdef BUILDGLBANALYZEAPI
#define EXPORTGLBANALYZEAPI __declspec(dllexport)
#else
#define EXPORTGLBANALYZEAPI __declspec(dllimport)
#endif

#include <string.h>

#include "GLBTypes.h"

class GLBAnalyze
{
public:
    GLBAnalyze() = delete;
    GLBAnalyze(const std::string &);
    ~GLBAnalyze();

    /** File IO */
    void WriteFile();
    void WriteFile(const std::string &);

    /** calculate images*/
    void CalculateDulpicatedImage();
    void DownscaleImage(const double);

    /** Get Infomation */
    void GetDuplicatedInfo(std::vector<DuplicatedImageInfo> &infos);
    void GetDownscaledInfo(std::vector<DownscaleInfo> &infos);

private:
    void *ParserPtr;

    bool isDuplicateCalculated;
    bool isImageDownscaled;

    std::string filename;

    void SetParser(void *inputParserPtr)
    {
        ParserPtr = inputParserPtr;
    }
}

#endif