#pragma once
#include "defines.h"


class PageMemory
{
public:

    class Latest
    {
    public:
        static void *pointer;
    };

    class Internal
    {
    public:
        static void *pointer;
    };

    class SetMask
    {
    public:
        static void *pointer;
    };

    class SetName
    {
    public:
        static void *pointer;
    };

    class FileManager
    {
    public:
        static void *pointer;
    };
};


void ChangeC_Memory_NumPoints(bool active);
void OnPressMemoryExtFileManager();
void Memory_SaveSignalToFlashDrive();
extern const void *pMspFileManager;
