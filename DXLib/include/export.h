#pragma once
#if defined(_WINDLL)
#define dllexport __declspec(dllexport)
#else
#define dllexport
#endif