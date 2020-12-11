local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local cartridge_parse = require('cartridge.graphql.parse').parse

function g.test_int()
    local query = [[ query($val: Int) { test } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_string()
    local query = [[ query($val: String) { test } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_long_boolean()
    local query = [[ query($l: Long $b: Boolean) { test } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_non_null()
    local query = [[ query($l: Long! $b: Boolean!) { test } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_list()
    local query = [=[ query($l: [Long!] $b: [Boolean] $s: [String]! $l: [[Int]]) { test } ]=]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_custom_type()
    local query = [=[ query($ct: CustomType) { test } ]=]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_default()
    local query = [=[ query($ct: String = [{a: 5}, "test", true, [1]]) { test(a: "String", b: "str") } ]=]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_complex()
    local query = [[
        query HeroNameAndFriends($episode: Episode) {
          hero(episode: $episode) {
            name
            friends {
              name
            }
          }
        }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end
