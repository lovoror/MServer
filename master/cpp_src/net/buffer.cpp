#include "buffer.h"

class ordered_pool<BUFFER_CHUNK> buffer::allocator;

buffer::buffer()
{
    _buff  = NULL;
    _size  = 0;
    _len   = 0;
    _pos   = 0;

    /* 与客户端通信的默认设定 */
    _max_buff = BUFFER_LARGE;
    _min_buff = BUFFER_CHUNK;
}

buffer::~buffer()
{
    if ( _len )
    {
        allocator.ordered_free( _buff,_len/BUFFER_CHUNK );
    }

    _buff = NULL;
    _size = 0;
    _len  = 0;
    _pos  = 0;
}
