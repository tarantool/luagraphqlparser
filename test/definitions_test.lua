local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local cartridge_parse = require('cartridge.graphql.parse').parse

function g.test_several_definitions()
    local query = [[
        query { test(a: 123) }
        mutation { test(a: 123) }
        mutation { test2(a: 123) }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_several_definitions_with_aliases()
    local query = [[
        query { a1: test(a: 123) }
        mutation { test(a: 123) }
        mutation { a3: test2(a: 123) }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end
