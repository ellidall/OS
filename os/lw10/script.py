import ctypes
from ctypes import c_double, Structure, c_bool, POINTER

LIB_PATH = "./cmake-build-debuglinux/src/so_project/libQuadraticSolverLin.so"

# Определение структуры SolveResult
class SolveResult(Structure):
    _fields_ = [
        ("success", c_bool),
        ("roots", c_double * 2),
    ]


# Загрузка DLL
solver_dll = ctypes.CDLL(LIB_PATH)

# Указание прототипа функции SolveQuadratic
solver_dll.SolveQuadratic.argtypes = [c_double, c_double, c_double]
solver_dll.SolveQuadratic.restype = SolveResult


# Функция для работы с DLL
def solve_quadratic(a, b, c):
    # Вызов функции из DLL
    result = solver_dll.SolveQuadratic(a, b, c)

    # Обработка результата
    if result.success:
        roots = [result.roots[0], result.roots[1]]
        return roots
    else:
        return None


def main():
    print("Enter coefficients a, b, c:")
    a = float(input("a: "))
    b = float(input("b: "))
    c = float(input("c: "))

    roots = solve_quadratic(a, b, c)
    if roots:
        print("Roots:", roots)
    else:
        print("No real roots.")


if __name__ == "__main__":
    main()
