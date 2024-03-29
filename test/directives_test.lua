local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local luagraphql_parse = require('graphql.parse')

function g.test_field()
    local query = [[ query { friends @include(if: $withFriends) } ]]
    t.assert_equals(parse(query), luagraphql_parse(query))
end

function g.test_subselections()
    local query = [[
        query {
            friends @include(if: $withFriends val: 2) @exclude { name { v } }
            friends2 @include(if: $withFriends val: 2) @exclude { name { v } }
        }
    ]]
    t.assert_equals(parse(query), luagraphql_parse(query))
end

function g.test_in_operation_definition()
    local query = [[ query @someRandomDirective { op } ]]
    t.assert_equals(parse(query), luagraphql_parse(query))
end

function g.test_in_fragment_spread()
    local query = [[ {...a@skip} ]]
    t.assert_equals(parse(query), luagraphql_parse(query))
end

function g.test_in_inline_fragment_spread()
    local query = [[ {...on a@skip { a }} ]]
    t.assert_equals(parse(query), luagraphql_parse(query))

    local query = [[ {...{a}} ]]
    t.assert_equals(parse(query), luagraphql_parse(query))
end

function g.test_with_fragment()
    local query = 'fragment X on Y\n@foo\n{ name }'
    -- We don't use old (Lua-implemented) graphql parser here
    -- since it doesn't support such declaration.
    local expected = {
        definitions = {
            {
                typeCondition = {
                    name = {
                        kind = 'name', value = 'Y',
                    },
                    kind = 'namedType',
                },
                directives = {
                    {name = {kind = 'name', value = 'foo'}, kind = 'directive'},
                },
                selectionSet = {
                    selections = {
                        {name = {kind = 'name', value = 'name'}, kind = 'field'},
                    },
                    kind = 'selectionSet',
                },
                name = {kind = 'name', value = 'X'},
                kind = 'fragmentDefinition',
            }
        },
        kind = 'document',
    }
    t.assert_equals(parse(query), expected)
end
