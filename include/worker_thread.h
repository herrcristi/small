#pragma once

#include <vector>
#include <thread>

#include "event_queue.h"

namespace small
{

    // small class for worker threads
    template<class T, typename _Callable>
    class worker_thread
    {
    public:
        // do it like it is done in thread
        worker_thread                               ( const int& threads_count /*= 1*/ ) : threads_( threads_count )  {  }

        ~worker_thread                              () { signal_exit(); stop_threads(); }
           
        

        // size
        size_t          size                        () const { return queue_items_.size();  }
        // empty
        bool            empty                       () const { return size() == 0; }
        
        // clear
        void            clear                       () { queue_items_.clear(); }
        


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
        worker_thread                               ( worker_thread&        ) = delete;
        worker_thread                               ( worker_thread&&       ) = delete;
        worker_thread                               ( const worker_thread&  ) = delete;
        worker_thread                               ( const worker_thread&& ) = delete;

        worker_thread&        operator=             ( const worker_thread&  ) = delete;
        worker_thread&        operator=             ( worker_thread&& __t   ) = delete;
        

    private:
        // create threads
        void            start_threads               ()
        {
            // todo
            // check bool if threads were started
            
            std::unique_lock< small::event_queue<T>> mlock( queue_items_ );
            for ( auto & th : threads_ )
            {
                if ( !th.joinable() )
                {
                    th = std::move( std::thread( [&]( ) 
                    {
                        while ( 1 )
                        {
                            T elem;
                            small::EnumEventQueue  ret = queue_items_.wait_pop_front_for( std::chrono::seconds( 3 ), &elem );
                            
                            if ( ret == small::EnumEventQueue::kQueue_Exit )
                            {
                                std::cout << "exit" << std::endl; break;
                            }
                            else if ( ret == small::EnumEventQueue::kQueue_Timeout )
                            {
                                std::cout << "timeout" << std::endl;
                            }
                            else if ( ret == small::EnumEventQueue::kQueue_Element )
                            {
                                processing_function_( elem );
                                std::cout << "element=" << elem << std::endl;
                            }
                        }
                    } ) );
                }
            }
        }

        // stop threads
        void            stop_threads                ()
        {
            std::vector<std::thread> threads;
            {
                std::unique_lock< small::event_queue<T>> mlock( queue_items_ );
                threads = std::move( threads_ );
            }
            for ( auto& th : threads )
            {
                if ( th.joinable() )
                {
                    th.join();
                }
            }
        }

        // process
        void Process()
        {
            for ( auto item : queue_items_ )
            {
                processing_function_( item );
            }
        }

    private:
        // threads
        std::vector<std::thread> threads_;

        // queue of items
        small::event_queue<T> queue_items_;

        // processing Function
        _Callable       processing_function_;
    };
}

