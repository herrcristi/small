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
        
        // data
        inline const char* data                     () const { return buffer_; }
        // extract buffer - be sure to call free after you use it
        inline char*    extract                     () { char* b =  nullptr; if ( buffer_ == empty_buffer_ ) { b = (char*) malloc(sizeof(char)); if (b) { *b='\0'; }  } else { b = buffer_; init( chunk_size_ ); } return b; }

        // reserve / resize
        inline void     reserve                     ( const size_t & new_size, const bool & shrink = false ) { ensure_size( new_size, shrink ); }
        inline void     resize                      ( const size_t & new_size, const bool & shrink = false ) { buffer_length_ = ensure_size( new_size, shrink ); }

        // set
        inline void     append                      ( const char* b, const size_t& b_length ) { set( b, b_length, size() ); }
        inline void     set                         ( const char* b, const size_t& b_length, const size_t& start_from = 0 )  { resize( start_from + b_length, false ); if ( b && buffer_ != empty_buffer_ /*good allocation*/ ) { memcpy( buffer_ + start_from, b, b_length ); buffer_[buffer_length_] = '\0'; } }



        // operators
        inline buffer&  operator=                   ( const buffer& o)  { if ( this != &o ) { chunk_size_ = o.chunk_size_; reserve( o.buffer_length_, true/*shrink*/ ); set( o.buffer_, o.buffer_length_ ); } return *this; }
        inline buffer&  operator=                   ( buffer&&      o ) { if ( this != &o ) { clear( true ); memcpy( this, &o, sizeof( *this ) ); o.init( this->chunk_size_ ); } return *this; }



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
            return new_size;
        }

    private:
        // chunk size
        size_t          chunk_size_;
        // buffer empty
        char            empty_buffer_[1];
        // buffer
        char *          buffer_;
        size_t          buffer_length_;
        size_t          buffer_alloc_size_;
    };
}