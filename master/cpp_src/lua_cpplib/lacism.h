#ifndef __LACISM_H__
#define __LACISM_H__

/* 暂时不提供忽略大小写
 * 原因是原acism库不支持这一功能，一旦修改，代码库不好管理
 * 另外游戏中过滤的很多是中文，大小写无意义.每个字符都转换也消耗性能
 * 如果有少部分大小写都要过滤，请把大小写都加入到关键字列表
 * 如果要修改
 * 1)在加载文件时把内存中的字符都转换为小写
 * 2)在acism_more加个默认参数，是否case_sensitive
 * 3)根据参数将_SYMBOL sym = ps.symv[(uint8_t)*cp++];中的*cp++转换为小写即可
 */

#include <lua.hpp>

#ifdef __cplusplus
extern "C" {
#endif

//#include <msutil.h>
#include <acism.h>

#ifdef __cplusplus
}
#endif


#include "../global/global.h"

typedef struct { char *ptr; size_t len; }	MEMBUF;
typedef struct
{
    int32 rpl_len;                          /* rpl的长度 */
    size_t text_pos;                        /* 已替换部分相对于原字符串位置 */
    size_t word_len;                        /* word的长度 */
    size_t rpl_size;                        /* rpl的总长度 */
    char *rpl_text;                         /* 替换后的字符串 */
    const char *word;                       /* 替换为此字符串 */
    const char *text;                       /* 原字符串 */

    void reserved( size_t bytes )
    {
        size_t new_size = rpl_size ? rpl_size : ACISM_REPLACE_DEFAULT;
        while ( new_size < bytes )
        {
            new_size *= 2;
        }

        const char *tmp = rpl_text;
        rpl_text = new char[new_size];
        if ( tmp )
        {
            memcpy( rpl_text,tmp,rpl_len );
            delete []tmp;
        }
        rpl_size = new_size;
    }
} MEMRPL;

class lacism
{
public:
    ~lacism();
    explicit lacism( lua_State *L );

    int32 scan( lua_State *L );
    int32 replace( lua_State *L );
    int32 load_from_file( lua_State *L );

    int32 do_replace( int32 strnum,int32 textpos );

    static int32 on_match( int32 strnum, int32 textpos, MEMREF const *pattv );
    static int32 on_replace( int32 strnum,int32 textpos,void *context );
private:
    /* 这几个函数在acism.h或msutil.h中都有类似的函数
     * 重写原因如下:
     * 1.原来的函数不支持大小写检测,slurp无法处理空文件...
     * 2.在原代码上修改导致第三方库难升级维护
     * 3.msutil根本就不在libacism.a中，需要改动makefile
     * 4.大小写的处理是在acism.c上修改的，原因是_acism.h中的写法在C++中编译不过
     */
    int32 acism_slurp( const char *path );
    MEMREF *acism_refsplit( char *text, char sep, int *pcount );
private:
    ACISM *_psp;
    MEMBUF _patt;
    int32 _loaded;
    MEMREF *_pattv;
    MEMRPL _memrpl;
    int32 _case_sensitive;
};

#endif /* __LACISM_H__ */
