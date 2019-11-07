#pragma once

#include <string.h>
#include <stddef.h>

#include <type_traits>
#include <string>
#include <string_view>
#include <vector>

// 
// small::buffer b;
// b.clear();
// 
// b.set( "ana", 3 );
// b.set( "b", 1, 2 );
// 
// char* e = b.extract(); // extract "anb"
// free( e );
//
// small::buffer b1 = { 8192/*chunksize*/, "buffer", 6/*length*/ };
// small::buffer b2 = { 8192/*chunksize*/, "buffer" };
// small::buffer b3 = "buffer";
// small::buffer b4 = std::string( "buffer" );
//
// auto d1 = b2.c_string();
// auto v1 = b2.c_vector();
// 
// b.append( "hello", 5 );
// b.clear( true );
// 
// char* e1 = b.extract(); // extract ""
// free( e1 );
// 
// b.append( "world", 5 );
// b.clear();
//
// std::string s64 = small::tobase64_s( "hello world", 11 );
// b.clear();
// small::frombase64( s64.c_str(), (int)s64.size(), &b );
// b = small::frombase64_b( s64 );
//
namespace small
{
    const size_t default_buffer_chunk_size = 4096;

    // class for representing a buffer
    class buffer
    {
    public:
        // buffer (allocates in chunks)
        buffer                                      ( const size_t& chunk_size = default_buffer_chunk_size ) { init( chunk_size ); }
        // from buffer
        buffer                                      ( const buffer& o ) noexcept                    { init( o.chunk_size_ ); operator=( o ); }
        buffer                                      ( buffer&&      o ) noexcept                    { init( o.chunk_size_ ); operator=( std::forward<buffer>( o ) ); }
        // from char*
        buffer                                      ( const char *  s )                             { init( default_buffer_chunk_size ); operator=( s ); }
        buffer                                      ( const size_t& chunk_size, const char * s)     { init( chunk_size ); operator=( s ); }
        buffer                                      ( const size_t& chunk_size, const char * s, const size_t& s_length) { init( chunk_size ); set( s, s_length ); }
        // from wchar_t*
        buffer                                      ( const wchar_t *  s )                          { init( default_buffer_chunk_size ); operator=( s ); }
        buffer                                      ( const size_t& chunk_size, const wchar_t* s)   { init( chunk_size ); operator=( s ); }
        buffer                                      ( const size_t& chunk_size, const wchar_t* s, const size_t& s_length) { init( chunk_size ); set( (const char*)s, sizeof(wchar_t) * s_length ); }
        // from char
        buffer                                      ( const char c    )                             { init( default_buffer_chunk_size ); operator=( c ); }
        buffer                                      ( const size_t& chunk_size, const char c  )     { init( chunk_size ); operator=( c ); }
        // from wchar_t
        buffer                                      ( const wchar_t c    )                          { init( default_buffer_chunk_size ); operator=( c ); }
        buffer                                      ( const size_t& chunk_size, const wchar_t c  )  { init( chunk_size ); operator=( c ); }
        // from std::string
        buffer                                      ( const std::string& s )                        { init( default_buffer_chunk_size ); operator=( s ); }
        buffer                                      ( const size_t& chunk_size, const std::string& s) { init( chunk_size ); operator=( s ); }
        // from std::wstring
        buffer                                      ( const std::wstring& s )                       { init( default_buffer_chunk_size ); operator=( s ); }
        buffer                                      ( const size_t& chunk_size, const std::wstring& s) { init( chunk_size ); operator=( s ); }
        // from std::vector<char>
        buffer                                      ( const std::vector<char>& v )                  { init( default_buffer_chunk_size ); operator=( v ); }
        buffer                                      ( const size_t& chunk_size, const std::vector<char>& v) { init( chunk_size ); operator=( v ); }

        // destructor
        ~buffer                                     () { free_buffer(); }
    

         // size / empty / clear
        inline size_t   size                        () const { return buffer_length_;  }
        inline size_t   length                      () const { return buffer_length_;  }
        inline bool     empty                       () const { return buffer_length_ == 0; }
        inline void     clear                       ( const bool & __free_buffer = false ) { buffer_length_ = 0; if ( __free_buffer ) { free_buffer(); } buffer_[0] = '\0'; }
       

        // reserve / resize
        inline void     reserve                     ( const size_t new_size, const bool & shrink = false ) { ensure_size( new_size, shrink ); }
        inline void     resize                      ( const size_t new_size, const bool & shrink = false ) { buffer_length_ = ensure_size( new_size, shrink ); }
        inline void     shrink_to_fit               ()                                                     { buffer_length_ = ensure_size( size(), true/*shrink*/); }



        // chunk size
        inline size_t   get_chunk_size              () const { return chunk_size_; }
        inline void     set_chunk_size              ( const size_t & chunk_size ){ chunk_size_ = chunk_size; }

        // data buffer
        inline const char* get_buffer               () const { return buffer_; }
        inline const char* c_str                    () const { return buffer_; }
        inline const char* data                     () const { return buffer_; }
        inline char*    get_buffer                  () { return buffer_; }
        inline char*    data                        () { return buffer_; }

                // as c_string
        inline std::string c_string                 () const { return std::string( data(), size() ); }
        inline std::vector<char> c_vector           () const { std::vector<char> v( size() ); memcpy( v.data(), data(), size() ); return v; }
        inline std::string_view c_view              () const { return std::string_view{ data(), size() }; }



        // extract buffer - be sure to call free after you use it
        inline char*    extract                     () { char* b =  nullptr; if ( buffer_ == empty_buffer_ ) { b = (char*) malloc(sizeof(char)); if (b) { *b='\0'; }  } else { b = buffer_; init( chunk_size_ ); } return b; }


        // append
        inline void     append                      ( const buffer& b           ) { set( b.data(),              b.size(),       size()/*startfrom*/ ); }
        inline void     append                      ( const char* s, const size_t& s_length ) { set( s,         s_length,       size()/*startfrom*/ ); }
        inline void     append                      ( const char* s             ) { set( s,                     strlen(s),      size()/*startfrom*/ ); }
        inline void     append                      ( const char  c             ) { set( &c,                    sizeof(c),      size()/*startfrom*/ ); }
        inline void     append                      ( const wchar_t* s          ) { set( (const char*)s,        sizeof(wchar_t) * wcslen(s), size()/*startfrom*/ ); }
        inline void     append                      ( const wchar_t* s, const size_t& s_length ){ set( (const char*)s,  sizeof(wchar_t) * s_length, size()/*startfrom*/ ); }
        inline void     append                      ( const wchar_t  c          ) { set( (const char*)&c,       sizeof(c),      size()/*startfrom*/ ); }
        inline void     append                      ( const std::string& s      ) { set( s.c_str(),             s.size(),       size()/*startfrom*/ ); }
        inline void     append                      ( const std::wstring& s     ) { set( (const char*)s.c_str(),sizeof(wchar_t) * s.size(), size()/*startfrom*/ ); }
        inline void     append                      ( const std::vector<char>& v) { set( v.data(),              v.size(),       size()/*startfrom*/ ); }

        // assign
        inline void     assign                      ( const buffer& b           ) { if ( this != &b ) { set( b.data(), b.size(), 0/*startfrom*/ ); } }
        inline void     assign                      ( const char* s, const size_t& s_length ) { set( s,         s_length,       0/*startfrom*/ ); }
        inline void     assign                      ( const char* s             ) { set( s,                     strlen(s),      0/*startfrom*/ ); }
        inline void     assign                      ( const char  c             ) { set( &c,                    sizeof(c),      0/*startfrom*/ ); }
        inline void     assign                      ( const wchar_t* s          ) { set( (const char*)s,        sizeof(wchar_t) * wcslen(s), 0/*startfrom*/ ); }
        inline void     assign                      ( const wchar_t* s, const size_t& s_length ){ set( (const char*)s,  sizeof(wchar_t) * s_length, 0/*startfrom*/ ); }
        inline void     assign                      ( const wchar_t  c          ) { set( (const char*)&c,       sizeof(c),      0/*startfrom*/ ); }
        inline void     assign                      ( const std::string& s      ) { set( s.c_str(),             s.size(),       0/*startfrom*/ ); }
        inline void     assign                      ( const std::wstring& s     ) { set( (const char*)s.c_str(),sizeof(wchar_t) * s.size(), 0/*startfrom*/ ); }
        inline void     assign                      ( const std::vector<char>& v) { set( v.data(),              v.size(),       0/*startfrom*/ ); }

        // set
        inline void     set                         ( const char* s, const size_t& s_length, const size_t& start_from = 0 ) { set_impl( s, s_length, start_from ); }
        inline void     overwrite                   ( const char* s, const size_t& s_length, const size_t& start_from = 0 ) { set( s, s_length, start_from ); }

        // insert
        inline void     insert                      ( const char* b, const size_t& b_length, const size_t& insert_from = 0 ) { insert_impl( b, b_length, insert_from ); }
        // erase
        inline void     erase                       ( const size_t& start_from ) { if ( start_from < size() ) { resize( start_from ); } }
        inline void     erase                       ( const size_t& start_from, const size_t& length ) { erase_impl( start_from, length ); }
        
        // compare
        inline bool     is_equal                    ( const char *s, const size_t& s_length ) const { return size() == s_length && compare( s, s_length ) == 0; }
        inline int      compare                     ( const char *s, const size_t& s_length ) const { return memcmp( data(), s, size() < s_length ? size()+1 : s_length+1 ); }

        // swap
        inline void     swap                        ( buffer & o ) { swap_impl( o ); }


        // operators

        // =
        inline buffer&  operator=                   ( const buffer& o           ) noexcept { if ( this != &o ) { chunk_size_ = o.chunk_size_; reserve( o.size(), true/*shrink*/ ); set( o.data(), o.size() ); } return *this; }
        inline buffer&  operator=                   ( buffer&&      o           ) noexcept { if ( this != &o ) { clear( true ); memcpy( this, &o, sizeof( *this ) ); o.init( this->chunk_size_ ); } return *this; }
        inline buffer&  operator=                   ( const char*   s           ) noexcept { set( s,                        strlen(s)                   ); return *this; }
        inline buffer&  operator=                   ( const char    c           ) noexcept { set( &c,                       sizeof(char)                ); return *this; }
        inline buffer&  operator=                   ( const wchar_t*s           ) noexcept { set( (const char *)s,          sizeof(wchar_t) * wcslen(s) ); return *this; }
        inline buffer&  operator=                   ( const wchar_t c           ) noexcept { set( (const char *)&c,         sizeof(wchar_t)             ); return *this; }
        inline buffer&  operator=                   ( const std::string&  s     ) noexcept { set( s.c_str(),                s.size()                    ); return *this; }
        inline buffer&  operator=                   ( const std::wstring& s     ) noexcept { set( (const char*)s.c_str(),   sizeof(wchar_t) * s.size()  ); return *this; }
        inline buffer&  operator=                   ( const std::vector<char>& v) noexcept { set( v.data(),                 v.size()                    ); return *this; }
        
        // +=
        inline buffer&  operator+=                  ( const buffer& b           ) noexcept { append( b.data(),              b.size()                    ); return *this; }
        inline buffer&  operator+=                  ( const char*   s           ) noexcept { append( s,                     strlen(s)                   ); return *this; }
        inline buffer&  operator+=                  ( const char    c           ) noexcept { append( &c,                    sizeof(char)                ); return *this; }
        inline buffer&  operator+=                  ( const wchar_t*s           ) noexcept { append( (const char *)s,       sizeof(wchar_t) * wcslen(s) ); return *this; }
        inline buffer&  operator+=                  ( const wchar_t c           ) noexcept { append( (const char *)&c,      sizeof(wchar_t)             ); return *this; }
        inline buffer&  operator+=                  ( const std::string&  s     ) noexcept { append( s.c_str(),             s.size()                    ); return *this; }
        inline buffer&  operator+=                  ( const std::wstring& s     ) noexcept { append( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ); return *this; }
        inline buffer&  operator+=                  ( const std::vector<char>& v) noexcept { append( v.data(),              v.size()                    ); return *this; }
        

        // []
        inline char&    operator[]                  ( const size_t& index       ) { return buffer_[ index ]; }
        inline const char operator[]                ( const size_t& index       ) const { return buffer_[ index ]; }

        inline char&    at                          ( const size_t& index       ) { return buffer_[ index ]; }
        inline const char at                        ( const size_t& index       ) const { return buffer_[ index ]; }

        inline char&    front                       () { return buffer_[ 0 ]; }
        inline const char front                     () const { return buffer_[ 0 ]; }
        inline char&    back                        () { return size() > 0 ? buffer_[ size()-1 ] : buffer_[0]; }
        inline const char back                      () const { return size() > 0 ? buffer_[size() - 1] : buffer_[0]; }

        inline void     push_back                   ( const char c ) { append( c ); }
        inline void     pop_back                    () { if ( size() > 0 ) { resize( size() - 1 ); } }


        // operator
        inline          operator std::string_view   () { return c_view(); }


    private:
        // init
        inline void     init                        ( const size_t chunk_size ) { memset( this, 0, sizeof( *this ) ); chunk_size_ = chunk_size; buffer_ = empty_buffer_; }
        // free_buffer
        inline void     free_buffer                 () { buffer_length_ = 0; buffer_alloc_size_ = 0; if ( buffer_ && (buffer_ != empty_buffer_) )  { free( buffer_ ); buffer_ = empty_buffer_; }  }

        // ensure size returns new_length
        inline size_t   ensure_size                 ( const size_t& new_size, const bool& shrink = false )
        {
            // we always append a '\0' to the end so we can use as string
            size_t new_alloc_size = ((new_size + sizeof(char)/*for '\0'*/ + (chunk_size_ - 1)) / chunk_size_) * chunk_size_;
            bool reallocate = false;
            if ( shrink )
            {
                reallocate = (buffer_alloc_size_ == 0) || (new_alloc_size != buffer_alloc_size_); // we need another size
            }
            else
            {
                reallocate = (buffer_alloc_size_ == 0) || (new_alloc_size > buffer_alloc_size_); // we need bigger size
            }
            
            // (re)allocate
            if ( reallocate )
            {
                buffer_             = (buffer_alloc_size_ == 0) ? (char*)malloc( new_alloc_size ) : (char*)realloc( buffer_, new_alloc_size );
                buffer_alloc_size_  = new_alloc_size;
            }
            
            // failure
            if ( buffer_ == nullptr )
            {
                init( chunk_size_ );
                return 0;
            }
            // return new_length of the buffer
            buffer_[new_size] = '\0';
            return new_size;
        }




        // set impl
        inline void     set_impl                    ( const char* b, const size_t& b_length, const size_t& start_from = 0 ) 
        { 
            resize( start_from + b_length, false ); 
            if ( b && buffer_ != empty_buffer_ /*good allocation*/ ) 
            { 
                memcpy( buffer_ + start_from, b, b_length );
            } 
        }


        // insert impl
        inline void     insert_impl                 ( const char* b, const size_t& b_length, const size_t& insert_from = 0 ) 
        { 
            size_t initial_length = size();
            resize( insert_from <= initial_length ? initial_length + b_length : insert_from + b_length, false );

            if ( b && buffer_ != empty_buffer_ /*good allocation*/ ) 
            { 
                if ( insert_from <= initial_length )
                    memmove( buffer_ + insert_from + b_length, buffer_ + insert_from, initial_length-insert_from );
                else
                    memset( buffer_ + initial_length, '\0', (insert_from-initial_length) );
                memcpy( buffer_ + insert_from, b, b_length ); 
            } 
        }


        // erase
        inline void     erase_impl                       ( const size_t& start_from, const size_t& length ) 
        { 
            if ( buffer_ != empty_buffer_ )
            {
                if ( start_from < size() )
                {
                    if ( start_from + length < size() )
                    {
                        size_t move_length = size() - (start_from + length);
                        memmove( buffer_ + start_from, buffer_ + start_from + length, move_length );
                        resize( size() - length );
                    }
                    else
                    {
                        resize( start_from );
                    }
                }
            }
        }


        // swap
        inline void     swap_impl                   ( buffer& o )
        {
            std::swap( chunk_size_,         o.chunk_size_           ); 
            std::swap( buffer_length_,      o.buffer_length_        ); 
            std::swap( buffer_alloc_size_,  o.buffer_alloc_size_    );
            // swap buffer has 4 cases
            if ( buffer_ != empty_buffer_ && o.buffer_ != o.empty_buffer_ )      { std::swap( buffer_, o.buffer_ ); }
            else if ( buffer_ == empty_buffer_ && o.buffer_ == o.empty_buffer_ ) { /*no nothing*/ }
            else if ( buffer_ != empty_buffer_ && o.buffer_ == o.empty_buffer_ ) { o.buffer_ = buffer_;    buffer_   = empty_buffer_; }
            else if ( buffer_ == empty_buffer_ && o.buffer_ != o.empty_buffer_ ) { buffer_   = o.buffer_;  o.buffer_ = o.empty_buffer_; }
        }


    private:
        // chunk size
        size_t          chunk_size_;
        // buffer empty
        char            empty_buffer_[1];
        // buffer use char* instead of vector<char> because it is much faster
        char *          buffer_;
        size_t          buffer_length_;
        size_t          buffer_alloc_size_;
    };





    // other operators

    // +
    inline buffer       operator+                   ( const buffer& b, const buffer& b2          ) { buffer br = b; br.append( b2.data(),             b2.size()                   ); return br; }
    inline buffer       operator+                   ( buffer&& b,      const buffer& b2          ) { buffer br( std::forward<buffer>( b ) ); br.append( b2.data(),             b2.size()                   ); return br; }
    inline buffer       operator+                   ( const buffer& b, const char*   s           ) { buffer br = b; br.append( s,                     strlen(s)                   ); return br; }
    inline buffer       operator+                   ( const buffer& b, const char    c           ) { buffer br = b; br.append( &c,                    sizeof(char)                ); return br; }
    inline buffer       operator+                   ( const buffer& b, const wchar_t*s           ) { buffer br = b; br.append( (const char *)s,       sizeof(wchar_t) * wcslen(s) ); return br; }
    inline buffer       operator+                   ( const buffer& b, const wchar_t c           ) { buffer br = b; br.append( (const char *)&c,      sizeof(wchar_t)             ); return br; }
    inline buffer       operator+                   ( const buffer& b, const std::string&  s     ) { buffer br = b; br.append( s.c_str(),             s.size()                    ); return br; }
    inline buffer       operator+                   ( const buffer& b, const std::wstring& s     ) { buffer br = b; br.append( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ); return br; }
    inline buffer       operator+                   ( const buffer& b, const std::vector<char>& v) { buffer br = b; br.append( v.data(),              v.size()                    ); return br; }

    inline buffer       operator+                   ( const char*   s           , const buffer& b) { buffer br ( b.get_chunk_size() ); br.append( s,                     strlen(s)                   ); br += b; return br; }
    inline buffer       operator+                   ( const char    c           , const buffer& b) { buffer br ( b.get_chunk_size() ); br.append( &c,                    sizeof(char)                ); br += b; return br; }
    inline buffer       operator+                   ( const wchar_t*s           , const buffer& b) { buffer br ( b.get_chunk_size() ); br.append( (const char *)s,       sizeof(wchar_t) * wcslen(s) ); br += b; return br; }
    inline buffer       operator+                   ( const wchar_t c           , const buffer& b) { buffer br ( b.get_chunk_size() ); br.append( (const char *)&c,      sizeof(wchar_t)             ); br += b; return br; }
    inline buffer       operator+                   ( const std::string&  s     , const buffer& b) { buffer br ( b.get_chunk_size() ); br.append( s.c_str(),             s.size()                    ); br += b; return br; }
    inline buffer       operator+                   ( const std::wstring& s     , const buffer& b) { buffer br ( b.get_chunk_size() ); br.append( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ); br += b; return br; }
    inline buffer       operator+                   ( const std::vector<char>& v, const buffer& b) { buffer br ( b.get_chunk_size() ); br.append( v.data(),              v.size()                    ); br += b; return br; }

    // ==
    inline bool         operator==                  ( const buffer& b, const buffer& b2          ) { return b.is_equal( b2.data(),             b2.size()                   ); }
    inline bool         operator==                  ( const buffer& b, const char*   s           ) { return b.is_equal( s,                     strlen(s)                   ); }
    inline bool         operator==                  ( const buffer& b, const char    c           ) { return b.is_equal( &c,                    sizeof(char)                ); }
    inline bool         operator==                  ( const buffer& b, const wchar_t*s           ) { return b.is_equal( (const char *)s,       sizeof(wchar_t) * wcslen(s) ); }
    inline bool         operator==                  ( const buffer& b, const wchar_t c           ) { return b.is_equal( (const char *)&c,      sizeof(wchar_t)             ); }
    inline bool         operator==                  ( const buffer& b, const std::string&  s     ) { return b.is_equal( s.c_str(),             s.size()                    ); }
    inline bool         operator==                  ( const buffer& b, const std::wstring& s     ) { return b.is_equal( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ); }
    inline bool         operator==                  ( const buffer& b, const std::vector<char>& v) { return b.is_equal( v.data(),              v.size()                    ); }
                                                                                                              
    inline bool         operator==                  ( const char*   s           , const buffer& b) { return b.is_equal( s,                     strlen(s)                   ); }
    inline bool         operator==                  ( const char    c           , const buffer& b) { return b.is_equal( &c,                    sizeof(char)                ); }
    inline bool         operator==                  ( const wchar_t*s           , const buffer& b) { return b.is_equal( (const char *)s,       sizeof(wchar_t) * wcslen(s) ); }
    inline bool         operator==                  ( const wchar_t c           , const buffer& b) { return b.is_equal( (const char *)&c,      sizeof(wchar_t)             ); }
    inline bool         operator==                  ( const std::string&  s     , const buffer& b) { return b.is_equal( s.c_str(),             s.size()                    ); }
    inline bool         operator==                  ( const std::wstring& s     , const buffer& b) { return b.is_equal( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ); }
    inline bool         operator==                  ( const std::vector<char>& v, const buffer& b) { return b.is_equal( v.data(),              v.size()                    ); }

    // !=
    inline bool         operator!=                  ( const buffer& b, const buffer& b2          ) { return !b.is_equal( b2.data(),             b2.size()                   ); }
    inline bool         operator!=                  ( const buffer& b, const char*   s           ) { return !b.is_equal( s,                     strlen(s)                   ); }
    inline bool         operator!=                  ( const buffer& b, const char    c           ) { return !b.is_equal( &c,                    sizeof(char)                ); }
    inline bool         operator!=                  ( const buffer& b, const wchar_t*s           ) { return !b.is_equal( (const char *)s,       sizeof(wchar_t) * wcslen(s) ); }
    inline bool         operator!=                  ( const buffer& b, const wchar_t c           ) { return !b.is_equal( (const char *)&c,      sizeof(wchar_t)             ); }
    inline bool         operator!=                  ( const buffer& b, const std::string&  s     ) { return !b.is_equal( s.c_str(),             s.size()                    ); }
    inline bool         operator!=                  ( const buffer& b, const std::wstring& s     ) { return !b.is_equal( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ); }
    inline bool         operator!=                  ( const buffer& b, const std::vector<char>& v) { return !b.is_equal( v.data(),              v.size()                    ); }
                                                                                                     
    inline bool         operator!=                  ( const char*   s           , const buffer& b) { return !b.is_equal( s,                     strlen(s)                   ); }
    inline bool         operator!=                  ( const char    c           , const buffer& b) { return !b.is_equal( &c,                    sizeof(char)                ); }
    inline bool         operator!=                  ( const wchar_t*s           , const buffer& b) { return !b.is_equal( (const char *)s,       sizeof(wchar_t) * wcslen(s) ); }
    inline bool         operator!=                  ( const wchar_t c           , const buffer& b) { return !b.is_equal( (const char *)&c,      sizeof(wchar_t)             ); }
    inline bool         operator!=                  ( const std::string&  s     , const buffer& b) { return !b.is_equal( s.c_str(),             s.size()                    ); }
    inline bool         operator!=                  ( const std::wstring& s     , const buffer& b) { return !b.is_equal( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ); }
    inline bool         operator!=                  ( const std::vector<char>& v, const buffer& b) { return !b.is_equal( v.data(),              v.size()                    ); }


     // < 
    inline bool         operator<                   ( const buffer& b, const buffer& b2          ) { return b.compare( b2.data(),             b2.size()                   ) < 0; }
    inline bool         operator<                   ( const buffer& b, const char*   s           ) { return b.compare( s,                     strlen(s)                   ) < 0; }
    inline bool         operator<                   ( const buffer& b, const char    c           ) { return b.compare( &c,                    sizeof(char)                ) < 0; }
    inline bool         operator<                   ( const buffer& b, const wchar_t*s           ) { return b.compare( (const char *)s,       sizeof(wchar_t) * wcslen(s) ) < 0; }
    inline bool         operator<                   ( const buffer& b, const wchar_t c           ) { return b.compare( (const char *)&c,      sizeof(wchar_t)             ) < 0; }
    inline bool         operator<                   ( const buffer& b, const std::string&  s     ) { return b.compare( s.c_str(),             s.size()                    ) < 0; }
    inline bool         operator<                   ( const buffer& b, const std::wstring& s     ) { return b.compare( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ) < 0; }
    inline bool         operator<                   ( const buffer& b, const std::vector<char>& v) { return b.compare( v.data(),              v.size()                    ) < 0; }
                                                                                                               
    inline bool         operator<                   ( const char*   s           , const buffer& b) { return b.compare( s,                     strlen(s)                   ) >= 0; }
    inline bool         operator<                   ( const char    c           , const buffer& b) { return b.compare( &c,                    sizeof(char)                ) >= 0; }
    inline bool         operator<                   ( const wchar_t*s           , const buffer& b) { return b.compare( (const char *)s,       sizeof(wchar_t) * wcslen(s) ) >= 0; }
    inline bool         operator<                   ( const wchar_t c           , const buffer& b) { return b.compare( (const char *)&c,      sizeof(wchar_t)             ) >= 0; }
    inline bool         operator<                   ( const std::string&  s     , const buffer& b) { return b.compare( s.c_str(),             s.size()                    ) >= 0; }
    inline bool         operator<                   ( const std::wstring& s     , const buffer& b) { return b.compare( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ) >= 0; }
    inline bool         operator<                   ( const std::vector<char>& v, const buffer& b) { return b.compare( v.data(),              v.size()                    ) >= 0; }

    // <= 
    inline bool         operator<=                  ( const buffer& b, const buffer& b2          ) { return b.compare( b2.data(),             b2.size()                   ) <= 0; }
    inline bool         operator<=                  ( const buffer& b, const char*   s           ) { return b.compare( s,                     strlen(s)                   ) <= 0; }
    inline bool         operator<=                  ( const buffer& b, const char    c           ) { return b.compare( &c,                    sizeof(char)                ) <= 0; }
    inline bool         operator<=                  ( const buffer& b, const wchar_t*s           ) { return b.compare( (const char *)s,       sizeof(wchar_t) * wcslen(s) ) <= 0; }
    inline bool         operator<=                  ( const buffer& b, const wchar_t c           ) { return b.compare( (const char *)&c,      sizeof(wchar_t)             ) <= 0; }
    inline bool         operator<=                  ( const buffer& b, const std::string&  s     ) { return b.compare( s.c_str(),             s.size()                    ) <= 0; }
    inline bool         operator<=                  ( const buffer& b, const std::wstring& s     ) { return b.compare( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ) <= 0; }
    inline bool         operator<=                  ( const buffer& b, const std::vector<char>& v) { return b.compare( v.data(),              v.size()                    ) <= 0; }
                                                                          
    inline bool         operator<=                  ( const char*   s           , const buffer& b) { return b.compare( s,                     strlen(s)                   ) > 0; }
    inline bool         operator<=                  ( const char    c           , const buffer& b) { return b.compare( &c,                    sizeof(char)                ) > 0; }
    inline bool         operator<=                  ( const wchar_t*s           , const buffer& b) { return b.compare( (const char *)s,       sizeof(wchar_t) * wcslen(s) ) > 0; }
    inline bool         operator<=                  ( const wchar_t c           , const buffer& b) { return b.compare( (const char *)&c,      sizeof(wchar_t)             ) > 0; }
    inline bool         operator<=                  ( const std::string&  s     , const buffer& b) { return b.compare( s.c_str(),             s.size()                    ) > 0; }
    inline bool         operator<=                  ( const std::wstring& s     , const buffer& b) { return b.compare( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ) > 0; }
    inline bool         operator<=                  ( const std::vector<char>& v, const buffer& b) { return b.compare( v.data(),              v.size()                    ) > 0; }


    // >
    inline bool         operator>                   ( const buffer& b, const buffer& b2          ) { return b.compare( b2.data(),             b2.size()                   ) > 0; }
    inline bool         operator>                   ( const buffer& b, const char*   s           ) { return b.compare( s,                     strlen(s)                   ) > 0; }
    inline bool         operator>                   ( const buffer& b, const char    c           ) { return b.compare( &c,                    sizeof(char)                ) > 0; }
    inline bool         operator>                   ( const buffer& b, const wchar_t*s           ) { return b.compare( (const char *)s,       sizeof(wchar_t) * wcslen(s) ) > 0; }
    inline bool         operator>                   ( const buffer& b, const wchar_t c           ) { return b.compare( (const char *)&c,      sizeof(wchar_t)             ) > 0; }
    inline bool         operator>                   ( const buffer& b, const std::string&  s     ) { return b.compare( s.c_str(),             s.size()                    ) > 0; }
    inline bool         operator>                   ( const buffer& b, const std::wstring& s     ) { return b.compare( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ) > 0; }
    inline bool         operator>                   ( const buffer& b, const std::vector<char>& v) { return b.compare( v.data(),              v.size()                    ) > 0; }
                                                                                                              
    inline bool         operator>                   ( const char*   s           , const buffer& b) { return b.compare( s,                     strlen(s)                   ) <= 0; }
    inline bool         operator>                   ( const char    c           , const buffer& b) { return b.compare( &c,                    sizeof(char)                ) <= 0; }
    inline bool         operator>                   ( const wchar_t*s           , const buffer& b) { return b.compare( (const char *)s,       sizeof(wchar_t) * wcslen(s) ) <= 0; }
    inline bool         operator>                   ( const wchar_t c           , const buffer& b) { return b.compare( (const char *)&c,      sizeof(wchar_t)             ) <= 0; }
    inline bool         operator>                   ( const std::string&  s     , const buffer& b) { return b.compare( s.c_str(),             s.size()                    ) <= 0; }
    inline bool         operator>                   ( const std::wstring& s     , const buffer& b) { return b.compare( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ) <= 0; }
    inline bool         operator>                   ( const std::vector<char>& v, const buffer& b) { return b.compare( v.data(),              v.size()                    ) <= 0; }

    // >= 
    inline bool         operator>=                  ( const buffer& b, const buffer& b2          ) { return b.compare( b2.data(),             b2.size()                   ) >= 0; }
    inline bool         operator>=                  ( const buffer& b, const char*   s           ) { return b.compare( s,                     strlen(s)                   ) >= 0; }
    inline bool         operator>=                  ( const buffer& b, const char    c           ) { return b.compare( &c,                    sizeof(char)                ) >= 0; }
    inline bool         operator>=                  ( const buffer& b, const wchar_t*s           ) { return b.compare( (const char *)s,       sizeof(wchar_t) * wcslen(s) ) >= 0; }
    inline bool         operator>=                  ( const buffer& b, const wchar_t c           ) { return b.compare( (const char *)&c,      sizeof(wchar_t)             ) >= 0; }
    inline bool         operator>=                  ( const buffer& b, const std::string&  s     ) { return b.compare( s.c_str(),             s.size()                    ) >= 0; }
    inline bool         operator>=                  ( const buffer& b, const std::wstring& s     ) { return b.compare( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ) >= 0; }
    inline bool         operator>=                  ( const buffer& b, const std::vector<char>& v) { return b.compare( v.data(),              v.size()                    ) >= 0; }
                                                                        
    inline bool         operator>=                  ( const char*   s           , const buffer& b) { return b.compare( s,                     strlen(s)                   ) < 0; }
    inline bool         operator>=                  ( const char    c           , const buffer& b) { return b.compare( &c,                    sizeof(char)                ) < 0; }
    inline bool         operator>=                  ( const wchar_t*s           , const buffer& b) { return b.compare( (const char *)s,       sizeof(wchar_t) * wcslen(s) ) < 0; }
    inline bool         operator>=                  ( const wchar_t c           , const buffer& b) { return b.compare( (const char *)&c,      sizeof(wchar_t)             ) < 0; }
    inline bool         operator>=                  ( const std::string&  s     , const buffer& b) { return b.compare( s.c_str(),             s.size()                    ) < 0; }
    inline bool         operator>=                  ( const std::wstring& s     , const buffer& b) { return b.compare( (const char*)s.c_str(),sizeof(wchar_t) * s.size()  ) < 0; }
    inline bool         operator>=                  ( const std::vector<char>& v, const buffer& b) { return b.compare( v.data(),              v.size()                    ) < 0; }


}
