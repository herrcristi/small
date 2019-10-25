#pragma once

namespace small
{
    // quick hash function h = h * 131 + char
    inline unsigned long long quick_hash            ( const char* buffer, const int length, unsigned long long start_hash = 0 )
    {
        if ( buffer == nullptr )
            return start_hash;

        for ( int i = 0; i < length; ++i, ++buffer )
        {
            start_hash = (start_hash << 7) + (start_hash << 1) + start_hash + (unsigned char)*buffer;
        }
        return start_hash;
    }
}