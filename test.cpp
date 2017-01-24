#include <iostream>
#include <memory>
#include <dlfcn.h>
#include <cstdlib>

int _new_count(void * handle) {
    using callable_type = int (*) ();
    callable_type libnew_get_new_count = reinterpret_cast<callable_type>(dlsym(handle, "get_new_count"));
    if (!libnew_get_new_count) throw std::runtime_error("get_new_count not found");
    return libnew_get_new_count();
}

int _malloc_count(void * handle) {
    using callable_type = int (*) ();
    callable_type libnew_get_malloc_count = reinterpret_cast<callable_type>(dlsym(handle, "get_malloc_count"));
    if (!libnew_get_malloc_count) throw std::runtime_error("get_malloc_count not found");
    return libnew_get_malloc_count();
}

std::string get_preload() {
    const char* ld_preload = std::getenv("LD_PRELOAD");
    if (ld_preload) return ld_preload;
    const char* dyld_insert = std::getenv("DYLD_INSERT_LIBRARIES");
    if (dyld_insert) return dyld_insert;
    return "";
}


int main() {
    try {
        std::string preload = get_preload();
        void * handle;
        if (!preload.empty()) {
            handle = dlopen(preload.c_str(),RTLD_LAZY);
        } else {
            throw std::runtime_error("Must provide LD_PRELOAD on linux and DYLD_INSERT_LIBRARIES on OS X");
        }

        std::clog << "nc1: " << _new_count(handle) << "\n";
        std::clog << "mc1: " << _malloc_count(handle) << "\n";
        {
            std::clog << "long std::string\n";
            std::string s("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        }

        std::clog << "nc2: " << _new_count(handle) << "\n";
        std::clog << "mc2: " << _malloc_count(handle) << "\n";
        {
            std::clog << "20 item unsigned int array\n";
            std::unique_ptr<unsigned int[]> out_row(new unsigned int[20]);
        }

        std::clog << "nc3: " << _new_count(handle) << "\n";
        std::clog << "mc3: " << _malloc_count(handle) << "\n";
        {
            std::clog << "direct malloc of unsigned int pointer\n";
            unsigned int *ptr_one = (unsigned int *)malloc(sizeof(unsigned int));
            free(ptr_one);
        }

        std::clog << "nc4: " << _new_count(handle) << "\n";
        std::clog << "mc4: " << _malloc_count(handle) << "\n";

        {
           int* p1 = (int*)std::calloc(4, sizeof(int));
           free(p1);
        }

        std::clog << "nc5: " << _new_count(handle) << "\n";
        std::clog << "mc5: " << _malloc_count(handle) << "\n";
    } catch (std::exception const& ex) {
        std::clog << "Error: " << ex.what() << "\n";
        return -1;
    }
    return 0;
}