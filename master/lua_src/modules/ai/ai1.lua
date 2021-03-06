-- benchmark测试ai逻辑

local PING_MAX = 1000
local PING_ONE_TIME = 2
assert( 0 == PING_MAX%PING_ONE_TIME )

local Ai_action = require "modules.ai.ai_action"

local Ai = {}

local ping_cnt = 0
local ping_idx = 0
local ping_finish = 0
local ping_start  = 0

local finish_entity = {}

function Ai.__init( entity,conf )
    entity.ping_ts = 0
    entity.ping_idx = -1 -- 等待进入游戏

    ping_cnt = ping_cnt + 1
end

function Ai.execute( entity )
    if entity.ping_idx < 0 or entity.ping_ts >= PING_MAX then return end

    if 0 == ping_start then
        PFLOG( "android ping start" )
        ping_start = ev:time()

        -- gm测试
        entity:send_pkt( CS.MISC_WELCOME_GET,{} )
        entity:send_pkt( CS.CHAT_DOCHAT,{ channel = 1,context = "@ghf" } )
        entity:send_pkt( CS.CHAT_DOCHAT,{ channel = 1,context = "@add_gold 9" } )
        entity:send_pkt( CS.CHAT_DOCHAT,{ channel = 1,context = "@add_item 10000 1" } )
        entity:send_pkt( CS.CHAT_DOCHAT,{ channel = 1,context = "@add_item 10001 1" } )
        entity:send_pkt( CS.CHAT_DOCHAT,{ channel = 1,context = "@add_item 20001 1" } )
    end

    for index = 1,PING_ONE_TIME do Ai_action.ping( entity ) end
    Ai_action.random_chat( entity )
end

function Ai.on_enter_world( entity )
    entity.ping_idx = 0
end

function Ai.on_ping( entity,ecode,pkt )
    entity.ping_idx = entity.ping_idx + 1

    if pkt.y ~= entity.ping_idx then
        PFLOG( "pkt pid not match,android %d,expect %d,got %d,ping %d",
                     entity.index,entity.ping_idx,pkt.y,ping_idx )
        os.exit(1)
        return
    end

    ping_idx = ping_idx + 1
    if entity.ping_idx >= PING_MAX then
        ping_finish = ping_finish + 1
        if finish_entity[entity] ~= nil then
            PFLOG( "dumplicate finish,android %d,send %d,recv %d,total %d",
                    entity.index,entity.ping_ts,entity.ping_idx,ping_idx )
            os.exit(1)
            return
        end
        finish_entity[entity] = 1
    end
    if ping_finish == ping_cnt then
        PFLOG( "android finish ping,%d entity recv %d package,time %d",
                            ping_cnt,ping_idx,ev:time() - ping_start )
    end

    if ping_idx > PING_MAX*ping_cnt then
        PFLOG( "too many android ping package receive" )
    end
end

-- 聊天回调
function Ai.on_chat( entity,ecode,pkt )
    -- PFLOG( "chat: %d say %s",pkt.pid,pkt.context )
end

g_player_ev:register( PLAYER_EV.ENTER,Ai.on_enter_world )

local function reg_cb( cmd,func )
    -- local cb = function( entity,ecode,... )
    --     return func( entity,ecode,... )
    -- end

    g_android_mgr:cmd_register( cmd,func )
end

-- 这里有个问题，一旦AI注册了某个协议，其他地方就不能注册了
reg_cb( SC.PLAYER_PING,Ai.on_ping )
reg_cb( SC.CHAT_DOCHAT,Ai.on_chat )

return Ai