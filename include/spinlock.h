#pragma once

#include <atomic>
#include <thread>
#include <chrono>

// spin lock - just like mutex it uses atomic lockless to do locking
// 
// small::spinlock lock; // small::critical_section lock;
// ...
// {
//     std::unique_lock<small::spinlock> mlock( lock );
//
//    // do your work
//    ...
// }
// 
namespace small
{
    // spinlock
    class spinlock
    {
    public:
        spinlock                                    ( const int & spin_count = 4000 ) : /*lock_( ATOMIC_FLAG_INIT ),*/ spin_count_( spin_count ){}

        // lock functions
        inline void     lock                        ()
        { 
            for ( int count = 0; lock_.test_and_set( std::memory_order_acquire ); ++count )
            {
                if ( count >= spin_count_ )
                    std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) ); // std::this_thread::yield();
            }
        }
        
        // unlock
        inline void     unlock                      () { lock_.clear( std::memory_order_release ); }
        
        // try lock
        inline bool     try_lock                    () { return lock_.test_and_set( std::memory_order_acquire ) ? /*before was true so no lock*/false : /*lock*/true; }

    private:
        // members
        std::atomic_flag lock_;
        int             spin_count_;
    };
}