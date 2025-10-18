#pragma once

#if defined(COMPILING_DLL)
    #define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT __declspec(dllimport)
#endif

extern "C" {

struct SolveResult
{
    bool success;
    double roots[2];
};

DLLEXPORT SolveResult SolveQuadratic(double a, double b, double c);

};