local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local cartridge_parse = require('cartridge.graphql.parse').parse

function g.test_simple_fragment_definition()
    local query = [[
        fragment f on Obj { k }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_simple_fragment_definition_with_alias()
    local query = [[
        fragment f on Obj { a: k }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_fragment_definition()
    local query = [[
        fragment f on Obj {
            key
            value { nested_obj { nested_field } }
         }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_fragment_spread()
    local query = [[
        { query { ...spread } }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_fragment_complex()
    local query = [[
        {
            leftComparison: hero(episode: EMPIRE) {
                ...comparisonFields
            }
            rightComparison: hero(episode: JEDI) {
                ...comparisonFields
            }
        }

        fragment comparisonFields on Character {
            name
            appearsIn
            friends {
                name
            }
        }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_fragment_with_variables()
    local query = [[
        query HeroComparison($first: Int = 3) {
          leftComparison: hero(episode: EMPIRE) {
            ...comparisonFields
          }
          rightComparison: hero(episode: JEDI) {
            ...comparisonFields
          }
        }

        fragment comparisonFields on Character {
          name
          friendsConnection(first: $first) {
            totalCount
            edges {
              node {
                name
              }
            }
          }
        }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_fragment_inline_simple()
    local query = [[ { value { ... on Human { height } } } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_fragment_inline_complex()
    local query = [[
        query HeroForEpisode($ep: Episode!) {
          hero(episode: $ep) {
            name
            ... on Droid {
              primaryFunction
            }
            ... on Human {
              height
            }
          }
        }
    ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end
