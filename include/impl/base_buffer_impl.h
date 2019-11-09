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
        base_buffer                                 (const base_buffer& ) = delete;
        base_buffer                                 (base_buffer&&      ) = delete;
        virtual ~base_buffer                        () {}
        
    public:
         // functions for getting size size / length / empty 
        inline size_t   size                        () const    { return buffer_length_;   }
        inline size_t   length                      () const    { return size();           }
        inline bool     empty                       () const    { return size() == 0;      }
                
        
        // functions to supply that affect size 
        // clear / reserve / resize / shrink_to_fit
        inline void     clear                       ()                  { this->clear_impl();              }
        inline void     reserve                     ( size_t new_size ) { this->reserve_impl( new_size );  }
        inline void     resize                      ( size_t new_size ) { this->resize_impl ( new_size );  }
        inline void     shrink_to_fit               ()                  { this->shrink_impl();             }

        
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
        inline void     assign                      ( const base_buffer& b      )   { if ( this != &b ) { set( 0/*startfrom*/, b.data(), b.size() ); } }
        
        inline void     assign                      ( const char  c             )   { set( 0/*startfrom*/, &c,          1           ); }
        inline void     assign                      ( const char* s             )   { set( 0/*startfrom*/, s,           strlen( s ) ); }
        inline void     assign                      ( const char* s, size_t len )   { set( 0/*startfrom*/, s,           len         ); }
        inline void     assign                      ( const std::string& s      )   { set( 0/*startfrom*/, s.c_str(),   s.size()    ); }
        inline void     assign                      ( const std::string_view s  )   { set( 0/*startfrom*/, s.data(),    s.size()    ); }
        inline void     assign                      ( const std::vector<char>& v)   { set( 0/*startfrom*/, v.data(),    v.size()    ); }

        inline void     assign                      ( const wchar_t  c          )   { set( 0/*startfrom*/, &c,          1           ); }
        inline void     assign                      ( const wchar_t* s          )   { set( 0/*startfrom*/, s,           wcslen( s ) ); }
        inline void     assign                      ( const wchar_t* s, size_t len ){ set( 0/*startfrom*/, s,           len         ); }
        inline void     assign                      ( const std::wstring& s     )   { set( 0/*startfrom*/, s.c_str(),   s.size()    ); }
        inline void     assign                      ( const std::wstring_view s )   { set( 0/*startfrom*/, s.data(),    s.size()    ); }
        inline void     assign                      ( const std::vector<wchar_t>& v){ set( 0/*startfrom*/, v.data(),    v.size()    ); }


        // append
        inline void     append                      ( const base_buffer& b      )   { set( size()/*startfrom*/, b.data(),   b.size()    ); }
                                                                                                               
        inline void     append                      ( const char  c             )   { set( size()/*startfrom*/, &c,         1           ); }
        inline void     append                      ( const char* s             )   { set( size()/*startfrom*/, s,          strlen( s ) ); }
        inline void     append                      ( const char* s, size_t len )   { set( size()/*startfrom*/, s,          len         ); }
        inline void     append                      ( const std::string& s      )   { set( size()/*startfrom*/, s.c_str(),  s.size()    ); }
        inline void     append                      ( const std::string_view s  )   { set( size()/*startfrom*/, s.data(),   s.size()    ); }
        inline void     append                      ( const std::vector<char>& v)   { set( size()/*startfrom*/, v.data(),   v.size()    ); }
                                                                                                              
        inline void     append                      ( const wchar_t  c          )   { set( size()/*startfrom*/, &c,          1          ); }
        inline void     append                      ( const wchar_t* s          )   { set( size()/*startfrom*/, s,           wcslen( s )); }
        inline void     append                      ( const wchar_t* s, size_t len ){ set( size()/*startfrom*/, s,           len        ); }
        inline void     append                      ( const std::wstring& s     )   { set( size()/*startfrom*/, s.c_str(),   s.size()   ); }
        inline void     append                      ( const std::wstring_view s )   { set( size()/*startfrom*/, s.data(),    s.size()   ); }
        inline void     append                      ( const std::vector<wchar_t>& v){ set( size()/*startfrom*/, v.data(),    v.size()   ); }


        // insert
        inline void     insert                      ( size_t from, const base_buffer& b      )  { this->insert_impl( from, b.data(),    b.size()    ); }
                        
        inline void     insert                      ( size_t from, const char  c             )  { this->insert_impl( from, &c,          1           ); }
        inline void     insert                      ( size_t from, const char* s             )  { this->insert_impl( from, s,           strlen( s ) ); }
        inline void     insert                      ( size_t from, const char* s, size_t len )  { this->insert_impl( from, s,           len         ); }
        inline void     insert                      ( size_t from, const std::string& s      )  { this->insert_impl( from, s.c_str(),   s.size()    ); }
        inline void     insert                      ( size_t from, const std::string_view s  )  { this->insert_impl( from, s.data(),    s.size()    ); }
        inline void     insert                      ( size_t from, const std::vector<char>& v)  { this->insert_impl( from, v.data(),    v.size()    ); }
                        
        inline void     insert                      ( size_t from, const wchar_t  c          )  { this->insert_impl( from, &c,          1           ); }
        inline void     insert                      ( size_t from, const wchar_t* s          )  { this->insert_impl( from, s,           wcslen( s ) ); }
        inline void     insert                      ( size_t from, const wchar_t* s, size_t len ){this->insert_impl( from, s,           len         ); }
        inline void     insert                      ( size_t from, const std::wstring& s     )  { this->insert_impl( from, s.c_str(),   s.size()    ); }
        inline void     insert                      ( size_t from, const std::wstring_view s )  { this->insert_impl( from, s.data(),    s.size()    ); }
        inline void     insert                      ( size_t from, const std::vector<wchar_t>& v){this->insert_impl( from, v.data(),    v.size()    ); }

        
        // overwrite
        inline void     overwrite                   ( size_t from, const base_buffer& b      )  { set( from, b.data(),  b.size()    ); }
                                                                                                           
        inline void     overwrite                   ( size_t from, const char  c             )  { set( from, &c,        1           ); }
        inline void     overwrite                   ( size_t from, const char* s             )  { set( from, s,         strlen( s ) ); }
        inline void     overwrite                   ( size_t from, const char* s, size_t len )  { set( from, s,         len         ); }
        inline void     overwrite                   ( size_t from, const std::string& s      )  { set( from, s.c_str(), s.size()    ); }
        inline void     overwrite                   ( size_t from, const std::string_view s  )  { set( from, s.data(),  s.size()    ); }
        inline void     overwrite                   ( size_t from, const std::vector<char>& v)  { set( from, v.data(),  v.size()    ); }
                                                                                                            
        inline void     overwrite                   ( size_t from, const wchar_t  c          )  { set( from, &c,        1           ); }
        inline void     overwrite                   ( size_t from, const wchar_t* s          )  { set( from, s,         wcslen( s ) ); }
        inline void     overwrite                   ( size_t from, const wchar_t* s, size_t len ){set( from, s,         len         ); }
        inline void     overwrite                   ( size_t from, const std::wstring& s     )  { set( from, s.c_str(), s.size()    ); }
        inline void     overwrite                   ( size_t from, const std::wstring_view s )  { set( from, s.data(),  s.size()    ); }
        inline void     overwrite                   ( size_t from, const std::vector<wchar_t>& v){set( from, v.data(),  v.size()    ); }


        // set
        inline void     set                         ( size_t from, const base_buffer& b         ) { this->set_impl( from, b.data(), b.size()    ); }
                        
        inline void     set                         ( size_t from, const char  c                ) { this->set_impl( from, &c,       1           ); }
        inline void     set                         ( size_t from, const char* s                ) { this->set_impl( from, s,        strlen( s ) ); }
        inline void     set                         ( size_t from, const char* s, size_t len    ) { this->set_impl( from, s,        len         ); }
        inline void     set                         ( size_t from, const std::string& s         ) { this->set_impl( from, s.c_str(),s.size()    ); }
        inline void     set                         ( size_t from, const std::string_view s     ) { this->set_impl( from, s.data(), s.size()    ); }
        inline void     set                         ( size_t from, const std::vector<char>& v   ) { this->set_impl( from, v.data(), v.size()    ); }
                        
        inline void     set                         ( size_t from, const wchar_t  c             ) { this->set_impl( from, &c,       1           ); }
        inline void     set                         ( size_t from, const wchar_t* s             ) { this->set_impl( from, s,        wcslen( s ) ); }
        inline void     set                         ( size_t from, const wchar_t* s, size_t len ) { this->set_impl( from, s,        len         ); }
        inline void     set                         ( size_t from, const std::wstring& s        ) { this->set_impl( from, s.c_str(),s.size()    ); }
        inline void     set                         ( size_t from, const std::wstring_view s    ) { this->set_impl( from, s.data(), s.size()    ); }
        inline void     set                         ( size_t from, const std::vector<wchar_t>& v) { this->set_impl( from, v.data(), v.size()    ); }

        
        // erase
        inline void     erase                       ( size_t from )                 { if ( from < size() ) { resize( from ); } }
        inline void     erase                       ( size_t from, size_t length )  { this->erase_impl( from, length ); }
        

        // compare
        inline bool     is_equal                    ( const char *s, size_t s_length ) const { return size() == s_length && compare( s, s_length ) == 0; }
        inline int      compare                     ( const char *s, size_t s_length ) const 
        { 
            int cmp = memcmp( data(), s, size() < s_length ? size() : s_length );  
            return (cmp != 0) ? /*different*/cmp : /*equal so far*/(size() == s_length ? 0/*true equal*/ : (size() < s_length ? -1 : 1));
        }

        // TODO
        inline bool     is_equal                    ( const wchar_t *s, size_t s_length ) const { return size() == s_length && compare( s, s_length ) == 0; }
        inline int      compare                     ( const wchar_t *s, size_t s_length ) const { return memcmp( data(), s, size() < s_length ? size()+1 : s_length+1 ); }

        
        // operators

        // these specific operators must be implemented in derived classes
        inline base_buffer&  operator=              ( const base_buffer& o           ) = delete;
        inline base_buffer&  operator=              ( base_buffer&&      o           ) = delete;
        // =
        inline base_buffer&  operator=              ( const char c              ) noexcept { set( 0/*from*/, &c,            1           ); return *this; }
        inline base_buffer&  operator=              ( const char* s             ) noexcept { set( 0/*from*/, s,             strlen( s ) ); return *this; }
        inline base_buffer&  operator=              ( const std::string&  s     ) noexcept { set( 0/*from*/, s.c_str(),     s.size()    ); return *this; }
        inline base_buffer&  operator=              ( const std::string_view  s ) noexcept { set( 0/*from*/, s.data(),      s.size()    ); return *this; }
        inline base_buffer&  operator=              ( const std::vector<char>& v) noexcept { set( 0/*from*/, v.data(),      v.size()    ); return *this; }
                                                                                                           
        inline base_buffer&  operator=              ( const wchar_t c           ) noexcept { set( 0/*from*/, &c,            1           ); return *this; }
        inline base_buffer&  operator=              ( const wchar_t* s          ) noexcept { set( 0/*from*/, s,             wcslen( s ) ); return *this; }
        inline base_buffer&  operator=              ( const std::wstring& s     ) noexcept { set( 0/*from*/, s.c_str(),     s.size()    ); return *this; }
        inline base_buffer&  operator=              ( const std::wstring_view s ) noexcept { set( 0/*from*/, s.data(),      s.size()    ); return *this; }
        inline base_buffer&  operator=              ( const std::vector<wchar_t>& v)noexcept{set( 0/*from*/, v.data(),      v.size()    ); return *this; }


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


        // !! functions to override
        virtual void    clear_impl      () = 0;
        virtual void    reserve_impl    ( size_t/*size*/ ) = 0;
        virtual void    resize_impl     ( size_t/*size*/ ) = 0;
        virtual void    shrink_impl     () = 0;
        
        virtual void    set_impl        ( size_t from, const char*    buffer, size_t length ) { buffer_set_impl( from, buffer, length ); }
        virtual void    set_impl        ( size_t from, const wchar_t* buffer, size_t length ) { buffer_set_impl( from, (const char*)buffer, sizeof( wchar_t ) * length ); }
        
        virtual void    insert_impl     ( size_t from, const char*    buffer, size_t length ) { buffer_insert_impl( from, buffer, length ); }
        virtual void    insert_impl     ( size_t from, const wchar_t* buffer, size_t length ) { buffer_insert_impl( from, (const char*)buffer, sizeof( wchar_t ) * length ); }

        virtual void    erase_impl      ( size_t from, size_t length ) { buffer_erase_impl( from, length ); }
        

    protected:
        // buffer set
        inline void     buffer_set_impl             ( size_t from, const char* b, size_t b_length )
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
