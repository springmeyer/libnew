#define _GNU_SOURCE
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <typeinfo>
#include <iostream>
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>

static std::size_t new_count = 0;
static std::size_t delete_count = 0;
static std::size_t malloc_count = 0;
static std::size_t free_count = 0;

void atexit_handler() 
{
    std::clog << "new: " << new_count << "\n";
    std::clog << "delete: " << delete_count << "\n";
    std::clog << "malloc: " << malloc_count << "\n";
    std::clog << "free: " << free_count << "\n";
}
 
const int result_1 = std::atexit(atexit_handler);

extern "C" {

void* malloc(size_t size)
{
    ++malloc_count;
    using callable_type = void * (*) (size_t);
    callable_type libc_malloc = reinterpret_cast<callable_type>(dlsym(RTLD_NEXT, "malloc"));
    //std::fprintf(stderr, "malloc %lu\ %s %s:%d\n", size,__FUNCTION__, __FILE__, __LINE__);
    //void* callstack[128];
    //int i, frames = backtrace(callstack, 128);
    //backtrace_symbols_fd(callstack,frames,2);        
    return libc_malloc(size);
}

void free(void *p)
{
    ++free_count;
    using callable_type = void (*) (void *);
    callable_type libc_free = reinterpret_cast<callable_type>(dlsym(RTLD_NEXT, "free"));
    libc_free(p);
}

}

void* operator new(std::size_t size) {
    ++new_count;
    void *p = std::malloc(size);
    if (p == 0) { // did malloc succeed?
        throw std::bad_alloc(); // ANSI/ISO compliant behavior
    }
    return p;
}

void operator delete(void* p) noexcept {
    ++delete_count;
    std::free(p);
}