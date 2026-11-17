#include <runtime/application/Application.hh>

#include <cstddef>
#include <cstdlib>
#include <new>

namespace {
    bool trackAllocations = false;
    std::size_t allocationCount = 0;
} // namespace

extern "C" void *__real_malloc(std::size_t size);
extern "C" void *__wrap_malloc(std::size_t size) {
    if (trackAllocations) {
        ++allocationCount;
    }
    return __real_malloc(size);
}

extern "C" void *__real_calloc(std::size_t count, std::size_t size);
extern "C" void *__wrap_calloc(std::size_t count, std::size_t size) {
    if (trackAllocations) {
        ++allocationCount;
    }
    return __real_calloc(count, size);
}

extern "C" void *__real_realloc(void *allocation, std::size_t size);
extern "C" void *__wrap_realloc(void *allocation, std::size_t size) {
    if (trackAllocations) {
        ++allocationCount;
    }
    return __real_realloc(allocation, size);
}

void *operator new(std::size_t size) {
    if (trackAllocations) {
        ++allocationCount;
    }
    if (auto *allocation = __real_malloc(size == 0 ? 1 : size)) {
        return allocation;
    }
    throw std::bad_alloc{};
}

void *operator new[](std::size_t size) {
    return ::operator new(size);
}

void operator delete(void *allocation) noexcept {
    std::free(allocation);
}

void operator delete[](void *allocation) noexcept {
    std::free(allocation);
}

void operator delete(void *allocation, std::size_t) noexcept {
    std::free(allocation);
}

void operator delete[](void *allocation, std::size_t) noexcept {
    std::free(allocation);
}

int main() {
    runtime::application::Application application;
    const runtime::application::Command boot =
        runtime::application::BootCompleted{runtime::application::ConfigurationStatus::missing};
    const runtime::application::Command confirm = runtime::application::Confirm{};

    trackAllocations = true;
    static_cast<void>(application.dispatch(boot));
    static_cast<void>(application.dispatch(confirm));
    static_cast<void>(application.query(runtime::application::GetCurrentView{}));
    trackAllocations = false;

    return allocationCount == 0 ? 0 : 1;
}
