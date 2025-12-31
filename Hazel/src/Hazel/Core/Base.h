#pragma once

#include <memory>

namespace Hazel {

    void InitializeCore();
    void ShutdownCore();

}

#ifdef HZ_DEBUG
    #define HZ_ENABLE_ASSERTS
#endif


#ifdef HZ_ENABLE_ASSERTS
    #if defined(_MSC_VER)
        #define HZ_DEBUGBREAK() __debugbreak()
    #elif defined(__GNUC__) || defined(__clang__)
        #include <signal.h>
        #define HZ_DEBUGBREAK() raise(SIGTRAP)
    #else
        #define HZ_DEBUGBREAK()
    #endif
    #define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK(); } }
    #define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK(); } }
#else
    #define HZ_ASSERT(x, ...)
    #define HZ_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define HZ_BIND_EVENT_FN(fn) std::bind(&##fn, this, std::placeholders::_1)

namespace Hazel {

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;

}