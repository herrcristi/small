# small
Small project

Contains everyday features to be used

# spinlock
Spinlock is just like a mutex but it uses atomic lockless to do locking.
The following functions are available
lock, unlock, try_lock

Use it like this
small::spinlock lock;
...
{
    std::unique_lock<small::spinlock> mlock( lock );
    ...
}
