if (APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined suppress -flat_namespace")
endif(APPLE)

# Add C library
add_library(luagraphqlparser SHARED lib.c)
add_dependencies(luagraphqlparser graphqlparser)
target_link_libraries(luagraphqlparser graphqlparser)
set_target_properties(luagraphqlparser PROPERTIES PREFIX "" OUTPUT_NAME "lib")

# Install module
install(TARGETS luagraphqlparser LIBRARY DESTINATION ${TARANTOOL_INSTALL_LIBDIR}/${PROJECT_NAME}/)
install(FILES init.lua DESTINATION ${TARANTOOL_INSTALL_LUADIR}/${PROJECT_NAME}/)
