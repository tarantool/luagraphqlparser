#!/usr/bin/env tarantool

local libgraphqlparser = require('luagraphqlparser').parse
local lulpegparser = require('cartridge.graphql.parse').parse

local query = [[
{
  nodes(ids: ["MDEyOk9yZ2FuaXphdGlvbjYyMzM5OTQ=", "MDEyOk9yZ2FuaXphdGlvbjY0MzA3MA=="]) {

    # Join to a nested list of organization objects.
    id
    ... on Organization {
      name
      repositories(first: 100, privacy: PUBLIC) {
        nodes {
          name
          createdAt
          url
          homepageUrl
          languages(first:5) {
            nodes {
              name
            }
          }
          pullRequests(first:5, states:[OPEN]) {
            nodes {
              author {
                login
              }
              title
            }
          }
        }
      }
    }
  }

  rateLimit {
    limit     # Your maximum budget. Your budget is reset to this every hour.
    cost      # The cost of this query.
    remaining # How much of your API budget remains.
    resetAt   # The time (in UTC epoch seconds) when your budget will reset.
  }
}
]]

local count = 1e3
local queries = table.new(count, 0)

for i = 1, count do
    queries[i] = string.rep('\n { line(first: $first) } ', i) .. query
end

local clock = require('clock')

local jit_off_libgraphql
local jit_off_lulpeg
local jit_on_libgraphql
local jit_on_lulpeg

local start

jit.off()
start = clock.time()
for i = 1, count do
    libgraphqlparser(queries[i])
end
jit_off_libgraphql = clock.time() - start

collectgarbage()
collectgarbage()

start = clock.time()
for i = 1, count do
    lulpegparser(queries[i])
end
jit_off_lulpeg = clock.time() - start

collectgarbage()
collectgarbage()

jit.on()

start = clock.time()
for i = 1, count do
    lulpegparser(queries[i])
end
jit_on_lulpeg = clock.time() - start

collectgarbage()
collectgarbage()

start = clock.time()
for i = 1, count do
    libgraphqlparser(queries[i])
end
jit_on_libgraphql = clock.time() - start

collectgarbage()
collectgarbage()

--
print('jit_off_libgraphql:', jit_off_libgraphql)
print('jit_off_lulpeg:', jit_off_lulpeg)
print('jit_on_libgraphql:', jit_on_libgraphql)
print('jit_on_lulpeg:', jit_on_lulpeg)

os.exit()
