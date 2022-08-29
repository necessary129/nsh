add_executable(nsh main.c utils.c prompt.c parser.c builtins.c)

target_include_directories(nsh PRIVATE ${CMAKE_SOURCE_DIR}/include)