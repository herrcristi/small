#pragma once

#include <vector>
#include <thread>
#include <functional>


#include "event_queue.h"

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
        size_t          size                        () const { return queue_items_.size();  }
        // empty
        bool            empty                       () const { return size() == 0; }
        // clear
        void            clear                       () { queue_items_.clear(); }



        // use it as locker (std::unique_lock<small:worker_thread<T>> m...)
        void            lock                        () { queue_items_.lock();   }
        void            unlock                      () { queue_items_.unlock(); }
        bool            try_lock                    () { return queue_items_.try_lock(); }
        



        // add items to be processed
        // push_back
        void            push_back                   ( const T& t ) { if ( is_exit() ) { return; } queue_items_.push_back( std::forward<T>( t ) ); start_threads(); }
        void            push_back                   ( T&& t      ) { if ( is_exit() ) { return; } queue_items_.push_back( std::forward<T>( t ) ); start_threads(); }
        // emplace_back
        template<typename... _Args>
        void            emplace_back                ( _Args&&... __args ) { if ( is_exit() ) { return; } queue_items_.emplace_back( std::forward<_Args>( __args )... ); start_threads(); }
        


        // signal exit
        void            signal_exit                 () { queue_items_.signal_exit(); }
        bool            is_exit                     () { return queue_items_.is_exit(); }


    private:
        worker_thread                               ( const worker_thread&  ) = delete;
        //worker_thread                               ( worker_thread&        ) = delete;
        worker_thread                               ( worker_thread&&       ) = delete;
        //worker_thread                               ( const worker_thread&& ) = delete;

        worker_thread&        operator=             ( const worker_thread&  ) = delete;
        worker_thread&        operator=             ( worker_thread&& __t   ) = delete;
        

    private:


        // thread function
        void thread_function()
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
        void            start_threads               ()
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
                    th = std::move( std::thread( /*[&]() { thread_function(); }*/&worker_thread::thread_function, this ) );
                }
            }
            // mark threads were created
            threads_flag_created_.store( true );
        }


        // stop threads
        void            stop_threads                ()
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

