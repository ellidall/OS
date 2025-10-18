#pragma once

#if defined(COMPILING_SO)
    #define SOEXPORT __attribute__((visibility("default")))
#else
    #define SOEXPORT
#endif

extern "C" {

struct SolveResult
{
    bool success;
    double roots[2];
};

SOEXPORT SolveResult SolveQuadratic(double a, double b, double c);

};