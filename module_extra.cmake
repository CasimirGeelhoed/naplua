add_subdirectory(thirdparty/lua52)

target_link_import_library(${PROJECT_NAME} lua52)

add_license(luabridge ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/LuaBridge/LICENSE.txt)

add_license(lua52 ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lua52/LICENSE.txt)
