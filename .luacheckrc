redefined = false
std = {
    read_globals = {
        'require', 'debug', 'pcall', 'xpcall', 'tostring',
        'tonumber', 'type', 'assert', 'ipairs', 'math', 'error', 'string',
        'table', 'pairs', 'os', 'io', 'select', 'unpack', 'dofile', 'next',
        'loadstring', 'setfenv', 'utf8', 'tonumber64', 'print', 'load',
        'rawget', 'rawset', 'getmetatable', 'setmetatable', '_G', 'jit',
        'collectgarbage',
    },
}
globals = {'box', 'utf8'}
include_files = {'**/*.lua', '*.luacheckrc', '*.rockspec'}
exclude_files = {'**/*.rocks/', 'graphql-lua/'}
max_line_length = 120
