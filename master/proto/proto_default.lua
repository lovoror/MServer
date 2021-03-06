-- server与client交互协议定义

--[[
1.schema文件名就是模块名。如登录模块MODULE_PLAYER中所有协议内容都定义在player.fbs
2.schema中的object名与具体功能协议号相同，并标记服务器或客户端使用。
    如登录功能LOGIN，s2c则为slogin，c2s则为clogin,s2s则为sslogin，当然，也可以使用通用的结构。如empty表示空协议。
3.注意协议号都为大写。这样在代码与容易与其他函数调用区分，并表明它为一个宏定义，不可修改。
4.协议号是一个16bit数字，前8bits表示模块，后8位表示功能。
5.协议必须是一对一。比如c2s协议号是1,那么s2c必须也是1。部分功能不需要对方回包，其他功能也不能占用此协议号。

]]

local MODULE_PLAYER = (0x01 << 8)  -- 256
local MODULE_BAG    = (0x02 << 8)  -- 512

-- faltbuffers
-- local SC =
-- {
--     PLAYER_LOGIN  = { MODULE_PLAYER + 0x01,"player.bfbs","SLogin" },
--     PLAYER_PING   = { MODULE_PLAYER + 0x02,"player.bfbs","CPing" },
--     PLAYER_CREATE = { MODULE_PLAYER + 0x03,"player.bfbs","SCreateRole" },
--     PLAYER_ENTER  = { MODULE_PLAYER + 0x04,"player.bfbs","SEnterWorld" },
--     PLAYER_OTHER  = { MODULE_PLAYER + 0x05,"player.bfbs","SLoginOtherWhere" },
-- }

-- local CS =
-- {
--     PLAYER_LOGIN  = { MODULE_PLAYER + 0x01,"player.bfbs","CLogin" },
--     PLAYER_PING   = { MODULE_PLAYER + 0x02,"player.bfbs","CPing" },
--     PLAYER_CREATE = { MODULE_PLAYER + 0x03,"player.bfbs","CCreateRole" },
--     PLAYER_ENTER  = { MODULE_PLAYER + 0x04,"player.bfbs","CEnterWorld" },
--     PLAYER_OTHER  = { MODULE_PLAYER + 0x05,"player.bfbs","CLoginOtherWhere" },
-- }

local SC =
{
    PLAYER_LOGIN  = { MODULE_PLAYER + 0x01,"player.pb","player.SLogin" },
    PLAYER_PING   = { MODULE_PLAYER + 0x02,"player.pb","player.CPing" },
    PLAYER_CREATE = { MODULE_PLAYER + 0x03,"player.pb","player.SCreateRole" },
    PLAYER_ENTER  = { MODULE_PLAYER + 0x04,"player.pb","player.SEnterWorld" },
    PLAYER_OTHER  = { MODULE_PLAYER + 0x05,"player.pb","player.SLoginOtherWhere" },
}

local CS =
{
    PLAYER_LOGIN  = { MODULE_PLAYER + 0x01,"player.pb","player.CLogin" },
    PLAYER_PING   = { MODULE_PLAYER + 0x02,"player.pb","player.CPing" },
    PLAYER_CREATE = { MODULE_PLAYER + 0x03,"player.pb","player.CCreateRole" },
    PLAYER_ENTER  = { MODULE_PLAYER + 0x04,"player.pb","player.CEnterWorld" },
    PLAYER_OTHER  = { MODULE_PLAYER + 0x05,"player.pb","player.CLoginOtherWhere" },
}

-- 使用oo的define功能让这两个表local后仍能热更
-- 这个表客户端也要用，不要在此文件使用oo.define，在外部使用即可
-- SC = oo.define( _SC,"command_sc" )
-- CS = oo.define( _CS,"command_cs" )


return {SC,CS}
