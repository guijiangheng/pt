#ifndef PT_CORE_PARALLEL_H
#define PT_CORE_PARALLEL_H

#include <functional>
#include <pt/math/vector2.h>

namespace pt {

void parallelInit();
void parallelCleanup();
void parallelFor1D(std::function<void(int64_t)> func, int64_t count, int chunkSize = 1);
void parallelFor2D(std::function<void(const Vector2i&)> func, const Vector2i& count);

}

#endif
