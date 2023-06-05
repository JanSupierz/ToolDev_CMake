#Root folder - MinecraftTool
cmake_minimum_required(VERSION 3.22)

project(MyJUCEApp VERSION 1.0.0)

# add the JUCE software as library in the project.
add_subdirectory("JUCE")

# common library add_subdirectory
# add the JuceGUI folder with your own code to the project.
add_subdirectory("JuceGUI")

#VS Solution
project(MinecraftTool)

#Json directory
add_subdirectory(rapidjson)
message("Include directory: ${JSONIncludeDir}")

#CommonCode directory
add_subdirectory(CommonCode)
message("Include directory: ${CommonCodeIncludeDir}")

#Executable
add_subdirectory(MinecraftTool)

#Project status
message(STATUS "${PROJECT_SOURCE_DIR}")

#Installer
install(TARGETS MinecraftTool DESTINATION bin)
install(TARGETS MyJUCEApp DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")

#Modify path
set(CPACK_NSIS_MODIFY_PATH ON)

include(CPack)