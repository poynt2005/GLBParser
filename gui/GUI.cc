#include "GUI.h"

#include <CommCtrl.h>
#include <unordered_map>
#include <iostream>

#define ScreenX GetSystemMetrics(SM_CXSCREEN)
#define ScreenY GetSystemMetrics(SM_CYSCREEN)

std::unordered_map<int, std::string> textMapping;
std::unordered_map<int, std::function<void()>> buttonCallbackMapping;

std::function<void(const unsigned int)> setCurrentMessageType;

HINSTANCE hInstance;
HWND hWnd;
HWND inputTextHWnd;
HWND progressBarHwnd;
WNDCLASSEX wcex;

std::string editTextValue;

void TextSetter(const int textId, const std::string &inputText)
{
    textMapping[textId] = textMapping[textId] + inputText;
    InvalidateRect(hWnd, nullptr, TRUE);
}

void ProgressSetter(const int progress)
{
    SendMessage(
        progressBarHwnd,
        PBM_SETPOS,
        progress,
        0);
    InvalidateRect(hWnd, nullptr, TRUE);
}

void TextGetter(std::string &val)
{
    val = editTextValue;
}

void SetMessageTypeGetter(std::function<void(const unsigned int)> &&getter)
{
    setCurrentMessageType = getter;
}

void ButtonCallbackSetter(const int buttonId, std::function<void()> &&callback)
{
    buttonCallbackMapping[buttonId] = callback;
}

bool OpenFileDialog(std::string &filename)
{
    OPENFILENAME ofn;
    char szFile[MAX_PATH];
    HANDLE hf;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Graphics Language Transmission Format\0*.glb\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    const bool isOpenFile = static_cast<bool>(GetOpenFileName(&ofn));
    if (isOpenFile)
    {
        filename = std::string(ofn.lpstrFile);
    }

    return isOpenFile;
}

LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    if (setCurrentMessageType)
    {
        setCurrentMessageType(message);
    }

    if (message == WM_PAINT)
    {
        hdc = BeginPaint(hWnd, &ps);

        TextOutA(
            hdc,
            10,
            30,
            "Duplicated Info:",
            16);

        TextOutA(
            hdc,
            10,
            50,
            textMapping[DUPLICATED_INFO_TEXT].data(),
            textMapping[DUPLICATED_INFO_TEXT].length());

        TextOutA(
            hdc,
            10,
            100,
            "Image Memory Info:",
            18);

        TextOutA(
            hdc,
            10,
            120,
            textMapping[IMAGE_MEMORY_INFO_TEXT].data(),
            textMapping[IMAGE_MEMORY_INFO_TEXT].length());

        TextOutA(
            hdc,
            10,
            170,
            "Compress Info:",
            14);

        TextOutA(
            hdc,
            10,
            190,
            textMapping[COMPRESS_INFO_TEXT].data(),
            textMapping[COMPRESS_INFO_TEXT].length());

        TextOutA(
            hdc,
            10,
            340,
            "Loaded GLB: ",
            11);

        TextOutA(
            hdc,
            100,
            340,
            textMapping[LOADED_GLB_TEXT].data(),
            textMapping[LOADED_GLB_TEXT].length());

        TextOutA(
            hdc,
            10,
            380,
            "Enter Size:",
            11);

        EndPaint(hWnd, &ps);
    }
    else if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
    }
    else if (message == WM_COMMAND)
    {
        const int commandId = static_cast<int>(wParam);

        if (HIWORD(wParam) == EN_CHANGE)
        {
            hdc = GetDC(hWnd);
            auto length = GetWindowTextLength(inputTextHWnd);
            auto buf = std::make_unique<char[]>(1000);
            auto ret = GetWindowText(inputTextHWnd, buf.get(), length + 1);
            editTextValue = std::string(buf.get(), length);
            return 0;
        }

        if (buttonCallbackMapping[commandId])
        {
            buttonCallbackMapping[commandId]();
            return 0;
        }
    }
    else
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool CreateMainWindow()
{
    return CreateMainWindow("Test_Window");
}

bool CreateMainWindow(const std::string &szText)
{

    hInstance = GetModuleHandle(0);
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_EXCLAMATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szClassName;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_EXCLAMATION);

    if (!RegisterClassExA(&wcex))
    {
        return false;
    }

    hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szClassName,
        szText.data(),
        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        (ScreenX - windowWidth) / 2, (ScreenY - windowHeight) / 2,
        windowWidth,
        windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    /** Create Button for Calculate */
    CreateWindow(
        "BUTTON",
        "Calculate",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        350, 275,
        100, 30,
        hWnd,
        reinterpret_cast<HMENU>(CALCUL_GLB_BUTTON_ID),
        hInstance,
        nullptr);

    /** Create Button for LoadGLB */
    CreateWindow(
        "BUTTON",
        "LoadGLB",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        350, 325,
        100, 30,
        hWnd,
        reinterpret_cast<HMENU>(LOAD_GLB_BUTTON_ID),
        hInstance,
        nullptr);

    /** Create Button for SaveGLB */
    CreateWindow(
        "BUTTON",
        "SaveGLB",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        350, 375,
        100, 30,
        hWnd,
        reinterpret_cast<HMENU>(SAVE_GLB_BUTTON_ID),
        hInstance,
        nullptr);

    /** Create Input Text*/
    inputTextHWnd = CreateWindow(
        "EDIT",
        0,
        WS_BORDER | WS_CHILD | WS_VISIBLE,
        100, 375,
        200, 30,
        hWnd,
        reinterpret_cast<HMENU>(TEXT_EDITOR_ID),
        hInstance,
        0);

    /** Create Progress Bar*/
    progressBarHwnd = CreateWindowEx(
        0,
        PROGRESS_CLASS,
        "",
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
        10, 420,
        440, 30,
        hWnd,
        reinterpret_cast<HMENU>(PROGRESS_PAR_ID),
        hInstance,
        nullptr);

    /** Set text*/
    textMapping.insert(std::pair<int, std::string>(DUPLICATED_INFO_TEXT, ""));
    textMapping.insert(std::pair<int, std::string>(IMAGE_MEMORY_INFO_TEXT, ""));
    textMapping.insert(std::pair<int, std::string>(COMPRESS_INFO_TEXT, ""));
    textMapping.insert(std::pair<int, std::string>(LOADED_GLB_TEXT, ""));

    return true;
}

void ShowWarningDialog(const std::string &title, const std::string &text)
{
    MessageBox(hWnd, text.data(), title.data(), MB_ICONEXCLAMATION);
}

void RenderFrame()
{
    MSG msg;

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void UpdateFrame()
{
    InvalidateRect(hWnd, NULL, TRUE);
}