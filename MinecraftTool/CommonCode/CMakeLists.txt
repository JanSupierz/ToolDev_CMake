add_library(CommonCode Cube.cpp ReadException.cpp)
set(CommonCodeIncludeDir "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)

message(STATUS "Linking with directory : ${JSONIncludeDir}")

target_include_directories(
	CommonCode PUBLIC
	"${JSONIncludeDir}"
)

target_link_directories(CommonCode PUBLIC rapidjson)