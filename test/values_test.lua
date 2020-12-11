local t = require('luatest')
local g = t.group()

local parse = require('luagraphqlparser').parse
local cartridge_parse = require('cartridge.graphql.parse').parse

function g.test_int()
    local query = [[ query { test(a: 123) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_bool()
    local query = [[ query { test(a: bool) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_float()
    local query = [[ query { test(a: 2.5) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_enum()
    local query = [[ query { test(a: en) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_string()
    local query = [[ query { test(a: "str") } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_null()
    local query = [[ query { test(a: null) } ]]
    local expected = {
        definitions = {
            {
                kind = "operation",
                operation = "query",
                selectionSet = {
                    kind = "selectionSet",
                    selections = {
                        {
                            arguments = {
                                {
                                    kind = "argument",
                                    name = {kind = "name", value = "a"},
                                    value = {kind = "null", value = "null"},
                                },
                            },
                            kind = "field",
                            name = {kind = "name", value = "test"},
                        },
                    },
                },
            },
        },
        kind = "document",
    }
    t.assert_equals(parse(query), expected)
end

function g.test_empty_list()
    local query = [[ query { test(a: []) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_list_simple()
    local query = [[ query { test(a: [1, "a", 2.5, false]) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_empty_object()
    local query = [[ query { test(a: {}) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_object_simple()
    local query = [[ query { test(a: {i: 1 f: 2.5 s: "s" e: en b: true}) } ]]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_nesting()
    local query = [==[ query { test(a: {l: [1, 2, {c: [true, false, {c: {d: {e: 5}}}]}, [1]]}) } ]==]
    t.assert_equals(parse(query), cartridge_parse(query))
end

function g.test_value_with_slash()
    local query = [[ query { a(id: "\\\\a\\\\") } ]]
    t.assert_equals(parse(query), {
        definitions = {
            {
                kind = "operation",
                operation = "query",
                selectionSet = {
                    kind = "selectionSet",
                    selections = {
                        {
                            arguments = {
                                {
                                    kind = "argument",
                                    name = {kind = "name", value = "id"},
                                    value = {kind = "string", value = "\\\\a\\\\"},
                                },
                            },
                            kind = "field",
                            name = {kind = "name", value = "a"},
                        },
                    },
                },
            },
        },
        kind = "document",
    })
end

function g.test_example()
    local query = [[
        {
          human(id: "1000") {
            name
            height
          }
        }
    ]]

    t.assert_equals(parse(query), cartridge_parse(query))
end
