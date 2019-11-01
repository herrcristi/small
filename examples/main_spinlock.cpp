#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <condition_variable>

// make sure the path is included correct
#include "small/include/spinlock.h"


int main()
{
    std::cout << "hello" << std::endl;

    // spinlock
    small::spinlock lock;

    // thread function
    auto fn_t = []( auto i, small::spinlock& lock )
    {
    
        for ( int t = 0; t < 5; ++t )
        {
            {
                std::unique_lock<small::spinlock> mlock( lock );

                //std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

                std::cout << "thread=" << i << ", iteration=" << t << std::endl;
            }
            std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        }
    };


    // create thread
    std::thread t[3];
    for ( size_t i = 0; i < sizeof( t ) / sizeof( t[0] ); ++i )
    {
        t[i] = std::move( std::thread( fn_t, i, std::ref( lock ) ) );
    }

    
    usleep( 1'000'000 );

    // wait
    for ( size_t i = 0; i < sizeof( t ) / sizeof( t[0] ); ++i )
    {
        if ( t[i].joinable() )
            t[i].join();
    }

    
    std::cout << "finishing" << std::endl;
    usleep( 3'000'000 );

    return 0;
}