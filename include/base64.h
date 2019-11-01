#pragma once

#include "impl/base64_impl.h"


//
// std::string b64 = small::tobase64_s( "hello world" );
// std::vector<char> vb64 = small::tobase64_v( "hello world", 11 );
// 
// std::string decoded = small::frombase64_s( b64 );
// std::vector<char> vd64 = small::frombase64_v( b64 );
//
namespace small
{
    //
    // implementations for common data
    //
    
    // tobase64
    template<typename T>
    inline void         tobase64                    ( const char* src, const int& src_length, T* base64 )
    {
        if ( base64 == nullptr )
            return;

        int base64_size = base64::get_base64_size( src_length );
        base64->resize( base64_size );
        base64::tobase64( (char *)base64->data(), src, src_length );
    }
    
    // to base64
    template<typename T>
    inline void         tobase64                    ( const std::string& src, T* base64     ) { return tobase64( src.c_str(), (int)src.size(), base64 ); }
    
    
    // frombase64
    template<typename T>
    inline void         frombase64                  ( const char* base64, const int& base64_length, T* decoded )
    {
        if ( decoded == nullptr )
            return;

        int decoded_size = base64::get_decodedbase64_size( base64_length );
        
        decoded->resize( decoded_size );
        int decoded_length = base64::frombase64( (char*)decoded->data(), base64, base64_length );
        decoded->resize( decoded_length );
    }
    
    // frombase64
    template<typename T>
    inline void         frombase64                  ( const std::string& base64, T* decoded ) { frombase64( base64.c_str(), (int)base64.size(), decoded ); }





    //////////////////////////////////////////////////////////////////////////
    // as string
    inline std::string  tobase64_s                  ( const char* src, const int& src_length ) { std::string base64; tobase64( src, src_length, &base64 ); return base64; }
    inline std::string  tobase64_s                  ( const std::string& src                 ) { return tobase64_s( src.c_str(), (int)src.size() ); }

    // as buffer vector<char>
    inline std::vector<char> tobase64_v             ( const char* src, const int& src_length ) { std::vector<char> base64; tobase64( src, src_length, &base64 ); return base64; }    
    inline std::vector<char> tobase64_v             ( const std::string& src                 ) { return tobase64_v( src.c_str(), (int)src.size() ); }



    // from base64_s
    inline std::string  frombase64_s                ( const char* base64, const int& base64_length ) { std::string decoded; frombase64( base64, base64_length, &decoded ); return decoded; }
    inline std::string  frombase64_s                ( const std::string& base64              ) { return frombase64_s( base64.c_str(), (int)base64.size() ); }


    // frombase64_v
    inline std::vector<char> frombase64_v           ( const char* base64, const int& base64_length ) { std::vector<char> decoded; frombase64( base64, base64_length, &decoded ); return decoded; }
    inline std::vector<char> frombase64_v           ( const std::string& base64              ) { return frombase64_v( base64.c_str(), (int)base64.size() ); }

    
}