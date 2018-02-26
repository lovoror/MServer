-- module_pre_header.lua
-- 2018-01-26
-- xzc

-- 引入高优先级的文件
-- 这里的文件请谨慎热更.global.oo是不能热更的

local require_ext = require_ex or require

require_ext "global.global" -- 这个文件是基础文件，但可以热更的
require "global.oo"
require_ex "global.table"
require_ex "global.string"

require_ex "modules.system.define"

g_unique_id = require "modules.system.unique_id"