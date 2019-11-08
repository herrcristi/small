#pragma once

#ifndef SMALL_IMPL_BASE_base_buffer
#define SMALL_IMPL_BASE_base_buffer

#include <string.h>
#include <stddef.h>

#include <functional>
#include <type_traits>
#include <string>
#include <string_view>
#include <vector>


namespace small
{
    // class for representing a base_buffer that implements 
    // all the needed functions and operators
    // it must be supplied with derived class with proper functions
    class base_buffer
    {
    protected:
        // base_buffer (allocates in chunks)
        base_buffer                                 () : empty_buffer_{ '\0' }, buffer_data_{ nullptr }, buffer_length_{ 0 } {}
        ~base_buffer                                () {}
        
    public:
         // functions for getting size size / length / empty 
        inline size_t   size                        () const    { return buffer_length_;   }
        inline size_t   length                      () const    { return size();           }
        inline bool     empty                       () const    { return size() == 0;      }
                
        
        // functions to supply that affect size 
        // clear / reserve / resize / shrink_to_fit
        inline void     clear                       ()                  { fn_clear_();              }
        inline void     reserve                     ( size_t new_size ) { fn_reserve_( new_size );  }
        inline void     resize                      ( size_t new_size ) { fn_resize_ ( new_size );  }
        inline void     shrink_to_fit               ()                  { fn_shrink_();             }

        
        // data access to buffer
        inline const char* get_buffer               () const    { return buffer_data_; }
        inline char*       get_buffer               ()          { return buffer_data_; } // direct access
        
        inline const char* data                     () const    { return buffer_data_; }
        inline char*       data                     ()          { return buffer_data_; }


        // conversion as c_string
        inline std::string      c_string            () const    { return std::string( data(), size() ); }
        inline std::vector<char>c_vector            () const    { std::vector<char> v; v.reserve( size() + 1 ); v.resize( size() ); memcpy( v.data(), data(), size() ); return v; }
        inline std::string_view c_view              () const    { return std::string_view{ data(), size() }; }


        // assign
        inline void     assign                      ( const base_buffer& b      )   { if ( this != &b ) { set( b.data(), b.size(), 0/*startfrom*/ ); } }
        
        inline void     assign                      ( const char  c             )   { set( &c,          1,          0/*startfrom*/ ); }
        inline void     assign                      ( const char* s             )   { set( s,           strlen( s ),0/*startfrom*/ ); }
        inline void     assign                      ( const char* s, size_t len )   { set( s,           len,        0/*startfrom*/ ); }
        inline void     assign                      ( const std::string& s      )   { set( s.c_str(),   s.size(),   0/*startfrom*/ ); }
        inline void     assign                      ( const std::string_view s  )   { set( s.data(),    s.size(),   0/*startfrom*/ ); }
        inline void     assign                      ( const std::vector<char>& v)   { set( v.data(),    v.size(),   0/*startfrom*/ ); }

        inline void     assign                      ( const wchar_t  c          )   { set( &c,          1,          0/*startfrom*/ ); }
        inline void     assign                      ( const wchar_t* s          )   { set( s,           wcslen( s ),0/*startfrom*/ ); }
        inline void     assign                      ( const wchar_t* s, size_t len ){ set( s,           len,        0/*startfrom*/ ); }
        inline void     assign                      ( const std::wstring& s     )   { set( s.c_str(),   s.size(),   0/*startfrom*/ ); }
        inline void     assign                      ( const std::wstring_view s )   { set( s.data(),    s.size(),   0/*startfrom*/ ); }
        inline void     assign                      ( const std::vector<wchar_t>& v){ set( v.data(),    v.size(),   0/*startfrom*/ ); }


        // append
        inline void     append                      ( const base_buffer& b      )   { set( b.data(),    b.size(),   size()/*startfrom*/ ); }
        
        inline void     append                      ( const char  c             )   { set( &c,          1,          size()/*startfrom*/ ); }
        inline void     append                      ( const char* s             )   { set( s,           strlen( s ),size()/*startfrom*/ ); }
        inline void     append                      ( const char* s, size_t len )   { set( s,           len,        size()/*startfrom*/ ); }
        inline void     append                      ( const std::string& s      )   { set( s.c_str(),   s.size(),   size()/*startfrom*/ ); }
        inline void     append                      ( const std::string_view s  )   { set( s.data(),    s.size(),   size()/*startfrom*/ ); }
        inline void     append                      ( const std::vector<char>& v)   { set( v.data(),    v.size(),   size()/*startfrom*/ ); }

        inline void     append                      ( const wchar_t  c          )   { set( &c,          1,          size()/*startfrom*/ ); }
        inline void     append                      ( const wchar_t* s          )   { set( s,           wcslen(s),  size()/*startfrom*/ ); }
        inline void     append                      ( const wchar_t* s, size_t len ){ set( s,           len,        size()/*startfrom*/ ); }
        inline void     append                      ( const std::wstring& s     )   { set( s.c_str(),   s.size(),   size()/*startfrom*/ ); }
        inline void     append                      ( const std::wstring_view s )   { set( s.data(),    s.size(),   size()/*startfrom*/ ); }
        inline void     append                      ( const std::vector<wchar_t>& v){ set( v.data(),    v.size(),   size()/*startfrom*/ ); }


        // insert
        inline void     insert                      ( size_t from, const base_buffer& b      )  { fn_insert_( from, b.data(),   b.size()    ); }
                        
        inline void     insert                      ( size_t from, const char  c             )  { fn_insert_( from, &c,         1           ); }
        inline void     insert                      ( size_t from, const char* s             )  { fn_insert_( from, s,          strlen( s ) ); }
        inline void     insert                      ( size_t from, const char* s, size_t len )  { fn_insert_( from, s,          len         ); }
        inline void     insert                      ( size_t from, const std::string& s      )  { fn_insert_( from, s.c_str(),  s.size()    ); }
        inline void     insert                      ( size_t from, const std::string_view s  )  { fn_insert_( from, s.data(),   s.size()    ); }
        inline void     insert                      ( size_t from, const std::vector<char>& v)  { fn_insert_( from, v.data(),   v.size()    ); }
                        
        inline void     insert                      ( size_t from, const wchar_t  c          )  { fn_insertw_( from, &c,        1           ); }
        inline void     insert                      ( size_t from, const wchar_t* s          )  { fn_insertw_( from, s,         wcslen( s ) ); }
        inline void     insert                      ( size_t from, const wchar_t* s, size_t len ){fn_insertw_( from, s,         len         ); }
        inline void     insert                      ( size_t from, const std::wstring& s     )  { fn_insertw_( from, s.c_str(), s.size()    ); }
        inline void     insert                      ( size_t from, const std::wstring_view s )  { fn_insertw_( from, s.data(),  s.size()    ); }
        inline void     insert                      ( size_t from, const std::vector<wchar_t>& v){fn_insertw_( from, v.data(),  v.size()    ); }

        
        // overwrite
        inline void     overwrite                   ( size_t from, const base_buffer& b      )  { set( b.data(),    b.size(),   from ); }
                        
        inline void     overwrite                   ( size_t from, const char  c             )  { set( &c,          1,          from ); }
        inline void     overwrite                   ( size_t from, const char* s             )  { set( s,           strlen( s ),from ); }
        inline void     overwrite                   ( size_t from, const char* s, size_t len )  { set( s,           len,        from ); }
        inline void     overwrite                   ( size_t from, const std::string& s      )  { set( s.c_str(),   s.size(),   from ); }
        inline void     overwrite                   ( size_t from, const std::string_view s  )  { set( s.data(),    s.size(),   from ); }
        inline void     overwrite                   ( size_t from, const std::vector<char>& v)  { set( v.data(),    v.size(),   from ); }
                        
        inline void     overwrite                   ( size_t from, const wchar_t  c          )  { set( &c,          1,          from ); }
        inline void     overwrite                   ( size_t from, const wchar_t* s          )  { set( s,           wcslen( s ),from ); }
        inline void     overwrite                   ( size_t from, const wchar_t* s, size_t len ){ set( s,          len,        from ); }
        inline void     overwrite                   ( size_t from, const std::wstring& s     )  { set( s.c_str(),   s.size(),   from ); }
        inline void     overwrite                   ( size_t from, const std::wstring_view s )  { set( s.data(),    s.size(),   from ); }
        inline void     overwrite                   ( size_t from, const std::vector<wchar_t>& v){set( v.data(),    v.size(),   from ); }


        // set
        inline void     set                         ( const base_buffer& b,         size_t from = 0 ) { fn_set_( from, b.data(),    b.size()    ); }
                        
        inline void     set                         ( const char  c,                size_t from = 0 ) { fn_set_( from, &c,          1           ); }
        inline void     set                         ( const char* s,                size_t from = 0 ) { fn_set_( from, s,           strlen( s ) ); }
        inline void     set                         ( const char* s, size_t len,    size_t from = 0 ) { fn_set_( from, s,           len         ); }
        inline void     set                         ( const std::string& s,         size_t from = 0 ) { fn_set_( from, s.c_str(),   s.size()    ); }
        inline void     set                         ( const std::string_view s,     size_t from = 0 ) { fn_set_( from, s.data(),    s.size()    ); }
        inline void     set                         ( const std::vector<char>& v,   size_t from = 0 ) { fn_set_( from, v.data(),    v.size()    ); }
                        
        inline void     set                         ( const wchar_t  c,             size_t from = 0 ) { fn_setw_( from, &c,         1           ); }
        inline void     set                         ( const wchar_t* s,             size_t from = 0 ) { fn_setw_( from, s,          wcslen( s ) ); }
        inline void     set                         ( const wchar_t* s, size_t len, size_t from = 0 ) { fn_setw_( from, s,          len         ); }
        inline void     set                         ( const std::wstring& s,        size_t from = 0 ) { fn_setw_( from, s.c_str(),  s.size()    ); }
        inline void     set                         ( const std::wstring_view s,    size_t from = 0 ) { fn_setw_( from, s.data(),   s.size()    ); }
        inline void     set                         ( const std::vector<wchar_t>& v,size_t from = 0 ) { fn_setw_( from, v.data(),   v.size()    ); }

        
        // erase
        inline void     erase                       ( size_t from )                 { if ( from < size() ) { resize( from ); } }
        inline void     erase                       ( size_t from, size_t length )  { fn_erase_( from, length ); }
        

        // compare
        inline bool     is_equal                    ( const char *s, size_t s_length ) const { return size() == s_length && compare( s, s_length ) == 0; }
        inline int      compare                     ( const char *s, size_t s_length ) const 
        { 
            int cmp = memcmp( data(), s, size() < s_length ? size() : s_length );  
            return (cmp != 0) ? /*different*/cmp : /*equal so far*/(size() == s_length ? 0/*true equal*/ : (size() < s_length ? -1 : 1));
        }

        inline bool     is_equal                    ( const wchar_t *s, size_t s_length ) const { return size() == s_length && compare( s, s_length ) == 0; }
        inline int      compare                     ( const wchar_t *s, size_t s_length ) const { return memcmp( data(), s, size() < s_length ? size()+1 : s_length+1 ); }

        
        // operators

        // these specific operators must be implemented in derived classes
        //inline base_buffer&  operator=                   ( const base_buffer& o           ) noexcept;
        //inline base_buffer&  operator=                   ( base_buffer&&      o           ) noexcept;
        // =
        inline base_buffer&  operator=              ( const char c              ) noexcept { set( &c,               1,          0/*from*/ ); return *this; }
        inline base_buffer&  operator=              ( const char* s             ) noexcept { set( s,                strlen(s),  0/*from*/ ); return *this; }
        inline base_buffer&  operator=              ( const std::string&  s     ) noexcept { set( s.c_str(),        s.size(),   0/*from*/ ); return *this; }
        inline base_buffer&  operator=              ( const std::string_view  s ) noexcept { set( s.data(),         s.size(),   0/*from*/ ); return *this; }
        inline base_buffer&  operator=              ( const std::vector<char>& v) noexcept { set( v.data(),         v.size(),   0/*from*/ ); return *this; }

        inline base_buffer&  operator=              ( const wchar_t c           ) noexcept { set( &c,               1,          0/*from*/ ); return *this; }
        inline base_buffer&  operator=              ( const wchar_t* s          ) noexcept { set( s,                wcslen( s ),0/*from*/ ); return *this; }
        inline base_buffer&  operator=              ( const std::wstring& s     ) noexcept { set( s.c_str(),        s.size(),   0/*from*/ ); return *this; }
        inline base_buffer&  operator=              ( const std::wstring_view s ) noexcept { set( s.data(),         s.size(),   0/*from*/ ); return *this; }
        inline base_buffer&  operator=              ( const std::vector<wchar_t>& v)noexcept{set( v.data(),         v.size(),   0/*from*/ ); return *this; }


        // +=
        inline base_buffer&  operator+=             ( const base_buffer& b      ) noexcept { append( b.data(),      b.size()    ); return *this; }
        
        inline base_buffer&  operator+=             ( const char c              ) noexcept { append( &c,            1           ); return *this; }
        inline base_buffer&  operator+=             ( const char* s             ) noexcept { append( s,             strlen(s)   ); return *this; }
        inline base_buffer&  operator+=             ( const std::string&  s     ) noexcept { append( s.c_str(),     s.size()    ); return *this; }
        inline base_buffer&  operator+=             ( const std::string_view s  ) noexcept { append( s.data(),      s.size()    ); return *this; }
        inline base_buffer&  operator+=             ( const std::vector<char>& v) noexcept { append( v.data(),      v.size()    ); return *this; }
        
        inline base_buffer&  operator+=             ( const wchar_t c           ) noexcept { append( &c,            1           ); return *this; }
        inline base_buffer&  operator+=             ( const wchar_t* s          ) noexcept { append( s,             wcslen(s)   ); return *this; }
        inline base_buffer&  operator+=             ( const std::wstring& s     ) noexcept { append( s.c_str(),     s.size()    ); return *this; }
        inline base_buffer&  operator+=             ( const std::wstring_view s ) noexcept { append( s.data(),      s.size()    ); return *this; }
        inline base_buffer&  operator+=             ( const std::vector<wchar_t>&v)noexcept{ append( v.data(),      v.size()    ); return *this; }
        

        // [] / at
        inline char&      operator[]                ( size_t index )        { return buffer_data_[ index ]; }
        inline const char operator[]                ( size_t index ) const  { return buffer_data_[ index ]; }

        inline char&      at                        ( size_t index )        { return buffer_data_[ index ]; }
        inline const char at                        ( size_t index ) const  { return buffer_data_[ index ]; }


        // front / back
        inline const char front                     () const                { return buffer_data_[ 0 ]; }
        inline char&      front                     ()                      { return buffer_data_[ 0 ]; }

        inline const char back                      () const                { return size() > 0 ? buffer_data_[size() - 1] : buffer_data_[0]; }
        inline char&      back                      ()                      { return size() > 0 ? buffer_data_[size() - 1] : buffer_data_[0]; }


        // push / pop
        inline void     push_back                   ( const char c )        { append( c ); }
        inline void     pop_back                    ()                      { if ( size() > 0 ) { resize( size() - 1 ); } }


        // operator
        inline          operator std::string_view   () const                { return c_view(); }


    protected:
        // empty buffer
        inline const char* get_empty_buffer         () const { return empty_buffer_; }

        // !! after every function call setup buffer data
        inline void     setup_buffer                ( char* buffer_data, size_t buffer_length )
        {
            buffer_data_    = buffer_data;
            buffer_length_  = buffer_length;
        }


    protected:
        // buffer set
        inline void     buffer_set_impl             ( const char* b, size_t b_length, size_t from = 0 ) 
        { 
            resize( from + b_length );  // make room

            if ( b && buffer_data_ && buffer_data_ != empty_buffer_ /*good allocation*/ )
            { 
                memcpy( buffer_data_ + from, b, b_length );
            } 
        }


        // buffer insert
        inline void     buffer_insert_impl          ( size_t from /*= 0*/, const char* b, size_t b_length )
        { 
            size_t initial_length = size();
            resize( from <= initial_length ? initial_length + b_length : from + b_length );

            if ( b && buffer_data_ && buffer_data_ != empty_buffer_ /*good allocation*/ )
            { 
                if ( from <= initial_length )
                {
                    memmove( buffer_data_ + from + b_length, buffer_data_ + from, initial_length - from );
                }
                else
                {
                    memset( buffer_data_ + initial_length, '\0', (from - initial_length) );
                }
                memcpy( buffer_data_ + from, b, b_length ); 
            } 
        }


        // erase
        inline void     buffer_erase_impl           ( size_t from, size_t length ) 
        { 
            if ( buffer_data_ && buffer_data_ != empty_buffer_ )
            {
                if ( from < size() )
                {
                    if ( from + length < size() )
                    {
                        size_t move_length = size() - (from + length);
                        memmove( buffer_data_ + from, buffer_data_ + from + length, move_length );
                        resize( size() - length );
                    }
                    else
                    {
                        resize( from );
                    }
                }
            }
        }


        
    private:
        // base_buffer empty
        char            empty_buffer_[1];
        // base_buffer use char* instead of vector<char> because it is much faster
        char *          buffer_data_;
        size_t          buffer_length_;
        
        // functions to supply
        std::function<void()>                                                                   fn_clear_;
        std::function<void( size_t/*size*/ )>                                                   fn_reserve_;
        std::function<void( size_t/*size*/ )>                                                   fn_resize_;
        std::function<void()>                                                                   fn_shrink_;

        std::function<void( size_t/*from*/, const char* /*buffer*/,     size_t/*length*/ )>     fn_set_;
        std::function<void( size_t/*from*/, const wchar_t* /*wbuffer*/, size_t/*wlength*/ )>    fn_setw_;

        std::function<void( size_t/*from*/, const char* /*buffer*/,     size_t/*length*/ )>     fn_insert_;
        std::function<void( size_t/*from*/, const wchar_t* /*wbuffer*/, size_t/*wlength*/ )>    fn_insertw_;

        std::function<void( size_t/*from*/, size_t/*length*/ )>                                 fn_erase_;
    };





    // other operators

    // + must be defined in derived classes

    // ==
    inline bool         operator==                  ( const base_buffer& b, const base_buffer& b2     ) { return b.is_equal( b2.data(),  b2.size()  ); }

    inline bool         operator==                  ( const base_buffer& b, const char    c           ) { return b.is_equal( &c,         1          ); }
    inline bool         operator==                  ( const base_buffer& b, const char*   s           ) { return b.is_equal( s,          strlen( s )); }
    inline bool         operator==                  ( const base_buffer& b, const std::string&  s     ) { return b.is_equal( s.c_str(),  s.size()   ); }
    inline bool         operator==                  ( const base_buffer& b, const std::string_view s  ) { return b.is_equal( s.data(),   s.size()   ); }
    inline bool         operator==                  ( const base_buffer& b, const std::vector<char>& v) { return b.is_equal( v.data(),   v.size()   ); }

    inline bool         operator==                  ( const base_buffer& b, const wchar_t c           ) { return b.is_equal( &c,        1           ); }
    inline bool         operator==                  ( const base_buffer& b, const wchar_t*s           ) { return b.is_equal( s,         wcslen( s ) ); }
    inline bool         operator==                  ( const base_buffer& b, const std::wstring& s     ) { return b.is_equal( s.c_str(), s.size()    ); }
    inline bool         operator==                  ( const base_buffer& b, const std::wstring_view s ) { return b.is_equal( s.data(),  s.size()    ); }
    inline bool         operator==                  ( const base_buffer& b, const std::vector<wchar_t>&v){return b.is_equal( v.data(),  v.size()    ); }

    // ==                                                                                                 
    inline bool         operator==                  ( const char c,             const base_buffer& b)   { return b.is_equal( &c,        1           ); }
    inline bool         operator==                  ( const char* s,            const base_buffer& b)   { return b.is_equal( s,         strlen( s ) ); }
    inline bool         operator==                  ( const std::string& s,     const base_buffer& b)   { return b.is_equal( s.c_str(), s.size()    ); }
    inline bool         operator==                  ( const std::string_view s, const base_buffer& b)   { return b.is_equal( s.data(),  s.size()    ); }
    inline bool         operator==                  ( const std::vector<char>&v,const base_buffer& b)   { return b.is_equal( v.data(),  v.size()    ); }
    
    inline bool         operator==                  ( const wchar_t c,          const base_buffer& b)   { return b.is_equal( &c,        1           ); }
    inline bool         operator==                  ( const wchar_t* s,         const base_buffer& b)   { return b.is_equal( s,         wcslen(s)   ); }
    inline bool         operator==                  ( const std::wstring& s,    const base_buffer& b)   { return b.is_equal( s.c_str(), s.size()    ); }
    inline bool         operator==                  ( const std::wstring_view s,const base_buffer& b)   { return b.is_equal( s.data(),  s.size()    ); }
    inline bool         operator==                  ( const std::vector<wchar_t>&v,const base_buffer& b){ return b.is_equal( v.data(),  v.size()    ); }


    // !=
    inline bool         operator!=                  ( const base_buffer& b, const base_buffer& b2     ) { return !b.is_equal( b2.data(),b2.size()   ); }

    inline bool         operator!=                  ( const base_buffer& b, const char    c           ) { return !b.is_equal( &c,       1           ); }
    inline bool         operator!=                  ( const base_buffer& b, const char*   s           ) { return !b.is_equal( s,        strlen(s)   ); }
    inline bool         operator!=                  ( const base_buffer& b, const std::string&  s     ) { return !b.is_equal( s.c_str(),s.size()    ); }
    inline bool         operator!=                  ( const base_buffer& b, const std::string_view s  ) { return !b.is_equal( s.data(), s.size()    ); }
    inline bool         operator!=                  ( const base_buffer& b, const std::vector<char>& v) { return !b.is_equal( v.data(), v.size()    ); }
    
    inline bool         operator!=                  ( const base_buffer& b, const wchar_t c           ) { return !b.is_equal( &c,       1           ); }
    inline bool         operator!=                  ( const base_buffer& b, const wchar_t*s           ) { return !b.is_equal( s,       wcslen( s )  ); }
    inline bool         operator!=                  ( const base_buffer& b, const std::wstring& s     ) { return !b.is_equal( s.c_str(),s.size()    ); }
    inline bool         operator!=                  ( const base_buffer& b, const std::wstring_view s ) { return !b.is_equal( s.data(), s.size()    ); }
    inline bool         operator!=                  ( const base_buffer& b, const std::vector<wchar_t>&v){return !b.is_equal( v.data(), v.size()    ); }

    // != 
    inline bool         operator!=                  ( const char c,             const base_buffer& b)   { return !b.is_equal( &c,       1           ); }
    inline bool         operator!=                  ( const char* s,            const base_buffer& b)   { return !b.is_equal( s,        strlen( s ) ); }
    inline bool         operator!=                  ( const std::string& s,     const base_buffer& b)   { return !b.is_equal( s.c_str(),s.size()    ); }
    inline bool         operator!=                  ( const std::string_view s, const base_buffer& b)   { return !b.is_equal( s.data(), s.size()    ); }
    inline bool         operator!=                  ( const std::vector<char>&v,const base_buffer& b)   { return !b.is_equal( v.data(), v.size()    ); }

    inline bool         operator!=                  ( const wchar_t c,          const base_buffer& b)   { return !b.is_equal( &c,       1           ); }
    inline bool         operator!=                  ( const wchar_t* s,         const base_buffer& b)   { return !b.is_equal( s,        wcslen( s ) ); }
    inline bool         operator!=                  ( const std::wstring& s,    const base_buffer& b)   { return !b.is_equal( s.c_str(),s.size()    ); }
    inline bool         operator!=                  ( const std::wstring_view s, const base_buffer& b)  { return !b.is_equal( s.data(), s.size()    ); }
    inline bool         operator!=                  ( const std::vector<wchar_t>&v,const base_buffer& b){ return !b.is_equal( v.data(), v.size()    ); }


     // < 
    inline bool         operator<                   ( const base_buffer& b, const base_buffer& b2       ) { return b.compare( b2.data(),b2.size()   ) < 0; }

    inline bool         operator<                   ( const base_buffer& b, const char c                ) { return b.compare( &c,       1           ) < 0; }
    inline bool         operator<                   ( const base_buffer& b, const char* s               ) { return b.compare( s,        strlen( s ) ) < 0; }
    inline bool         operator<                   ( const base_buffer& b, const std::string&  s       ) { return b.compare( s.c_str(),s.size()    ) < 0; }
    inline bool         operator<                   ( const base_buffer& b, const std::string_view s    ) { return b.compare( s.data(), s.size()    ) < 0; }
    inline bool         operator<                   ( const base_buffer& b, const std::vector<char>& v  ) { return b.compare( v.data(), v.size()    ) < 0; }

    inline bool         operator<                   ( const base_buffer& b, const wchar_t c             ) { return b.compare( &c,       1           ) < 0; }
    inline bool         operator<                   ( const base_buffer& b, const wchar_t* s            ) { return b.compare( s,        wcslen( s ) ) < 0; }
    inline bool         operator<                   ( const base_buffer& b, const std::wstring& s       ) { return b.compare( s.c_str(),s.size()    ) < 0; }
    inline bool         operator<                   ( const base_buffer& b, const std::wstring_view s   ) { return b.compare( s.data(), s.size()    ) < 0; }
    inline bool         operator<                   ( const base_buffer& b, const std::vector<wchar_t>&v) { return b.compare( v.data(), v.size()    ) < 0; }
                                        
    // <
    inline bool         operator<                   ( const char c,             const base_buffer& b    ) { return b.compare( &c,       1           ) >= 0; }
    inline bool         operator<                   ( const char* s,            const base_buffer& b    ) { return b.compare( s,        strlen( s ) ) >= 0; }
    inline bool         operator<                   ( const std::string& s,     const base_buffer& b    ) { return b.compare( s.c_str(),s.size()    ) >= 0; }
    inline bool         operator<                   ( const std::string_view s, const base_buffer& b    ) { return b.compare( s.data(), s.size()    ) >= 0; }
    inline bool         operator<                   ( const std::vector<char>&v,const base_buffer& b    ) { return b.compare( v.data(), v.size()    ) >= 0; }
    
    inline bool         operator<                   ( const wchar_t c,              const base_buffer& b) { return b.compare( &c,       1           ) >= 0; }
    inline bool         operator<                   ( const wchar_t* s,             const base_buffer& b) { return b.compare( s,        wcslen(s)   ) >= 0; }
    inline bool         operator<                   ( const std::wstring& s,        const base_buffer& b) { return b.compare( s.c_str(),s.size()    ) >= 0; }
    inline bool         operator<                   ( const std::wstring_view s,    const base_buffer& b) { return b.compare( s.data(), s.size()    ) >= 0; }
    inline bool         operator<                   ( const std::vector<wchar_t>&v, const base_buffer& b) { return b.compare( v.data(), v.size()    ) >= 0; }


    // <= 
    inline bool         operator<=                  ( const base_buffer& b, const base_buffer& b2       ) { return b.compare( b2.data(),b2.size()   ) <= 0; }

    inline bool         operator<=                  ( const base_buffer& b, const char c                ) { return b.compare( &c,       1           ) <= 0; }
    inline bool         operator<=                  ( const base_buffer& b, const char* s               ) { return b.compare( s,        strlen( s ) ) <= 0; }
    inline bool         operator<=                  ( const base_buffer& b, const std::string& s        ) { return b.compare( s.c_str(),s.size()    ) <= 0; }
    inline bool         operator<=                  ( const base_buffer& b, const std::string_view s    ) { return b.compare( s.data(), s.size()    ) <= 0; }
    inline bool         operator<=                  ( const base_buffer& b, const std::vector<char>& v  ) { return b.compare( v.data(), v.size()    ) <= 0; }

    inline bool         operator<=                  ( const base_buffer& b, const wchar_t c             ) { return b.compare( &c,       1           ) <= 0; }
    inline bool         operator<=                  ( const base_buffer& b, const wchar_t* s            ) { return b.compare( s,        wcslen( s ) ) <= 0; }
    inline bool         operator<=                  ( const base_buffer& b, const std::wstring& s       ) { return b.compare( s.c_str(),s.size()    ) <= 0; }
    inline bool         operator<=                  ( const base_buffer& b, const std::wstring_view s   ) { return b.compare( s.data(), s.size()    ) <= 0; }
    inline bool         operator<=                  ( const base_buffer& b, const std::vector<wchar_t>&v) { return b.compare( v.data(), v.size()    ) <= 0; }
    
    // <= 
    inline bool         operator<=                  ( const char c,                 const base_buffer& b) { return b.compare( &c,       1           ) > 0; }
    inline bool         operator<=                  ( const char* s,                const base_buffer& b) { return b.compare( s,        strlen( s ) ) > 0; }
    inline bool         operator<=                  ( const std::string& s,         const base_buffer& b) { return b.compare( s.c_str(),s.size()    ) > 0; }
    inline bool         operator<=                  ( const std::string_view s,     const base_buffer& b) { return b.compare( s.data(), s.size()    ) > 0; }
    inline bool         operator<=                  ( const std::vector<char>& v,   const base_buffer& b) { return b.compare( v.data(), v.size()    ) > 0; }

    inline bool         operator<=                  ( const wchar_t c,              const base_buffer& b) { return b.compare( &c,       1           ) > 0; }
    inline bool         operator<=                  ( const wchar_t* s,             const base_buffer& b) { return b.compare( s,        wcslen( s ) ) > 0; }
    inline bool         operator<=                  ( const std::wstring& s,        const base_buffer& b) { return b.compare( s.c_str(),s.size()    ) > 0; }
    inline bool         operator<=                  ( const std::wstring_view s,    const base_buffer& b) { return b.compare( s.data(), s.size()    ) > 0; }
    inline bool         operator<=                  ( const std::vector<wchar_t>& v,const base_buffer& b) { return b.compare( v.data(), v.size()    ) > 0; }


    // >
    inline bool         operator>                   ( const base_buffer& b, const base_buffer& b2       ) { return b.compare( b2.data(),b2.size()   ) > 0; }

    inline bool         operator>                   ( const base_buffer& b, const char c                ) { return b.compare( &c,       1           ) > 0; }
    inline bool         operator>                   ( const base_buffer& b, const char* s               ) { return b.compare( s,        strlen( s ) ) > 0; }
    inline bool         operator>                   ( const base_buffer& b, const std::string& s        ) { return b.compare( s.c_str(),s.size()    ) > 0; }
    inline bool         operator>                   ( const base_buffer& b, const std::string_view s    ) { return b.compare( s.data(), s.size()    ) > 0; }
    inline bool         operator>                   ( const base_buffer& b, const std::vector<char>& v  ) { return b.compare( v.data(), v.size()    ) > 0; }

    inline bool         operator>                   ( const base_buffer& b, const wchar_t c             ) { return b.compare( &c,       1           ) > 0; }
    inline bool         operator>                   ( const base_buffer& b, const wchar_t* s            ) { return b.compare( s,        wcslen( s ) ) > 0; }
    inline bool         operator>                   ( const base_buffer& b, const std::wstring& s       ) { return b.compare( s.c_str(),s.size()    ) > 0; }
    inline bool         operator>                   ( const base_buffer& b, const std::wstring_view s   ) { return b.compare( s.data(), s.size()    ) > 0; }
    inline bool         operator>                   ( const base_buffer& b, const std::vector<wchar_t>&v) { return b.compare( v.data(), v.size()    ) > 0; }

    
    // >
    inline bool         operator>                   ( const char c,                 const base_buffer& b) { return b.compare( &c,       1           ) <= 0; }
    inline bool         operator>                   ( const char* s,                const base_buffer& b) { return b.compare( s,        strlen( s ) ) <= 0; }
    inline bool         operator>                   ( const std::string& s,         const base_buffer& b) { return b.compare( s.c_str(),s.size()    ) <= 0; }
    inline bool         operator>                   ( const std::string_view s,     const base_buffer& b) { return b.compare( s.data(), s.size()    ) <= 0; }
    inline bool         operator>                   ( const std::vector<char>& v,   const base_buffer& b) { return b.compare( v.data(), v.size()    ) <= 0; }

    inline bool         operator>                   ( const wchar_t c,              const base_buffer& b) { return b.compare( &c,       1           ) <= 0; }
    inline bool         operator>                   ( const wchar_t* s,             const base_buffer& b) { return b.compare( s,        wcslen( s ) ) <= 0; }
    inline bool         operator>                   ( const std::wstring& s,        const base_buffer& b) { return b.compare( s.c_str(),s.size()    ) <= 0; }
    inline bool         operator>                   ( const std::wstring_view s,    const base_buffer& b) { return b.compare( s.data(), s.size()    ) <= 0; }
    inline bool         operator>                   ( const std::vector<wchar_t>& v,const base_buffer& b) { return b.compare( v.data(), v.size()    ) <= 0; }


    // >= 
    inline bool         operator>=                  ( const base_buffer& b, const base_buffer& b2       ) { return b.compare( b2.data(),b2.size()   ) >= 0; }

    inline bool         operator>=                  ( const base_buffer& b, const char c                ) { return b.compare( &c,       1           ) >= 0; }
    inline bool         operator>=                  ( const base_buffer& b, const char* s               ) { return b.compare( s,        strlen( s ) ) >= 0; }
    inline bool         operator>=                  ( const base_buffer& b, const std::string& s        ) { return b.compare( s.c_str(),s.size()    ) >= 0; }
    inline bool         operator>=                  ( const base_buffer& b, const std::string_view s    ) { return b.compare( s.data(), s.size()    ) >= 0; }
    inline bool         operator>=                  ( const base_buffer& b, const std::vector<char>& v  ) { return b.compare( v.data(), v.size()    ) >= 0; }

    inline bool         operator>=                  ( const base_buffer& b, const wchar_t c             ) { return b.compare( &c,       1           ) >= 0; }
    inline bool         operator>=                  ( const base_buffer& b, const wchar_t* s            ) { return b.compare( s,        wcslen( s ) ) >= 0; }
    inline bool         operator>=                  ( const base_buffer& b, const std::wstring& s       ) { return b.compare( s.c_str(),s.size()    ) >= 0; }
    inline bool         operator>=                  ( const base_buffer& b, const std::wstring_view s   ) { return b.compare( s.data(), s.size()    ) >= 0; }
    inline bool         operator>=                  ( const base_buffer& b, const std::vector<wchar_t>&v) { return b.compare( v.data(), v.size()    ) >= 0; }
    
    // >= 
    inline bool         operator>=                  ( const char c,                 const base_buffer& b) { return b.compare( &c,       1           ) < 0; }
    inline bool         operator>=                  ( const char* s,                const base_buffer& b) { return b.compare( s,        strlen( s ) ) < 0; }
    inline bool         operator>=                  ( const std::string& s,         const base_buffer& b) { return b.compare( s.c_str(),s.size()    ) < 0; }
    inline bool         operator>=                  ( const std::string_view s,     const base_buffer& b) { return b.compare( s.data(), s.size()    ) < 0; }
    inline bool         operator>=                  ( const std::vector<char>& v,   const base_buffer& b) { return b.compare( v.data(), v.size()    ) < 0; }

    inline bool         operator>=                  ( const wchar_t c,              const base_buffer& b) { return b.compare( &c,       1           ) < 0; }
    inline bool         operator>=                  ( const wchar_t* s,             const base_buffer& b) { return b.compare( s,        wcslen( s ) ) < 0; }
    inline bool         operator>=                  ( const std::wstring& s,        const base_buffer& b) { return b.compare( s.c_str(),s.size()    ) < 0; }
    inline bool         operator>=                  ( const std::wstring_view s,    const base_buffer& b) { return b.compare( s.data(), s.size()    ) < 0; }
    inline bool         operator>=                  ( const std::vector<wchar_t>& v,const base_buffer& b) { return b.compare( v.data(), v.size()    ) < 0; }


} // namespace small



#endif // SMALL_IMPL_BASE_base_buffer
