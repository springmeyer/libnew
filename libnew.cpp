#define _GNU_SOURCE
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <typeinfo>
#include <iostream>
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>

static std::size_t new_max = 0;
static std::size_t malloc_max = 0;
static std::size_t calloc_max = 0;
static std::size_t new_count = 0;
static std::size_t delete_count = 0;
static std::size_t malloc_count = 0;
static std::size_t free_count = 0;
static std::size_t calloc_count = 0;

void atexit_handler() 
{
    std::clog << "new:              " << new_count << "\n";
    std::clog << " new max size:    " << new_max << "\n";
    std::clog << " delete:          " << delete_count << "\n";
    std::clog << "malloc:           " << malloc_count << "\n";
    std::clog << " malloc max size: " << malloc_max << "\n";
    std::clog << " free:            " << free_count << "\n";
    std::clog << "calloc:           " << calloc_count << "\n";
    std::clog << " calloc max size: " << calloc_max << "\n";
}
 
const int result_1 = std::atexit(atexit_handler);

const int max_size_new_debug = std::stoi(std::getenv("NEW_DEBUG_SIZE") ? std::getenv("NEW_DEBUG_SIZE") : "0" );
const int max_size_malloc_debug = std::stoi(std::getenv("MALLOC_DEBUG_SIZE") ? std::getenv("MALLOC_DEBUG_SIZE") : "0" );
const int max_size_new_abort = std::stoi(std::getenv("NEW_ABORT_SIZE") ? std::getenv("NEW_ABORT_SIZE") : "0" );
const int max_size_malloc_abort = std::stoi(std::getenv("MALLOC_ABORT_SIZE") ? std::getenv("MALLOC_ABORT_SIZE") : "0" );

extern "C" {

int get_new_count() { return new_count; }
int get_malloc_count() { return malloc_count; }

void* calloc(size_t num, size_t size)
{
    ++calloc_count;
    if (size > calloc_max) calloc_max = size;
    using callable_type = void * (*) (size_t, size_t);
    callable_type libc_calloc = reinterpret_cast<callable_type>(dlsym(RTLD_NEXT, "calloc"));
    return libc_calloc(num,size);
}

void* malloc(size_t size)
{
    ++malloc_count;
    if (size > malloc_max) malloc_max = size;
    using callable_type = void * (*) (size_t);
    callable_type libc_malloc = reinterpret_cast<callable_type>(dlsym(RTLD_NEXT, "malloc"));
    if (max_size_malloc_debug > 0  && size >= max_size_malloc_debug) {
        std::fprintf(stderr, "malloc %lu %s %s:%d\n", size,__FUNCTION__, __FILE__, __LINE__);
        void* callstack[128];
        int i, frames = backtrace(callstack, 128);
        backtrace_symbols_fd(callstack,frames,2);
    }
    if (max_size_malloc_abort > 0 && size >= max_size_malloc_abort) {
        abort();
    }
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
    if (size > new_max) new_max = size;
    void *p = std::malloc(size);
    if (p == 0) { // did malloc succeed?
        throw std::bad_alloc(); // ANSI/ISO compliant behavior
    }
    if (max_size_new_debug > 0  && size >= max_size_new_debug) {
        std::fprintf(stderr, "malloc %lu %s %s:%d\n", size,__FUNCTION__, __FILE__, __LINE__);
        void* callstack[128];
        int i, frames = backtrace(callstack, 128);
        backtrace_symbols_fd(callstack,frames,2);
    }
    if (max_size_new_abort > 0 && size >= max_size_new_abort) {
        abort();
    }
    return p;
}

void operator delete(void* p) noexcept {
    ++delete_count;
    std::free(p);
}