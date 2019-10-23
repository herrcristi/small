#pragma once

#include <deque>
#include <atomic>

#include "event.h"

namespace small
{
    // enum
    enum class EnumEventQueue
    {
        kQueue_Element,
        kQueue_Timeout,
        kQueue_Exit,
    };

    // queue with events
    template<typename T>
    class event_queue
    {
    public:
        // size
        size_t          size                        () const {  std::unique_lock<small::event> mlock( event_ ); return queue_.size();  }
        // empty
        bool            empty                       () const { return size() == 0; }


        // push_back
        void            push_back                   ( const T& t ) { { std::unique_lock<small::event> mlock( event_ ); queue_.push_back( std::forward<T>( t ) ); } event_.set_event(); }
        void            push_back                   ( T&& t      ) { { std::unique_lock<small::event> mlock( event_ ); queue_.push_back( std::forward<T>( t ) ); } event_.set_event(); }
        // emplace_back
        template<typename... _Args>
        void            emplace_back                ( _Args&&... __args ) { { std::unique_lock<small::event> mlock( event_ );  queue_.emplace_back( std::forward<_Args>( __args )... ); } event_.set_event(); }
        
        // quit
        void            signal_exit                 () { exit_flag_ = true; event_.set_event(); }



        // wait pop_front and return that element
        EnumEventQueue  wait_pop_front              ( T * elem )
        {
            if ( exit_flag_.load() == true )
                return EnumEventQueue::kQueue_Exit;

            // wait
            event_.wait( [&]() -> bool {
                return test_and_get_front( elem );
            } );

            if ( exit_flag_.load() == true )
                return EnumEventQueue::kQueue_Exit;

            return EnumEventQueue::kQueue_Element;
        }


        // wait pop_front_for and return that element
        template<typename _Rep, typename _Period>
        EnumEventQueue  wait_pop_front_for          ( const std::chrono::duration<_Rep, _Period>& __rtime, T* elem )
        {
            using __dur = typename std::chrono::system_clock::duration;
            auto __reltime = std::chrono::duration_cast<__dur>(__rtime);
            if ( __reltime < __rtime )
                ++__reltime;
            return wait_pop_front_until( std::chrono::system_clock::now() + __reltime, elem );
        }



        // wait until
        template<typename _Clock, typename _Duration>
        EnumEventQueue  wait_pop_front_until        ( const std::chrono::time_point<_Clock, _Duration>& __atime, T* elem )
        {
            if ( exit_flag_.load() == true )
                return EnumEventQueue::kQueue_Exit;

            // wait
            bool ret = event_.wait_until( __atime, [&]() -> bool {
                return test_and_get_front( elem );
            } );

            if ( exit_flag_.load() == true )
                return EnumEventQueue::kQueue_Exit;

            return ret ? EnumEventQueue::kQueue_Element : EnumEventQueue::kQueue_Timeout;
        }



    private:
        // check for front element
        bool            test_and_get_front          ( T* elem )
        {
            if ( exit_flag_.load() == true )
                return true;

            if ( queue_.empty() )
                return false;

            if ( elem )
                *elem = std::move( queue_.front() );
            queue_.pop_front();
            return true;
        }


    private:
        // queue
        std::deque<T>   queue_;
        // event
        small::event    event_;
        // exit flag
        std::atomic<bool> exit_flag_ = false;
    };
}
