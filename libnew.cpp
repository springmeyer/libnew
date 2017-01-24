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
static std::size_t new_total = 0;
static std::size_t new_count = 0;
static std::size_t delete_count = 0;
const int max_size_new_debug = std::stoi(std::getenv("NEW_DEBUG_SIZE") ? std::getenv("NEW_DEBUG_SIZE") : "0" );
const int max_size_new_abort = std::stoi(std::getenv("NEW_ABORT_SIZE") ? std::getenv("NEW_ABORT_SIZE") : "0" );

static std::size_t malloc_max = 0;
static std::size_t malloc_total = 0;
static std::size_t malloc_count = 0;
static std::size_t free_count = 0;
const int max_size_malloc_debug = std::stoi(std::getenv("MALLOC_DEBUG_SIZE") ? std::getenv("MALLOC_DEBUG_SIZE") : "0" );
const int max_size_malloc_abort = std::stoi(std::getenv("MALLOC_ABORT_SIZE") ? std::getenv("MALLOC_ABORT_SIZE") : "0" );

static std::size_t calloc_count = 0;
static std::size_t calloc_total = 0;
static std::size_t calloc_max = 0;

void atexit_handler() 
{
    std::clog << "new:\n";
    std::clog << "  count:    " << new_count << "\n";
    std::clog << "  total:    " << new_total << "\n";
    std::clog << "  max:      " << new_max << "\n";
    std::clog << "  deletes:  " << delete_count << "\n";
    std::clog << "\n";
    std::clog << "malloc:\n";
    std::clog << "  count:    " << malloc_count << "\n";
    std::clog << "  total:    " << malloc_total << "\n";
    std::clog << "  max:      " << malloc_max << "\n";
    std::clog << "  free:     " << free_count << "\n";
    std::clog << "\n";
    std::clog << "calloc:\n";
    std::clog << "  count:    " << calloc_count << "\n";
    std::clog << "  total:    " << calloc_total << "\n";
    std::clog << "  max:      " << calloc_max << "\n";
}
 
const int result_1 = std::atexit(atexit_handler);

extern "C" {

int get_new_count() { return new_count; }
int get_malloc_count() { return malloc_count; }

using calloc_type = void * (*) (size_t, size_t);
static void* (*real_calloc)(size_t nmemb, size_t size)=nullptr;

void* calloc(size_t num, size_t size)
{
    if(real_calloc == nullptr) {
        real_calloc = reinterpret_cast<calloc_type>(dlsym(RTLD_NEXT, "calloc"));
        if (real_calloc == nullptr) {
          abort();
        }
    }

    ++calloc_count;
    calloc_total += size;
    if (size > calloc_max) calloc_max = size;
    return real_calloc(num,size);
}

using malloc_type = void * (*) (size_t);
static void* (*real_malloc)(size_t size)=nullptr;

void* malloc(size_t size)
{
    if(real_malloc == nullptr) {
        real_malloc = reinterpret_cast<malloc_type>(dlsym(RTLD_NEXT, "malloc"));
        if (real_malloc == nullptr) {
          abort();
        }
    }

    ++malloc_count;
    malloc_total += size;
    if (size > malloc_max) malloc_max = size;
    if (max_size_malloc_debug > 0  && size >= max_size_malloc_debug) {
        std::fprintf(stderr, "malloc %lu %s %s:%d\n", size,__FUNCTION__, __FILE__, __LINE__);
        void* callstack[128];
        int i, frames = backtrace(callstack, 128);
        backtrace_symbols_fd(callstack,frames,2);
    }
    if (max_size_malloc_abort > 0 && size >= max_size_malloc_abort) {
        abort();
    }
    return real_malloc(size);
}


using free_type = void (*) (void *);
static void (*real_free)(void * p)=nullptr;

void free(void * p)
{
    if(real_free == nullptr) {
        real_free = reinterpret_cast<free_type>(dlsym(RTLD_NEXT, "free"));
        if (real_free == nullptr) {
          abort();
        }
    }

    ++free_count;
    return real_free(p);
}

}

void* operator new(std::size_t size) {
    ++new_count;
    new_total += size;
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