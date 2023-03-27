#pragma once

#include <vector>
#include <thread>
#include <functional>


#include "event_queue.h"


// using qc = std::pair<int, std::string>;
// ...
// // with a lambda for processing working function
// small::worker_thread<qc> workers( 2, []( auto& w/*this*/, auto& item, auto b/*extra param*/ ) -> void
// {
//     {
//         std::unique_lock< small::worker_thread<qc>> mlock( w ); // use worker_thread to lock
//         ...
//         //std::cout << "thread " << std::this_thread::get_id() << "processing " << item.first << " " << item.second << " b=" << b << "\n";
//     }
// }, 5/*extra param*/ );
// ...
// // or like this
// small::worker_thread<qc> workers2( 1, WorkerThreadFunction() );
// ...
// // where WorkerThreadFunction can be
// struct WorkerThreadFunction
// {
//     using qc = std::pair<int, std::string>;
//     void operator()( small::worker_thread<qc>& w/*worker_thread*/, qc& item )
//     {
//         ...
//         // add extra in queue
//         // w.push_back(...)
// 
//         std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
//     }
// };
// ..
// ...
// workers.push_back( { 1, "a" } );
// workers.push_back( std::make_pair( 2, "b" ) );
// workers.emplace_back( 3, "e" );
// ...
// // when finishing after signal_exit the work is aborted
// workers.signal_exit();
// //

namespace small
{

    // small class for worker threads
    template<class T>
    class worker_thread
    {
    public:
        // worker_thread
        template<typename _Callable, typename... Args>
        worker_thread                               ( const int& threads_count /*= 1*/, _Callable function, Args... parameters ) : threads_( threads_count ), threads_flag_created_( false ),
                                                        processing_function_( std::bind( std::forward<_Callable>( function ), std::ref(*this), std::placeholders::_1/*, std::placeholders::_2*/, std::forward<Args>( parameters )... ) ){ }

        ~worker_thread                              () { signal_exit(); stop_threads(); }
           
        

        // size
        inline size_t   size                        () const { return queue_items_.size();  }
        // empty
        inline bool     empty                       () const { return size() == 0; }
        // clear
        inline void     clear                       () { queue_items_.clear(); }



        // use it as locker (std::unique_lock<small:worker_thread<T>> m...)
        inline void     lock                        () { queue_items_.lock();   }
        inline void     unlock                      () { queue_items_.unlock(); }
        inline bool     try_lock                    () { return queue_items_.try_lock(); }
        



        // add items to be processed
        // push_back
        inline void     push_back                   ( const T& t ) { if ( is_exit() ) { return; } queue_items_.push_back( std::forward<T>( t ) ); start_threads(); }
        inline void     push_back                   ( T&& t      ) { if ( is_exit() ) { return; } queue_items_.push_back( std::forward<T>( t ) ); start_threads(); }
        // emplace_back
        template<typename... _Args>
        inline void     emplace_back                ( _Args&&... __args ) { if ( is_exit() ) { return; } queue_items_.emplace_back( std::forward<_Args>( __args )... ); start_threads(); }
        


        // signal exit
        inline void     signal_exit                 () { queue_items_.signal_exit(); }
        inline bool     is_exit                     () { return queue_items_.is_exit(); }


    private:
        worker_thread                               ( const worker_thread&  ) = delete;
        //worker_thread                               ( worker_thread&        ) = delete;
        worker_thread                               ( worker_thread&&       ) = delete;
        //worker_thread                               ( const worker_thread&& ) = delete;

        worker_thread&        operator=             ( const worker_thread&  ) = delete;
        worker_thread&        operator=             ( worker_thread&& __t   ) = delete;
        

    private:


        // thread function
        inline void     thread_function             ()
        {
            T elem;
            while ( 1 )
            {
                // wait
                small::EnumEventQueue  ret = queue_items_.wait_pop_front/*_for*/( /*std::chrono::minutes( 10 ),*/ &elem );

                if ( ret == small::EnumEventQueue::kQueue_Exit )
                {
                    break;
                }
                else if ( ret == small::EnumEventQueue::kQueue_Timeout )
                {

                }
                else if ( ret == small::EnumEventQueue::kQueue_Element )
                {
                    processing_function_( std::ref( elem ) );
                }
            }
        }



        // create threads
        inline void     start_threads               ()
        {
            // check first if threads were already started
            if ( threads_flag_created_.load() == true )
                return;
            
            std::unique_lock< small::event_queue<T>> mlock( queue_items_ );
            
            // check again
            if ( threads_flag_created_.load() == true )
                return;
            
            // create threads
            for ( auto & th : threads_ )
            {
                if ( !th.joinable() )
                {
                    th = std::thread( /*[&]() { thread_function(); }*/&worker_thread::thread_function, this );
                }
            }
            // mark threads were created
            threads_flag_created_.store( true );
        }


        // stop threads
        inline void     stop_threads                ()
        {
            std::vector<std::thread> threads;
            {
                std::unique_lock< small::event_queue<T>> mlock( queue_items_ );
                threads = std::move( threads_ );
                threads_flag_created_.store( false );
            }
            // wait for threads to stop
            for ( auto& th : threads )
            {
                if ( th.joinable() )
                {
                    th.join();
                }
            }
        }

        
        

    private:
        // threads
        std::vector<std::thread>    threads_;
        // threads flag
        std::atomic<bool>           threads_flag_created_;

        // queue of items
        small::event_queue<T>       queue_items_;

        // processing Function
        std::function<void( T& )>   processing_function_;
    };
}

