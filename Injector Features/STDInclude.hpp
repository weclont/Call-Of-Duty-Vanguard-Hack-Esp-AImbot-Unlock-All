#pragma once

#define STD_INCLUDED

#ifndef RC_INVOKED

#define _HAS_CXX17 1
#define VC_EXTRALEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN

// Requires Visual Leak Detector plugin: http://vld.codeplex.com/
#define VLD_FORCE_ENABLE
//#include <vld.h>

#include <windows.h>
#include <assert.h>
#include <intrin.h>
#include <Shellapi.h>
#include <winternl.h>
#include <VersionHelpers.h>
#include <conio.h>

#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "ntdll.lib")

#pragma warning(push)
#pragma warning(disable: 4091)
#pragma warning(disable: 4244)
//#include <dbghelp.h>

#include <map>
#include <mutex>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <queue>
#include <random>
#include <atomic>
#include <sstream>
#include <iostream>
#include <regex>

// Experimental C++17 features
#include <filesystem>
#include <optional>

#pragma warning(pop)

#define VA_DONT_USE_THREAD_STORAGE

// Enable additional literals
using namespace std::literals;

#include "Utils/IO.hpp"
#include "Utils/NT.hpp"
#include "Utils/Lock.hpp"
#include "Utils/Time.hpp"
#include "Utils/Queue.hpp"
#include "Utils/Utils.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Signal.hpp"
#include "Utils/String.hpp"
#include "Utils/Network.hpp"
#include "Utils/InfoString.hpp"
#include "Utils/SignalHandler.hpp"

#endif

// Resource stuff
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
// Defines below make accessing the resources from the code easier.
#define _APS_NEXT_RESOURCE_VALUE        102
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif

// Resources
#define IDI_ICON            102

#define WINDIVERT_DLL           300
#define WINDIVERT_DRIVER_x86    301
#define WINDIVERT_DRIVER_x64    302
