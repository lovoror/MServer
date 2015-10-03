#ifndef __BACKEND_H__
#define __BACKEND_H__

/* 后台工作类
 * socket自杀
 * 假如客户端有数条消息到达，我们收到后在一个while循环中处理，然后在第一条消息中关闭socket
 * (比如数据验证不通过)并直接delete掉socket。可是，while循环还在进行，于是程序当掉。又或者
 * 我们发送完数据后需要关闭socket，但其实数据只是放到sending队列中而已。如果这时delete掉
 * socket对象，则无法完成数据发送。
 *
 * socket和timer需要一个队列来管理，并且这个队列可以根据fd或id快速存取。因为我们并不会把
 * socket或timer对象抛给lua管理。抛给lua管理会造成很多麻烦，比如无法防止socket自杀，无
 * 法设置sendings队列。我们只给lua一个id，backend类作为接口。lua只能告诉它想做什么，然
 * 后backend根据id安全地把工作完成。
 */

#include <lua.hpp>

#include "../global/global.h"
#include "../ev/ev.h"
#include "../ev/ev_watcher.h"
#include "../net/socket.h"

class backend : public ev_loop
{
public:
    explicit backend();
    explicit backend( lua_State *L );
    ~backend();

    int32 exit();
    int32 run ();
    int32 time();
    
    int32 listen();
    int32 connect();
    int32 send();
    int32 raw_send();
    
    int32 io_kill();
    int32 timer_kill();
    
    int32 set_net_ref();
    int32 fd_address();
    
    void listen_cb( ev_io &w,int revents );
    void read_cb( ev_io &w,int revents );
    void connect_cb( ev_io &w,int32 revents );
    
private:
    void packet_parse( int32 fd,class socket *_socket );
    void slist_add( int32 fd,class socket *_socket );
    void invoke_sending();
    void socket_disconect( int32 fd );

private:
    typedef struct
    {
        ev_timer *w;
        uint32 id;
    }ANTIMER;

    typedef class socket *ANIO;/* AN = array node */
    typedef int32 ANSENDING;

    class ev_loop *loop;
    lua_State *L;
    
    /* io 管理队列，通常是socket */
    ANIO *anios;
    int32 aniomax;
    
    /* 待发送队列 */
    ANSENDING *ansendings;
    int32 ansendingmax;
    int32 ansendingcnt;

    /* timer 管理队列 */
    ANTIMER *antimers;
    int32 antimermax;
    int32 antimercnt;
    
    /* lua层网络回调 */
    int32 net_accept;
    int32 net_read;
    int32 net_disconnect;
    int32 net_connected;
    int32 net_self;
};

#endif /* __BACKEND_H__ */
