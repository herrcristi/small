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
        stack_string                                () { init(); }

        // from stack_string
        stack_string                                ( const stack_string& o ) noexcept : stack_string() { operator=( o ); }
        stack_string                                ( stack_string&&      o ) noexcept : stack_string() { operator=( std::forward<stack_string>( o ) ); }
        // from char*
        stack_string                                ( const char* s )                  : stack_string() { operator=( s ); }
        stack_string                                ( const char* s, const size_t& s_length ) : stack_string() { set( s, s_length ); }
        // from wchar_t*
        stack_string                                ( const wchar_t *s )               : stack_string() {  operator=( s ); }
        stack_string                                ( const wchar_t *s, const size_t& s_length ) : stack_string() { set( s, s_length ); }
        // from char
        stack_string                                ( const char c    )                : stack_string() { operator=( c ); }
        // from wchar_t
        stack_string                                ( const wchar_t c    )             : stack_string() { operator=( c ); }
        // from std::string
        stack_string                                ( const std::string& s )           : stack_string() { operator=( s ); }
        // from std::wstring
        stack_string                                ( const std::wstring& s )          : stack_string() { operator=( s ); }

        // destructor
        ~stack_string                               () { }



        // size / empty / clear
        inline size_t   size                        () const { return std_string_ ? std_string_->size() : stack_string_size_;  }
        inline size_t   length                      () const { return size();  }
        inline bool     empty                       () const { return size() == 0; }
        inline void     clear                       () { stack_string_size_ = 0; init(); }


        // reserve / resize
        inline void     reserve                     ( const size_t new_size ) { ensure_size( new_size ); if ( std_string_ ) { std_string_->reserve( new_size ); } else {} }
        inline void     resize                      ( const size_t new_size ) { ensure_size( new_size ); if ( std_string_ ) { std_string_->resize( new_size ); } else { stack_string_size_ = new_size; stack_string_[ stack_string_size_ ] = '\0'; } }
        inline void     shrink_to_fit               ()                        { if ( std_string_ ) { std_string_->shrink_to_fit(); } else {} }


        // data
        inline const char* c_str                    () const { return std_string_ ? std_string_.get()->c_str() : stack_string_.data(); }
        inline const char* data                     () const { return std_string_ ? std_string_.get()->data() : stack_string_.dta(); }
        inline char*    data                        ()       { return std_string_ ? std_string_.get()->data() : stack_string_.data(); }

        // as c_string
        inline std::string c_string                 () const { return std_string_ ? *std_string_.get() : std::string( stack_string_, stack_string_size_ ); }
        inline std::string_view c_view              () const { return std::string_view{ data(), size() }; }



        // append
        inline void     append                      ( const stack_string& s     ) { set( s.data(),              s.size(),       size()/*startfrom*/ ); }
        inline void     append                      ( const char* s, size_t s_length ) { set( s,         s_length,       size()/*startfrom*/ ); }
        inline void     append                      ( const char* s             ) { set( s,                     strlen(s),      size()/*startfrom*/ ); }
        inline void     append                      ( const char  c             ) { set( &c,                    sizeof(c),      size()/*startfrom*/ ); }
        inline void     append                      ( const wchar_t* s          ) { set( s,                     wcslen(s),      size()/*startfrom*/ ); }
        inline void     append                      ( const wchar_t* s, size_t s_length ){ set( s,              s_length,       size()/*startfrom*/ ); }
        inline void     append                      ( const wchar_t  c          ) { set( &c,                    sizeof(c) / sizeof(wchar_t), size()/*startfrom*/ ); }
        inline void     append                      ( const std::string& s      ) { set( s.c_str(),             s.size(),       size()/*startfrom*/ ); }
        inline void     append                      ( const std::wstring& s     ) { set( s.c_str(),             s.size(),       size()/*startfrom*/ ); }

        // assign
        inline void     assign                      ( const stack_string& s     ) { if ( this != &s ) { set( s.data(), s.size(), 0/*startfrom*/ ); } }
        inline void     assign                      ( const char* s, const size_t& s_length ) { set( s,         s_length,       0/*startfrom*/ ); }
        inline void     assign                      ( const char* s             ) { set( s,                     strlen(s),      0/*startfrom*/ ); }
        inline void     assign                      ( const char  c             ) { set( &c,                    sizeof(c),      0/*startfrom*/ ); }
        inline void     assign                      ( const wchar_t* s          ) { set( (const char*)s,        wcslen(s),      0/*startfrom*/ ); }
        inline void     assign                      ( const wchar_t* s, const size_t& s_length ){ set( s,       s_length,       0/*startfrom*/ ); }
        inline void     assign                      ( const wchar_t  c          ) { set( &c,                    sizeof(c)/sizeof(wchar_t),      0/*startfrom*/ ); }
        inline void     assign                      ( const std::string& s      ) { set( s.c_str(),             s.size(),       0/*startfrom*/ ); }
        inline void     assign                      ( const std::wstring& s     ) { set( s.c_str(),             s.size(),       0/*startfrom*/ ); }

        // set
        inline void     set                         ( const char* s,    const size_t& s_length, const size_t& start_from = 0 ) { set_impl( s, s_length, start_from ); }
        inline void     overwrite                   ( const char* s,    const size_t& s_length, const size_t& start_from = 0 ) { set( s, s_length, start_from ); }
        
        inline void     set                         ( const wchar_t* s, const size_t& s_length, const size_t& start_from = 0 ) { set_impl( s, s_length, start_from ); }

        // insert
        inline void     insert                      ( const char* b, const size_t& b_length, const size_t& insert_from = 0 ) { insert_impl( b, b_length, insert_from ); }
        // erase
        inline void     erase                       ( const size_t& start_from ) { if ( start_from < size() ) { resize( start_from ); } }
        inline void     erase                       ( const size_t& start_from, const size_t& length ) { erase_impl( start_from, length ); }
        
        // compare
        inline bool     is_equal                    ( const char *s, const size_t& s_length ) const { return size() == s_length && compare( s, s_length ) == 0; }
        inline int      compare                     ( const char *s, const size_t& s_length ) const { return strcmp( data(), s/*, size() < s_length ? size()+1 : s_length+1*/ ); }

        // swap
        inline void     swap                        ( stack_string& o ) { swap_impl( o ); }



        // operators

        // =
        inline stack_string& operator=              ( const stack_string& o     ) noexcept 
        { 
            if ( this != &o ) 
            { 
                stack_string_size_  = o.stack_string_size_; 
                stack_string_       = std::copy( o.stack_string_ ); 
                if ( o.std_string_ ) 
                {  
                    if ( !std_string_ )
                    { 
                        std_string_ = std::make_unique<std::string>(); 
                    } 
                    std_string_ = *o.std_string_.get(); 
                } 
                else 
                { 
                    std_string_.reset(); 
                } 
            } 
            return *this; 
        }
        inline stack_string& operator=              ( stack_string&&      o     ) noexcept 
        { 
            if ( this != &o ) 
            { 
                stack_string_size_  = std::move( o.stack_string_size_ ); 
                stack_string_       = std::move( o.stack_string_ );  
                std_string_         = std::move( o.std_string_ ); 
                o.init(); 
            } 
            return *this; 
        }

        inline stack_string& operator=              ( const char*   s           ) noexcept { set( s,                        strlen(s)       ); return *this; }
        inline stack_string& operator=              ( const char    c           ) noexcept { set( &c,                       sizeof(char)    ); return *this; }
        inline stack_string& operator=              ( const wchar_t*s           ) noexcept { set( s,                        wcslen(s)       ); return *this; }
        inline stack_string& operator=              ( const wchar_t c           ) noexcept { set( &c,                       sizeof(c) / sizeof(wchar_t) ); return *this; }
        inline stack_string& operator=              ( const std::string&  s     ) noexcept { set( s.c_str(),                s.size()        ); return *this; }
        inline stack_string& operator=              ( const std::wstring& s     ) noexcept { set( s.c_str(),                s.size()        ); return *this; }
        
        // +=
        inline stack_string& operator+=             ( const stack_string& s     ) noexcept { append( s.data(),              s.size()        ); return *this; }
        inline stack_string& operator+=             ( const char*   s           ) noexcept { append( s,                     strlen(s)       ); return *this; }
        inline stack_string& operator+=             ( const char    c           ) noexcept { append( &c,                    sizeof(char)    ); return *this; }
        inline stack_string& operator+=             ( const wchar_t*s           ) noexcept { append( s,                     wcslen(s)       ); return *this; }
        inline stack_string& operator+=             ( const wchar_t c           ) noexcept { append( &c,                    1               ); return *this; }
        inline stack_string& operator+=             ( const std::string&  s     ) noexcept { append( s.c_str(),             s.size()        ); return *this; }
        inline stack_string& operator+=             ( const std::wstring& s     ) noexcept { append( s.c_str(),             s.size()        ); return *this; }
        

        // []
        inline char&    operator[]                  ( const size_t& index       ) { return stack_string_[ index ]; }
        inline const char operator[]                ( const size_t& index       ) const { return stack_string_[ index ]; }

        inline char&    at                          ( const size_t& index       ) { return stack_string_[ index ]; }
        inline const char at                        ( const size_t& index       ) const { return stack_string_[ index ]; }

        inline void     push_back                   ( const char c ) { append( c ); }
        inline void     pop_back                    () { if ( size() > 0 ) { resize( size() - 1 ); } }


        // operator
        inline          operator std::string_view   () { return c_view(); }



    
    private:
        // init
        inline void     init                        () { stack_string_size_  = 0; stack_string_.fill( '\0' ); std_string_.reset(); }

        // ensure size
        inline void     ensure_size                 ( size_t new_size )
        {
            if ( new_size < stack_string_.max_size() )
            {

            }
            else 
            {
                if ( !std_string_ )
                {
                    std_string_ = std::make_unique<std::string>( stack_string_.data(), stack_string_size_ );
                } 
            }
        }

        // set impl
        inline void     set_impl                    ( const char* b, const size_t& b_length, const size_t& start_from = 0 ) 
        { 
            resize( start_from + b_length ); 
            //if ( std_string_ )
            //{
            //    memcpy( std_string_.get()->data() + start_from, b, b_length );
            //}
            //else
            //{
            //    memcpy( stack_string_.data() + start_from, b, b_length );
            //}
            memcpy( data() + start_from, b, b_length );
        }

         // set impl
        inline void     set_impl                    ( const wchar_t* b, const size_t& b_length, const size_t& start_from = 0 ) 
        { 
           //TODO
           
        }


        // insert impl
        inline void     insert_impl                 ( const char* b, const size_t& b_length, const size_t& insert_from = 0 ) 
        { 
            size_t initial_length = size();
            reserve( insert_from <= initial_length ? initial_length + b_length : insert_from + b_length );
            
            if ( std_string_ )
            {
                std_string_->insert( insert_from, b, b_length );
            }
            else
            {
                resize( insert_from <= initial_length ? initial_length + b_length : insert_from + b_length );
                if ( insert_from <= initial_length )
                    memmove( stack_string_ + insert_from + b_length, stack_string_ + insert_from, initial_length - insert_from );
                else
                    memset( stack_string_ + initial_length, '\0', (insert_from - initial_length) );
                memcpy( stack_string_ + insert_from, b, b_length );
            }
        }


        // erase
        inline void     erase_impl                       ( const size_t& start_from, const size_t& length ) 
        { 
            if ( std_string_ )
            {
                std_string_->erase( start_from, length );
            }
            else
            {
                if ( start_from < size() )
                {
                    if ( start_from + length < size() )
                    {
                        size_t move_length = size() - (start_from + length);
                        memmove( stack_string_ + start_from, stack_string_ + start_from + length, move_length );
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
        inline void     swap_impl                   ( stack_string& o )
        {
            std::swap( stack_string_size_,  o.stack_string_size_ );
            std::swap( stack_string_,       o.stack_string_ );
            std::swap( std_string_,         o.std_string_ );
        }
    
    private:
        // string by default uses the array and if it goes beyond makes use of the std_string
        std::array<char, StackAllocSize> stack_string_;
        size_t          stack_string_size_;
        
        // for larger string
        std::unique_ptr<std::string> std_string_;
    };
}