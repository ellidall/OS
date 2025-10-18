#include <iostream>
#include "../soProject/quadraticSolverLin.h"

int main()
{
    double a, b, c;
    std::cout << "Enter coefficients a, b, c: ";
    std::cin >> a >> b >> c;

    auto result = SolveQuadratic(a, b, c);
    if (result.success)
    {
        std::cout << "Roots: ";
        for (const auto root : result.roots)
        {
            std::cout << root << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "No real roots" << std::endl;
    }

    return EXIT_SUCCESS;
}