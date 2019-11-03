#pragma once

#include <type_traits>
#include <string.h>
#include <stddef.h>

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
    // class for representing a buffer
    class buffer
    {
    public:
        // buffer (allocates in chunks)
        buffer                                      ( const int& chunk = 4096 ) { init( chunk ); }

        buffer                                      ( const buffer& o) { init( o.chunk_size_ ); operator=( o ); }
        buffer                                      ( buffer&&      o) { init( o.chunk_size_ ); operator=( std::forward<buffer>( o ) ); }
        // destructor
        ~buffer                                     () { free_buffer(); }
    

         // size / empty / clear
        inline size_t   size                        () const { return buffer_length_;  }
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
        inline char*    get_buffer                  () { return buffer_; }
        inline const char* data                     () const { return buffer_; }
        inline char*    data                        () { return buffer_; }


        // extract buffer - be sure to call free after you use it
        inline char*    extract                     () { char* b =  nullptr; if ( buffer_ == empty_buffer_ ) { b = (char*) malloc(sizeof(char)); if (b) { *b='\0'; }  } else { b = buffer_; init( chunk_size_ ); } return b; }


        // set
        inline void     append                      ( const char* b, const size_t& b_length ) { set( b, b_length, size()/*startfrom*/ ); }
        inline void     assign                      ( const char* b, const size_t& b_length ) { set( b, b_length,      0/*startfrom*/ ); }
        inline void     overwrite                   ( const char* b, const size_t& b_length, const size_t& start_from = 0 ) { set( b, b_length, start_from ); }

        inline void     set                         ( const char* b, const size_t& b_length, const size_t& start_from = 0 ) { set_impl( b, b_length, start_from ); }

        // insert
        inline void     insert                      ( const char* b, const size_t& b_length, const size_t& insert_from = 0 ) { insert_impl( b, b_length, insert_from ); }
        // erase
        inline void     erase                       ( const size_t& start_from ) { if ( start_from < size() ) { resize( start_from ); } }
        inline void     erase                       ( const size_t& start_from, const size_t& length ) { erase_impl( start_from, length ); }



        // operators
        inline buffer&  operator=                   ( const buffer& o ) noexcept { if ( this != &o ) { chunk_size_ = o.chunk_size_; reserve( o.size(), true/*shrink*/ ); set( o.data(), o.size() ); } return *this; }
        inline buffer&  operator=                   ( buffer&&      o ) noexcept { if ( this != &o ) { clear( true ); memcpy( this, &o, sizeof( *this ) ); o.init( this->chunk_size_ ); } return *this; }

        

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
}