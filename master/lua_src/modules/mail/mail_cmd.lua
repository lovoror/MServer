-- mail_cmd.lua
-- 2018-05-20
-- xzc

-- 邮件指令入口

local g_mail_mgr = g_mail_mgr
local g_player_mgr = g_player_mgr

local function mail_mgr_cb( cmd,func )
    local cb = function( srv_conn,pid,pkt )
        local player = g_player_mgr:get_player( pid )
        return func( g_mail_mgr,player,pkt )
    end

    g_command_mgr:clt_register( cmd,cb )
end

-- 其他服发邮件时通过rpc发送个人邮件到world进程处理
local function rpc_send_mail( pid,title,ctx,attachment,op )
    g_mail_mgr:raw_send_mail( pid,title,ctx,attachment,op )
end

-- 其他服发邮件时通过rpc发送系统邮件到world进程处理
local function rpc_send_sys_mail( title,ctx,attachment,op )
    g_mail_mgr:raw_send_sys_mail( title,ctx,attachment,op,expire,level,vip )
end

if "world" == g_app.srvname then
    g_rpc:declare( "rpc_send_mail",rpc_send_mail )
    g_rpc:declare( "rpc_send_sys_mail",rpc_send_sys_mail )
end
