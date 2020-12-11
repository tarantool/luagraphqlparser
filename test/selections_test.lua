local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local cartridge_parse = require('cartridge.graphql.parse').parse

function g.test_select_field()
    local query = [[ { test { a } } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_select_set()
    local query = [[ { test { a { b { c d } e } } } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end
