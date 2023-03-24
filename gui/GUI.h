#ifndef __GUI_H__
#define __GUI_H__

#include <Windows.h>
#include <string>
#include <functional>

#ifdef BUILDGUIAPI
#define EXPORTGUIAPI __declspec(dllexport)
#else
#define EXPORTGUIAPI __declspec(dllimport)
#endif

/** Buttons */
constexpr int LOAD_GLB_BUTTON_ID = 0x01;
constexpr int SAVE_GLB_BUTTON_ID = 0x02;
constexpr int CALCUL_GLB_BUTTON_ID = 0x03;

/** Texts*/
constexpr int DUPLICATED_INFO_TEXT = 0x10;
constexpr int IMAGE_MEMORY_INFO_TEXT = 0x11;
constexpr int COMPRESS_INFO_TEXT = 0x12;
constexpr int LOADED_GLB_TEXT = 0x13;

/** Miscs */
constexpr int TEXT_EDITOR_ID = 0x20;
constexpr int PROGRESS_PAR_ID = 0x21;

constexpr int windowWidth = 500;
constexpr int windowHeight = 500;
constexpr char *szClassName = "GLBParser_Window";

EXPORTGUIAPI bool CreateMainWindow();
EXPORTGUIAPI bool CreateMainWindow(const std::string &);
EXPORTGUIAPI void TextSetter(const int, const std::string &);
EXPORTGUIAPI void ProgressSetter(const int);
EXPORTGUIAPI void TextGetter(std::string &);
EXPORTGUIAPI bool OpenFileDialog(std::string &);
EXPORTGUIAPI void ButtonCallbackSetter(const int, std::function<void()> &&);
EXPORTGUIAPI void SetMessageTypeGetter(std::function<void(const unsigned int)> &&);

EXPORTGUIAPI void ShowWarningDialog(const std::string &, const std::string &);

EXPORTGUIAPI void RenderFrame();
EXPORTGUIAPI void UpdateFrame();

#endif