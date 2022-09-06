
add_executable(nsh main.c utils.c prompt.c parser.c builtins.c execute.c signals.c)

target_link_libraries(nsh PRIVATE lib)
target_include_directories(nsh PRIVATE ${CMAKE_SOURCE_DIR}/include)