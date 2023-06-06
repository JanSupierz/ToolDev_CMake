#Tool directory
#Executable
add_executable(MinecraftTool "MinecraftTool.cpp")
message(STATUS "Linking with directory : ${JSONIncludeDir}")
message(STATUS "Linking with directory : ${CommonCodeIncludeDir}")

target_include_directories(
	MinecraftTool PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${JSONIncludeDir}"
	"${CommonCodeIncludeDir}"
)

target_link_directories(MinecraftTool PUBLIC rapijson CommonCode)
