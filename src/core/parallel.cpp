#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <pt/core/parallel.h>

namespace pt {

class ParallelForLoop;

static std::vector<std::thread> threads;
static auto shutdownThreads = false;
static ParallelForLoop* workList = nullptr;
static std::mutex m;
static std::condition_variable cv;

class ParallelForLoop {
public:
    ParallelForLoop(std::function<void(int64_t)>&& func1D, int64_t count, int chunkSize = 1) noexcept
        : func1D(std::move(func1D)), count(count), chunkSize(chunkSize)
    { }

    ParallelForLoop(std::function<void(const Vector2i&)>&& func2D, const Vector2i& count) noexcept
        : func2D(std::move(func2D)), count(count.x * count.y), countX(count.x), chunkSize(1)
    { }

    bool isFinish() const {
        return nextIndex == count && activeThreads == 0;
    }

public:
    std::function<void(int64_t)> func1D;
    std::function<void(const Vector2i&)> func2D;
    int64_t count;
    int64_t nextIndex = 0;
    int countX;
    int chunkSize;
    int activeThreads = 0;
    ParallelForLoop* next = nullptr;
};

void workerThreadFunc() {
    std::unique_lock<std::mutex> lock(m);
    while (!shutdownThreads) {
        if (!workList) cv.wait(lock);
        else {
            auto& loop = *workList;
            auto beg = loop.nextIndex;
            auto end = std::min(loop.nextIndex + loop.chunkSize, loop.count);
            loop.nextIndex = end;
            if (loop.nextIndex == loop.count) workList = loop.next;
            ++loop.activeThreads;

            lock.unlock();

            if (loop.func1D)
                for (auto i = beg; i < end; ++i) loop.func1D(i);
            else
                loop.func2D(Vector2i(beg % loop.countX, beg / loop.countX));

            lock.lock();
            --loop.activeThreads;
        }
    }
}

void parallelInit() {
    int maxThreads = std::max(std::thread::hardware_concurrency(), 1u) - 1;
    for (auto i = 0; i < maxThreads; ++i)
        threads.emplace_back(workerThreadFunc);
}

void parallelCleanup() {
    {
        std::lock_guard<std::mutex> guard(m);
        shutdownThreads = true;
    }

    cv.notify_all();

    for (auto& thread : threads) thread.join();
    threads.clear();

    shutdownThreads = false;
}

void parallelFor1D(std::function<void(int64_t)> func, int64_t count, int chunkSize) {
    if (threads.empty() || count <= chunkSize) {
        for (int64_t i = 0; i < count; ++i) func(i);
        return;
    }

    ParallelForLoop loop(std::move(func), count, chunkSize);

    std::unique_lock<std::mutex> lock(m);
    workList = &loop;
    cv.notify_all();

    while (!loop.isFinish()) {
        auto beg = loop.nextIndex;
        auto end = std::min(loop.nextIndex + chunkSize, loop.count);
        loop.nextIndex = end;
        if (loop.nextIndex == loop.count) workList = loop.next;
        ++loop.activeThreads;
        lock.unlock();
        for (auto i = beg; i < end; ++i) loop.func1D(i);
        lock.lock();
        --loop.activeThreads;
    }
}

void parallelFor2D(std::function<void(const Vector2i&)> func, const Vector2i& count) {
    if (count.x * count.y == 0) return;

    if (threads.empty() || count.x * count.y == 1) {
        func(Vector2i(0, 0));
        return;
    }

    ParallelForLoop loop(std::move(func), count);

    std::unique_lock<std::mutex> lock(m);
    workList = &loop;
    cv.notify_all();

    while (!loop.isFinish()) {
        if (loop.nextIndex == loop.count) {
            lock.unlock();
            lock.lock();
        } else {
            auto beg = loop.nextIndex;
            auto end = std::min(loop.nextIndex + 1, loop.count);
            loop.nextIndex = end;
            if (loop.nextIndex == loop.count) workList = loop.next;
            ++loop.activeThreads;
            lock.unlock();
            loop.func2D(Vector2i(beg % loop.countX, beg / loop.countX));
            lock.lock();
            --loop.activeThreads;
        }
    }
}

}
