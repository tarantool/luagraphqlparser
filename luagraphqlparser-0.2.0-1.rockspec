package = 'luagraphqlparser'
version = '0.2.0-1'

source  = {
    url    = 'git://github.com/tarantool/luagraphqlparser.git';
    tag = '0.2.0';
}

description = {
    summary  = "Lua graphql parser";
    detailed = [[
        Lua port of libgraphqlparser
    ]];
    homepage = 'https://github.com/tarantool/luagraphqlparser.git';
    maintainer = "Oleg Babin <olegrok@tarantool.org>";
    license  = 'BSD2';
}

dependencies = {
    'lua == 5.1';
}

build = {
    type = 'cmake';
    variables = {
        CMAKE_BUILD_TYPE="RelWithDebInfo";
        TARANTOOL_DIR="$(TARANTOOL_DIR)";
        TARANTOOL_INSTALL_LIBDIR="$(LIBDIR)";
        TARANTOOL_INSTALL_LUADIR="$(LUADIR)";
    };
}
-- vim: syntax=lua ts=4 sts=4 sw=4 et
