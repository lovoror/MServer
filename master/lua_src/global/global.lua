--global.lua
--2015-09-07
--xzc

--常用的全局函数

local util = require "util"

local function to_readable( val )
    if type(val) == "string" then
        return "\"" .. val .. "\""
    end

    return val
end

--- @param data 要打印的字符串
--- @param [max_level] table要展开打印的计数，默认nil表示全部展开
--- @param [prefix] 用于在递归时传递缩进，该参数不供用户使用于
local recursion = {}
local function var_dump(data, max_level, prefix)
    if type(prefix) ~= "string" then
        prefix = ""
    end

    if type(data) ~= "table" then
        print(prefix .. tostring(data))
    elseif recursion[data] then
        print(data,"dumplicate")  --重复递归
    else
        recursion[data] = true
        print(data)

        local prefix_next = prefix .. "    "
        print(prefix .. "{")
        for k,v in pairs(data) do
            io.stdout:write(prefix_next .. tostring( to_readable(k) ) .. " = ")
            if type(v) ~= "table" or 
                (type(max_level) == "number" and max_level <= 1) then
                print( to_readable(v) )
            else
                var_dump(v, max_level - 1, prefix_next)
            end
        end
        print(prefix .. "}")
    end
end

--[[
    eg: local b = {aaa="aaa",bbb="bbb",ccc="ccc"}
]]
function vd(data, max_level)
    var_dump(data, max_level or 20)
    recursion = {}  --释放内存
end

-- 时间字符串。用的不是ev:now()，故只能用于错误打印，不能用于游戏逻辑
local function time_str()
    return os.date("%m-%d %H:%M:%S", os.time())
end

local function write_log_file( file,log )
    local file = io.open( file,"a+" )

    -- 无写入权限...
    if not file then return end

    file:write( log )
    file:write( "\r\n" )
    file:close()
end

function __G__TRACKBACK__( msg,co )
    local stack_trace = debug.traceback( co )
    local info_table = { "[LCRASH ",time_str(),"]",tostring(msg),"\n",stack_trace }
    local str = table.concat( info_table )

    print( str )
    write_log_file( "lua_crash.txt",str )
end

--只打印不写入文件
function PFLOG( fmt,... )
    -- 默认为c方式的print字符串格式化打印方式
    if "string" == type( fmt ) then
        print( "[LINFO  " .. time_str() .. "]" .. string.format( fmt,... ) )
    else
        print( "[LINFO  " .. time_str() .. "]",fmt,... )
    end
end

-- 只打印，不格式化
function PLOG( any,... )
    print( "[LINFO  " .. time_str() .. "]" .. tostring(any),... )
end

--错误处调用 直接写入根目录下的lua_error.txt文件 (参数不能带有nil参数)
function ELOG( fmt,... )
    local info_table = nil
    if "string" == type( fmt ) then
        info_table = { "[LERROR ",time_str(),"]",string.format( fmt,... ) }
    else
        info_table = { "[LERROR ",time_str(),"]",fmt,... }
    end

    local ss = table.concat( info_table )
    print(ss)
    write_log_file( "lua_error.txt",ss )
end

--测试时间,耗时打印
local _sec, _usec -- 这函数热更会导致出错，仅测试用
function f_tm_start()
    _sec, _usec = util.timeofday()
end

--[[
1秒＝1000毫秒，
1毫秒＝1000微秒，
1微妙＝1000纳秒，
1纳秒＝1000皮秒。
秒用s表现,毫秒用ms,微秒用μs表示，纳秒用ns表示，皮秒用ps表示
]]
function f_tm_stop(...)
    local sec,usec = util.timeofday()
    assert( sec >= _sec,"time jump" )
    local temp =  math.floor( (sec-_sec)*1000000 + usec - _usec )
    print(...,temp,"microsecond")
end