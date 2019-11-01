#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <map>

// make sure the path is included correct
#include "small/include/util.h"


int main()
{
    std::cout << "hello" << std::endl;

    std::map<std::string, int, small::icasecmp> m;
    m["B"] = 2;
    m["a"] = 1;

    int r = small::stricmp( "a", "C" );



    return 0;
}