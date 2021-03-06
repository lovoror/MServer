#ifndef __LSTATE_H__
#define __LSTATE_H__

#include "../global/global.h"

struct lua_State;

class lstate
{
public:
    static class lstate *instance();
    static void uninstance();

    inline lua_State *state()
    {
        return L;
    }
private:
    lstate();
    ~lstate();

    void open_cpp();

    lua_State *L;
    static class lstate *_state;
};

#endif /* __LSTATE_H__ */
