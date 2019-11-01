#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <map>

// make sure the path is included correct
#include "small/include/hash.h"


int main()
{
    std::cout << "hello" << std::endl;

    unsigned long long hash = small::quick_hash( "some text", 9/*strlen*/, 0 );
    
    unsigned long long h1 = small::quick_hash( "some ", 5/*strlen*/, 0 );
    unsigned long long h2 = small::quick_hash( "text",  4/*strlen*/, h1 );

    
    return 0;
}