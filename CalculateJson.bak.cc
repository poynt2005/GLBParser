#include "CalculateJson.h"
#include <Python.h>
#include <algorithm>

void Py_Print(PyObject *obj)
{
    auto PyBuildinModule = PyImport_ImportModule("builtins");
    auto PyPrintMethodCallResult = PyObject_CallMethod(PyBuildinModule, "print", "O", obj);
    Py_DECREF(PyBuildinModule);
    Py_DECREF(PyPrintMethodCallResult);
}

bool CalculateJson(const std::string &inputJsonStr, const std::map<int, int> &duplicateIndexMapping, std::string &outputJsonStr)
{
    Py_Initialize();

    auto PyJsonModule = PyImport_ImportModule("json");
    auto PyGlbJsonObj = PyObject_CallMethod(PyJsonModule, "loads", "s", inputJsonStr.data());

    if (!PyDict_Check(PyGlbJsonObj))
    {
        Py_XDECREF(PyGlbJsonObj);
        Py_DECREF(PyJsonModule);
        return false;
    }

    auto PyBufferViewList = PyDict_GetItemString(PyGlbJsonObj, "bufferViews");

    if (!PyList_Check(PyBufferViewList))
    {
        Py_DECREF(PyGlbJsonObj);
        Py_DECREF(PyJsonModule);
        return false;
    }

    int listLength = PyList_Size(PyBufferViewList);
    int totalReduce = 0;

    auto firstMappingItem = duplicateIndexMapping.begin();

    for (int i = firstMappingItem->first; i < listLength; i++)
    {
        auto PyBufferViewObj = PyList_GetItem(PyBufferViewList, i);
        auto PyBufferViewObjPrev = PyList_GetItem(PyBufferViewList, i - 1);

        if (!PyDict_Check(PyBufferViewObj) || !PyDict_Check(PyBufferViewObjPrev))
        {
            Py_DECREF(PyGlbJsonObj);
            Py_DECREF(PyJsonModule);
            return false;
        }

        auto PyBufferViewByteOffset = PyDict_GetItemString(PyBufferViewObj, "byteOffset");
        auto PyBufferViewByteLength = PyDict_GetItemString(PyBufferViewObj, "byteLength");

        if (!PyLong_Check(PyBufferViewByteOffset))
        {
            Py_DECREF(PyGlbJsonObj);
            Py_DECREF(PyJsonModule);
            return false;
        }

        if (!PyLong_Check(PyBufferViewByteLength))
        {
            Py_DECREF(PyGlbJsonObj);
            Py_DECREF(PyJsonModule);
            return false;
        }

        int byteLength = PyLong_AsSize_t(PyBufferViewByteLength);
        int byteOffset = PyLong_AsSize_t(PyBufferViewByteOffset);

        auto mapItem = duplicateIndexMapping.find(i);

        if (mapItem != duplicateIndexMapping.end())
        {
            PyDict_SetItemString(PyBufferViewObj, "byteLength", PyLong_FromSize_t(0));

            auto PyBufferViewPrevOffset = PyDict_GetItemString(PyBufferViewObjPrev, "byteOffset");

            if (!PyLong_Check(PyBufferViewPrevOffset))
            {
                Py_DECREF(PyGlbJsonObj);
                Py_DECREF(PyJsonModule);
                return false;
            }

            Py_INCREF(PyBufferViewPrevOffset);

            PyDict_SetItemString(PyBufferViewObj, "byteOffset", PyBufferViewPrevOffset);
            totalReduce += byteLength;
        }
        else
        {
            PyDict_SetItemString(PyBufferViewObj, "byteOffset", PyLong_FromSize_t(byteOffset - totalReduce));
        }
    }

    auto PyImagesListObj = PyDict_GetItemString(PyGlbJsonObj, "images");

    if (!PyList_Check(PyImagesListObj))
    {
        Py_DECREF(PyGlbJsonObj);
        Py_DECREF(PyJsonModule);
        return false;
    }

    int imageListSize = PyList_Size(PyImagesListObj);

    for (int i = 0; i < imageListSize; i++)
    {
        auto PyImageObj = PyList_GetItem(PyImagesListObj, i);

        if (!PyDict_Check(PyImageObj))
        {
            Py_DECREF(PyGlbJsonObj);
            Py_DECREF(PyJsonModule);
            return false;
        }

        auto PyImageBufferViewIndex = PyDict_GetItemString(PyImageObj, "bufferView");

        if (!PyLong_Check(PyImageBufferViewIndex))
        {
            Py_DECREF(PyGlbJsonObj);
            Py_DECREF(PyJsonModule);
            return false;
        }

        int bufferViewIndex = PyLong_AsSize_t(PyImageBufferViewIndex);

        auto sourceIdx = duplicateIndexMapping.find(bufferViewIndex);

        if (sourceIdx != duplicateIndexMapping.end())
        {
            PyDict_SetItemString(PyImageObj, "bufferView", PyLong_FromSize_t(sourceIdx->second));
        }
    }

    // auto PyJsonSerializeResult = PyObject_CallMethod(PyJsonModule, "dumps", "O")

    auto PyJsonArgs = PyTuple_New(1);

    Py_INCREF(PyGlbJsonObj);
    PyTuple_SetItem(PyJsonArgs, 0, PyGlbJsonObj);

    auto PyJsonXArgs = PyDict_New();
    PyDict_SetItemString(PyJsonXArgs, "ensure_ascii", Py_False);

    auto PyJsonDumpsFunc = PyObject_GetAttrString(PyJsonModule, "dumps");

    auto PyJsonSerializeResult = PyObject_Call(PyJsonDumpsFunc, PyJsonArgs, PyJsonXArgs);

    Py_DECREF(PyJsonArgs);
    Py_DECREF(PyJsonXArgs);
    Py_DECREF(PyJsonDumpsFunc);

    if (!PyUnicode_Check(PyJsonSerializeResult))
    {
        Py_XDECREF(PyJsonSerializeResult);
        Py_DECREF(PyGlbJsonObj);
        Py_DECREF(PyJsonModule);
        return false;
    }

    auto PyJsonSerializeByteResult = PyUnicode_AsEncodedString(PyJsonSerializeResult, "utf-8", nullptr);

    if (!PyBytes_Check(PyJsonSerializeByteResult))
    {
        Py_XDECREF(PyJsonSerializeByteResult);
        Py_DECREF(PyJsonSerializeResult);
        Py_DECREF(PyGlbJsonObj);
        Py_DECREF(PyJsonModule);
        return false;
    }

    auto jsonSerializeResult = PyBytes_AsString(PyJsonSerializeByteResult);

    outputJsonStr = std::string(jsonSerializeResult);

    Py_DECREF(PyJsonSerializeResult);
    Py_DECREF(PyJsonSerializeByteResult);

    Py_DECREF(PyGlbJsonObj);
    Py_DECREF(PyJsonModule);

    Py_Finalize();
    return false;
}