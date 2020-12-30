#!/usr/bin/env tarantool

local fio = require('fio')
local checks = require('checks')
local errors = require('errors')
local clock = require('clock')
local fiber = require('fiber')

local utils = require('cartridge.utils')

local parse = require('cartridge.graphql.parse')
local luagraphqlparser = require('luagraphqlparser')

local Parse_Error = errors.new_class('GraphQL parse failed')

local function load_file(filename)
    checks('string')
    local data, err = utils.file_read(filename)
    if data == nil then return nil, err end

    return data
end

local function print_col(str, width, justify)
    if justify == 'left' or justify == nil then
        return str .. string.rep(' ', width - #str)
    elseif justify == 'center' then
        local half = math.floor((width - #str)/2)
        return string.rep(' ', half) .. str .. string.rep(' ', width - #str - half)
    else
        return string.rep(' ', width - #str) .. str
    end
end

local function test_perf(folder, iterations)
    local requests = {}
    -- load all requests
    folder = fio.pathjoin(debug.sourcedir(), folder)
    if fio.path.is_dir(folder) then
        local files_list, err = fio.glob(fio.pathjoin(folder, '*.graphql'))
        if err ~= nil then
            error(err)
        end
        if files_list then
            for _, file in ipairs(files_list) do
                local item = {
                    request = load_file(fio.abspath(file)),
                    vanilla = 0,
                    fast = 0,
                    name = fio.basename(file):sub(0, #fio.basename(file) -
                            #fio.basename(file):match('^.+(%..+)$'))
                }
                table.insert(requests, item)
            end
        else
            return
        end

        -- make fast parse
        local fast_parse, start, duration
        for index, request in ipairs(requests) do
            print('Fast parsing: ' .. request.name)
            for _ = 1, iterations do
                start = clock.monotonic64()
                fast_parse = luagraphqlparser.parse(request.request)
                duration = clock.monotonic64() - start
                if fast_parse then
                    requests[index].fast = requests[index].fast + tonumber(duration)
                end
            end
            fiber.yield()
        end

        collectgarbage()
        collectgarbage()

        -- make vanilla parse
        local vanilla_parse
        for index, request in ipairs(requests) do
            print('Vanilla parsing: ' .. request.name)
            for _ = 1, iterations do
                start = clock.monotonic64()
                vanilla_parse, err = Parse_Error:pcall(parse.parse, request.request)
                if err ~= nil then
                    error(err)
                end
                duration = clock.monotonic64() - start
                if vanilla_parse then
                    requests[index].vanilla = requests[index].vanilla + tonumber(duration)
                end
            end
            fiber.yield()
        end

        print(
                print_col('Request', 20, 'center') .. ' | ' ..
                        print_col('Iterations', 10, 'center') .. ' | ' ..
                        print_col('Vanilla took time, ms', 22, 'center') .. ' | '..
                        print_col('Fast took time, ms', 22, 'center') .. ' | ' ..
                        print_col('Vanilla, RPS', 15, 'center') .. ' | ' ..
                        print_col('Fast, RPS', 15, 'center') .. ' | ' ..
                        print_col('Fast vs Vanilla ratio', 25, 'center') .. ' | '
        )

        print(
                print_col(string.rep('-', 19), 20, 'center') .. ' | ' ..
                        print_col(string.rep('-', 9), 10, 'center') .. ' | ' ..
                        print_col(string.rep('-', 21), 22, 'center') .. ' | '..
                        print_col(string.rep('-', 21), 22, 'center') .. ' | ' ..
                        print_col(string.rep('-', 14), 15, 'center') .. ' | ' ..
                        print_col(string.rep('-', 14), 15, 'center') .. ' | ' ..
                        print_col(string.rep('-', 24), 25, 'center') .. ' | '
        )

        for _, request in ipairs(requests) do
            print(
                    print_col(request.name, 20, 'left') .. ' | ' ..
                            print_col(tostring(iterations), 10, 'center') .. ' | ' ..
                            print_col(string.format('%.3f', request.vanilla/1e6), 22, 'center').. ' | '..
                            print_col(string.format('%.3f', request.fast/1e6), 22, 'center').. ' | ' ..
                            print_col(tostring(math.floor(1/(request.vanilla/1e9/iterations))), 15, 'right').. ' | ' ..
                            print_col(tostring(math.floor(1/(request.fast/1e9/iterations))), 15, 'right').. ' | ' ..
                            print_col(string.format('%.3f', request.vanilla/request.fast), 25, 'right').. ' | '
            )
        end
    else
        print(string.format('%q is not a folder', folder))
    end
end

print('\nPerformance test (short run):')
test_perf('./fixtures/execution/', 1)

print('\nPerformance test (long run):')
test_perf('./fixtures/execution/', 100000)
