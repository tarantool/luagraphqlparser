local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local cartridge_parse = require('cartridge.graphql.parse').parse

function g.test_field()
    local query = [[ query { friends @include(if: $withFriends) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_subselections()
    local query = [[
        query {
            friends @include(if: $withFriends val: 2) @exclude { name { v } }
            friends2 @include(if: $withFriends val: 2) @exclude { name { v } }
        }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_in_operation_definition()
    local query = [[ query @someRandomDirective { op } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_in_fragment_spread()
    local query = [[ {...a@skip} ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_in_inline_fragment_spread()
    local query = [[ {...on a@skip { a }} ]]
    t.assert_equals(parse(query), cartridge_parse(query))

    local query = [[ {...{a}} ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end
