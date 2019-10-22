# small
Small project

Contains everyday features to be used

## spinlock
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


## event
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