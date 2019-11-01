# small
Small project

Contains usefull everyday features to be used like:

* spinlock (or critical_section)
* event
* event_queue
* worker_thread

#
* base64
* quick_hash
* util functions

#

### spinlock (or critical_section)
Spinlock is just like a mutex but it uses atomic lockless to do locking (based on std::atomic_flag).

The following functions are available
```lock, unlock, try_lock```

Use it like this
```
small::spinlock lock; // small::critical_section lock;
...
{
    std::unique_lock<small::spinlock> mlock( lock );
    
    // do your work
    ...
}
```


#

### event
Event is based on mutex and condition_variable

##### !!Important!! An automatic event stay set until it is consumed, a manual event stay set until is reseted

The main functions are 

```set_event, reset_event```

```wait, wait_for, wait_until```

Also these functions are available (thanks to mutex)

```lock, unlock, try_lock```


Use it like this
```
small::event e;
...
{
    std::unique_lock<small::event> mlock( e );
    ...
}
...
e.set_event();
...

// on some thread
e.wait();
// or
e.wait( [&]() -> bool {
    return /*some conditions*/ ? true : false;
} );
...
```

or

```
small::event e( small::EventType::kEvent_Manual );
...
...
e.set_event();
...

// on some thread
e.wait();
...
// somewhere else
e.reset_event()
```


#

### event_queue
A queue with events functions to wait for items to be available

The following functions are available

For container

```size, empty, clear, reset```

```push_back, emplace_back```

For events or locking

```lock, unlock, try_lock```

Wait for items

```wait_pop_front, wait_pop_front_for, wait_pop_front_until```

Signal exit when we no longer want to use the queue

```signal_exit, is_exit```


Use it like this

```
small::event_queue<int> q;
...
q.push_back( 1 );
...

// on some thread
int e = 0;
auto ret = q.wait_pop_front( &e ); 
//auto ret = q.wait_pop_front_for( std::chrono::minutes( 1 ), &e ); 

// ret can be small::EnumEventQueue::kQueue_Exit, 
// small::EnumEventQueue::kQueue_Timeout or ret == small::EnumEventQueue::kQueue_Element

if ( ret == small::EnumEventQueue::kQueue_Element )
 { 
     // do something with e
    ... 
}

...
// on main thread, no more processing
q.signal_exit();

```


#

### worker_thread
A class that creates several threads for producer/consumer

The following functions are available

For data

```size, empty, clear```

```push_back, emplace_back```

To use it as a locker

```lock, unlock, try_lock```

Signal exit when we no longer want to use worker threads, 
usefull when we have multiple objects that do some stuff that takes some time on destructor, 
so until it is the turn of the destructor of this element we may already closed all the working threads

```signal_exit, is_exit```


Use it like this
```
using qc = std::pair<int, std::string>;
...
// with a lambda for processing working function
small::worker_thread<qc> workers( 2, []( auto& w/*this*/, auto& item, auto b/*extra param*/ ) -> void
{
    {
        std::unique_lock< small::worker_thread<qc>> mlock( w ); // use worker_thread to lock
        ...
        //std::cout << "thread " << std::this_thread::get_id()  
        // << "processing " << item.first << " " << item.second << " b=" << b << "\n";
    }
}, 5/*extra param*/ );
...
// or like this
small::worker_thread<qc> workers2( 1, WorkerThreadFunction() );
...
// where WorkerThreadFunction can be
struct WorkerThreadFunction
{
    using qc = std::pair<int, std::string>;
    void operator()( small::worker_thread<qc>& w/*worker_thread*/, qc& item )
    {
        ...
        // add extra in queue
        // w.push_back(...)
        
        std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
    }
};
..
...
workers.push_back( { 1, "a" } );
workers.push_back( std::make_pair( 2, "b" ) );
workers.emplace_back( 3, "e" );
...
// when finishing after signal_exit the work is aborted
workers.signal_exit();
//

```



## Utilities


### base64
Functions to encode or decode base64

The following function is available
```tobase64, frombase64```

and additionals for string and vector<char>
```tobase64_s, tobase64_v, frombase64_s, frombase64_v```
and other can be addedUse it like this
```
std::string b64 = small::tobase64_s( "hello world" );
std::vector<char> vb64 = small::tobase64_v( "hello world", 11 );
   
std::string decoded = small::frombase64_s( b64 );
std::vector<char> vd64 = small::frombase64_v( b64 );
```


#

### quick_hash
When you want to do a simple hash

The following function is available
```quick_hash```

Use it like this
```
unsigned long long h = small::quick_hash( "some text", 9/*strlen(...)*/ );
...
// or you can used like this
unsigned long long h1 = small::quick_hash( "some ", 5/*strlen(...)*/ );
unsigned long long h2 = small::quick_hash( "text",  4/*strlen(...)*/, h1/*continue from h1*/ );
```


#

### util
Utility functions or defines

The following functions are available


```stricmp, struct icasecmp```

Use it like this
```
int r = small::stricmp( "a", "C" );
...
std::map<std::string, int, small::icasecmp> m;
```


