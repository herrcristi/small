#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <condition_variable>
#include <thread>

// make sure the path is included correct
#include "small/include/event.h"
#include "small/include/event_queue.h"



int main()
{
    std::cout << "hello" << std::endl;


    using qc = std::pair<int, std::string>;
    small::event_queue<qc> q;

    std::thread t( []( small::event_queue<qc> & q ) {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
        std::cout << "push 1, B" << std::endl;
        q.push_back( { 1, "B" } );

        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
        std::cout << "push 2, a" << std::endl;
        q.emplace_back( 2, "a" );

        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
        std::cout << "signal exit" << std::endl;
        q.signal_exit();
    }, std::ref( q ) );
    
    
    qc e;
    auto ret = q.wait_pop_front_for( std::chrono::milliseconds( 1 ), &e );
    std::cout << "ret=" << static_cast<int>(ret) << ", pop " << e.first << "," << e.second << std::endl;

    ret = q.wait_pop_front( &e );
    std::cout << "ret=" << static_cast<int>(ret) << ", pop " << e.first << "," << e.second << std::endl;

    ret = q.wait_pop_front( &e );
    std::cout << "ret=" << static_cast<int>(ret) << ", pop " << e.first << "," << e.second << std::endl;

    ret = q.wait_pop_front( &e );
    std::cout << "ret=" << static_cast<int>(ret) << ", pop " << e.first << "," << e.second << std::endl;

    t.join();
    
   
    std::cout << "finishing" << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );

    return 0;
}