local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local cartridge_parse = require('cartridge.graphql.parse').parse

function g.test_operation_name()
    local query = [[
        query HeroNameAndFriends {
          hero {
            name
            friends {
              name
            }
          }
        }
]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_comment()
    local query = [[
        query HeroNameAndFriends { # Comment
          hero {    # Comment
            name
            friends {   # Comment
              name
            }
          }
        }
        # Comment
]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_metafields()
    local query = [[
        {
          search(text: "an") {
            __typename
            ... on Human {
              name
            }
            ... on Droid {
              name
            }
            ... on Starship {
              name
            }
          }
        }
]]
    t.assert_equals(parse(query), cartridge_parse(query))
end
