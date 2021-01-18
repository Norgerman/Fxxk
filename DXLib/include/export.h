#pragma once
#if defined(_WINDLL) && !defined(_CLR)
#define dllexport __declspec(dllexport)
#else
#define dllexport
#endif