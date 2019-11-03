#pragma once

#include <array>
#include <string>

namespace small
{
    // a string class that allocates on stack
    template<std::size_t StackAllocSize = 256>
    class stack_string
    {
    public:
        stack_string                                () { stack_string_.fill( '\0' ); }
        ~stack_string                               () { }

    
        // todo
    
    private:
        // string by default uses the array and if it goes beyond makes use of the std_string
        std::array<char, StackAllocSize> stack_string_;
        std::unique_ptr<std::string> std_string_;
    };
}