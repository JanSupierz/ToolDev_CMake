#Tool directory
#Executable
add_executable(MinecraftTool "MinecraftTool.cpp")
message(STATUS "Linking with library : ${CommonCodeIncludeDir}")

target_include_directories(
	MinecraftTool PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${CommonCodeIncludeDir}"
)

target_link_libraries(MinecraftTool PUBLIC CommonCode)
