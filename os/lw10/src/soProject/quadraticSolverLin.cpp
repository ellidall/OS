#include "quadraticSolverLin.h"
#include <cmath>

bool IsZero(double number)
{
    if ((fabs(number) < 1e-9))
    {
        return true;
    }
    return false;
}

SOEXPORT SolveResult SolveQuadratic(double a, double b, double c)
{
    SolveResult result{};
    result.success = false;

    if (IsZero(a))
    {
        return result;
    }

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return result;
    }

    double sqrtDiscriminant = std::sqrt(discriminant);
    result.roots[0] = (-b + sqrtDiscriminant) / (2 * a);
    result.success = true;

    if (discriminant > 0)
    {
        result.roots[1] = (-b - sqrtDiscriminant) / (2 * a);
    }
    else
    {
        result.roots[1] = result.roots[0];
    }

    return result;
}