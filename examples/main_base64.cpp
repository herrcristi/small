#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <map>

// make sure the path is included correct
#include "small/include/base64.h"


int main()
{
    std::cout << "hello" << std::endl;

    std::string b64 = small::tobase64_s( "hello world" );
    std::vector<char> vb64 = small::tobase64_v( "hello world", 11 );
   
    std::string decoded = small::frombase64_s( b64 );
    std::vector<char> vd64 = small::frombase64_v( b64 );

   
    return 0;
}