#include <iostream>
#include <memory>

int main() {
    {
        std::clog << "long std::string\n";
        std::string s("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    }

    {
        std::clog << "20 item unsigned int array\n";
        std::unique_ptr<unsigned int[]> out_row(new unsigned int[20]);
    }

    {
        std::clog << "direct malloc of unsigned int pointer\n";
        unsigned int *ptr_one = (unsigned int *)malloc(sizeof(unsigned int));
        free(ptr_one);
    }

    return 0;
}