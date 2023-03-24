#include "GUI.h"

#include <Napi.h>
#include <vector>
#include <iostream>
#include <functional>

#include <Windows.h>

std::vector<Napi::ThreadSafeFunction> tsfns;

/** current message type getter*/
unsigned int messageType;

Napi::Value Node_CreateMainWindow(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    bool isWindowCreated = CreateMainWindow(info[0].As<Napi::String>().Utf8Value());
    return Napi::Boolean::New(env, isWindowCreated);
}

Napi::Value Node_RenderFrame(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    RenderFrame();
    return env.Null();
}

Napi::Value Node_UpdateFrame(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    UpdateFrame();
    return env.Null();
}

Napi::Value Node_SetText(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one number and one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong argument, position 0 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsString())
    {
        Napi::TypeError::New(env, "Wrong argument, position 1 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    TextSetter(
        info[0].As<Napi::Number>().Int32Value(),
        std::move(info[1].As<Napi::String>().Utf8Value()));
    return env.Null();
}

Napi::Value Node_GetText(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    std::string editTextValue;
    TextGetter(editTextValue);
    return Napi::String::New(env, editTextValue);
}

Napi::Value Node_SetProgress(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong argument, position 0 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    ProgressSetter(info[0].As<Napi::Number>().Int32Value());
    return env.Null();
}

Napi::Value Node_OpenFileDialog(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    std::string filename;
    const bool isFile = OpenFileDialog(filename);

    if (isFile)
    {
        return Napi::String::New(env, filename);
    }

    else
    {
        return env.Null();
    }
}

Napi::Value Node_SetButtonCallback(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong argument, excepted one number and one function").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong argument, position 0 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsFunction())
    {
        Napi::TypeError::New(env, "Wrong argument, position 1 excepted one function").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto tsfn = Napi::ThreadSafeFunction::New(
        env,
        info[1].As<Napi::Function>(),
        "ButtonCallback",
        0,
        1,
        (void *)nullptr);

    std::function<void()> cppCallback = [tsfn]() -> void
    {
        auto callback = [](Napi::Env, Napi::Function jsCallback)
        {
            jsCallback.Call({});
        };
        tsfn.BlockingCall(callback);
    };

    tsfns.emplace_back(tsfn);

    ButtonCallbackSetter(info[0].As<Napi::Number>().Int32Value(), std::move(cppCallback));

    return env.Null();
}

Napi::Value Node_GetCurrentMessageType(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    switch (messageType)
    {
    case WM_DESTROY:
        return Napi::String::New(env, "WM_DESTROY");
    case WM_NCDESTROY:
        return Napi::String::New(env, "WM_NCDESTROY");
    case WM_MOUSEMOVE:
        return Napi::String::New(env, "WM_MOUSEMOVE");
    case WM_CTLCOLOREDIT:
        return Napi::String::New(env, "WM_CTLCOLOREDIT");
    case WM_PAINT:
        return Napi::String::New(env, "WM_PAINT");
    case WM_COMMAND:
        return Napi::String::New(env, "WM_COMMAND");
    default:
        return Napi::String::New(env, "Unknown");
    }
}

Napi::Value Node_ReleaseThreadFunction(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    for (auto &&tsfn : tsfns)
    {
        tsfn.Release();
    }

    return env.Null();
}

Napi::Value Node_ShowWarningMessage(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong arguments, excepted two string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString() || !info[1].IsString())
    {
        Napi::TypeError::New(env, "Wrong argument, position 0 or position 1 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    ShowWarningDialog(
        std::move(info[0].As<Napi::String>().Utf8Value()),
        std::move(info[1].As<Napi::String>().Utf8Value()));

    return env.Null();
}

Napi::Value Node_GetConstant(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    auto constantObj = Napi::Object::New(env);

    constantObj.Set(
        "LOAD_GLB_BUTTON_ID",
        Napi::Number::New(env, 0x01));

    constantObj.Set(
        "SAVE_GLB_BUTTON_ID",
        Napi::Number::New(env, 0x02));

    constantObj.Set(
        "CALCUL_GLB_BUTTON_ID",
        Napi::Number::New(env, 0x03));

    constantObj.Set(
        "DUPLICATED_INFO_TEXT",
        Napi::Number::New(env, 0x10));

    constantObj.Set(
        "IMAGE_MEMORY_INFO_TEXT",
        Napi::Number::New(env, 0x11));

    constantObj.Set(
        "COMPRESS_INFO_TEXT",
        Napi::Number::New(env, 0x12));

    constantObj.Set(
        "LOADED_GLB_TEXT",
        Napi::Number::New(env, 0x13));

    constantObj.Set(
        "TEXT_EDITOR_ID",
        Napi::Number::New(env, 0x20));

    constantObj.Set(
        "PROGRESS_PAR_ID",
        Napi::Number::New(env, 0x21));

    return constantObj;
}

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    exports.Set(
        Napi::String::New(env, "CreateMainWindow"),
        Napi::Function::New(env, Node_CreateMainWindow));
    exports.Set(
        Napi::String::New(env, "SetText"),
        Napi::Function::New(env, Node_SetText));
    exports.Set(
        Napi::String::New(env, "GetText"),
        Napi::Function::New(env, Node_GetText));
    exports.Set(
        Napi::String::New(env, "SetProgress"),
        Napi::Function::New(env, Node_SetProgress));
    exports.Set(
        Napi::String::New(env, "SetButtonCallback"),
        Napi::Function::New(env, Node_SetButtonCallback));
    exports.Set(
        Napi::String::New(env, "ReleaseThreadFunction"),
        Napi::Function::New(env, Node_ReleaseThreadFunction));
    exports.Set(
        Napi::String::New(env, "OpenFileDialog"),
        Napi::Function::New(env, Node_OpenFileDialog));

    exports.Set(
        Napi::String::New(env, "RenderFrame"),
        Napi::Function::New(env, Node_RenderFrame));
    exports.Set(
        Napi::String::New(env, "UpdateFrame"),
        Napi::Function::New(env, Node_UpdateFrame));
    exports.Set(
        Napi::String::New(env, "GetCurrentMessageType"),
        Napi::Function::New(env, Node_GetCurrentMessageType));
    exports.Set(
        Napi::String::New(env, "ShowWarningMessage"),
        Napi::Function::New(env, Node_ShowWarningMessage));
    exports.Set(
        Napi::String::New(env, "GetConstant"),
        Napi::Function::New(env, Node_GetConstant));

    auto setCurrentMessageType = [](const unsigned int inputType) -> void
    {
        messageType = inputType;
    };
    SetMessageTypeGetter(std::move(setCurrentMessageType));

    return exports;
}

NODE_API_MODULE(GUI, Initialize);