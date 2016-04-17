#include "lstream_socket.h"

#include "ltools.h"
#include "lclass.h"
#include "../ev/ev_def.h"

#define DEFINE_STREAM_READ_FUNCTION(type)                           \
    int32 lstream_socket::read_##type()                             \
    {                                                               \
        int32 *perr = 0;                                            \
        type val = _recv.read_##type( perr );                       \
        if ( perr < 0 )                                             \
        {                                                           \
            return luaL_error( L,"read_"#type" buffer overflow" );  \
        }                                                           \
        lua_pushinteger( L,val );                                   \
        return 1;                                                   \
    }

lstream_socket::lstream_socket( lua_State *L )
    : lsocket(L)
{

}

lstream_socket::~lstream_socket()
{
}

DEFINE_STREAM_READ_FUNCTION( int8   )
DEFINE_STREAM_READ_FUNCTION( uint8  )
DEFINE_STREAM_READ_FUNCTION( int16  )
DEFINE_STREAM_READ_FUNCTION( uint16 )
DEFINE_STREAM_READ_FUNCTION( int32  )
DEFINE_STREAM_READ_FUNCTION( uint32 )
DEFINE_STREAM_READ_FUNCTION( float  )
DEFINE_STREAM_READ_FUNCTION( double )

int32 lstream_socket::read_int64()
{
    int32 *perr = 0;

    int64 val = _recv.read_int64( perr );
    if ( perr < 0 )
    {
        return luaL_error( L,"read_int64 buffer overflow" );
    }

    lua_pushint64( L,val );
    return 1;
}

int32 lstream_socket::read_uint64()
{
    int32 *perr = 0;

    uint64 val = _recv.read_uint64( perr );
    if ( perr < 0 )
    {
        return luaL_error( L,"read_uint64 buffer overflow" );
    }

    lua_pushint64( L,val );
    return 1;
}

int32 lstream_socket::is_message_complete()
{
    uint32 size = _recv.data_size();
    if ( size < sizeof(uint16) ) return 0;

    uint16 plt = _recv.read_uint16( 0,false );

    return (size < sizeof(uint16) + plt) ? 0 : 1;
}

void lstream_socket::listen_cb( int32 revents )
{
    assert( "libev listen cb  error",!(EV_ERROR & revents) );

    while ( socket::active() )
    {
        int32 new_fd = socket::accept();
        if ( new_fd < 0 )
        {
            if ( EAGAIN != errno && EWOULDBLOCK != errno )
            {
                FATAL( "stream_socket::accept fail:%s\n",strerror(errno) );
                return;
            }

            break;  /* 所有等待的连接已处理完 */
        }

        socket::non_block( new_fd );
        KEEP_ALIVE( new_fd );
        USER_TIMEOUT( new_fd );

        /* 直接进入监听 */
        class lstream_socket *_s = new class lstream_socket( L );
        _s->socket::set<lsocket,&lsocket::message_cb>( _s );
        _s->socket::start( new_fd,EV_READ );  /* 这里会设置fd */

        lua_rawgeti(L, LUA_REGISTRYINDEX, ref_acception);
        int32 param = 1;
        if ( ref_self )
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, ref_self);
            param ++;
        }
        lclass<lstream_socket>::push( L,_s,true );
        if ( expect_false( LUA_OK != lua_pcall(L,param,0,0) ) )
        {
            /* 如果失败，会出现几种情况：
             * 1) lua能够引用socket，只是lua其他逻辑出错.需要lua层处理此异常
             * 2) lua不能引用socket，导致lua层gc时会销毁socket,ev还来不及fd_reify，又
             *    将此fd删除，触发一个错误
             */
            ERROR( "listen cb call accept handler fail:%s",lua_tostring(L,-1) );
            return;
        }
    }
}
