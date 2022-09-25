add_library(lib 
	error_handler.c 
	sdll.c 
)

target_include_directories(lib PRIVATE ${CMAKE_SOURCE_DIR}/include)