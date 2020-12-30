local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local luagraphql_parse = require('graphql.parse')

function g.test_select_field()
    local query = [[ { test { a } } ]]
    t.assert_equals(parse(query), luagraphql_parse(query))
end

function g.test_select_set()
    local query = [[ { test { a { b { c d } e } } } ]]
    t.assert_equals(parse(query), luagraphql_parse(query))
end
