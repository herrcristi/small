# small
Small project

Contains everyday features to be used
* spinlock
* event
* event_queue

* base64
* quick_hash
* util functions



### spinlock
Spinlock is just like a mutex but it uses atomic lockless to do locking (based on std::atomic_flag).

The following functions are available
```lock, unlock, try_lock```

Use it like this
```
small::spinlock lock;
...
{
    std::unique_lock<small::spinlock> mlock( lock );
    ...
}
```


### event
Event is based on mutex and condition_variable

!!Important!! An automatic event stay set until it is consumed, a manual event stay set until is reseted

The following functions are available

```set_event, reset_event, wait, wait_for, wait_until```

Also these functions are available

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

e.wait();
```


### event_queue
A queue with events functions

The following functions are available

```size, push_back, emplace_back, wait_pop_front, wait_pop_front_for, wait_pop_front_until, signal_exit```


Use it like this
```
small::event_queue<int> q;
...
q.push_back( 1 ); // q.signal_exit();
...
int e = 0;
auto ret = q.wait_pop_front( &e );
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


