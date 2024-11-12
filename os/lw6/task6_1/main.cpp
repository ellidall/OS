#include <cassert>
#include <cstddef>
#include <vector>
#include "MemoryManager.h"

int main()
{
    alignas(sizeof(std::max_align_t)) char buffer[1000];
    MemoryManager memoryManager(buffer, std::size(buffer));

    auto ptr = memoryManager.Allocate(sizeof(double));
    assert(ptr != nullptr);

    auto value = std::construct_at(static_cast<double*>(ptr), 3.1415927);
    assert(*value == 3.1415927);

    memoryManager.Free(ptr);

    auto ptr2 = memoryManager.Allocate(sizeof(double));
    assert(ptr2 != nullptr);
    memoryManager.Free(ptr2);

    return EXIT_SUCCESS;
}
