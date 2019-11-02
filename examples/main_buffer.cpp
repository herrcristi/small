#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <map>

// make sure the path is included correct
#include "small/include/buffer.h"



int main()
{
    std::cout << "hello" << std::endl;
    
    small::buffer b;
    b.clear();

    b.set( "ana", 3 );
    b.set( "b", 1, 2 );
    
    char* e = b.extract(); // extract "anb"
    free( e );
    
    b.append( "hello", 5 );
    b.clear( true );

    char* e1 = b.extract(); // extract ""
    free( e1 );

    b.append( "world", 5 );
    b.clear();

    
    std::cout << "finishing" << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );

    return 0;
}