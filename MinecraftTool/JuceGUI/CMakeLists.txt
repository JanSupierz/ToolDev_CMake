# Set the source files for the project
set(SOURCES
    Main.cpp
    MainComponent.h
    MainComponent.cpp
)

# Add an executable target for the project
juce_add_gui_app(MyJUCEApp
    PRODUCT_NAME "MinecraftToolGUI"
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "KoenSoft"
    DOCUMENT_EXTENSIONS "myext"
    DOCUMENT_NAMES "My Document"
    DOCUMENT_DESCRIPTIONS "My Document Description"
)

juce_generate_juce_header(MyJUCEApp)

target_sources(MyJUCEApp PRIVATE ${SOURCES} "TableModel.h" "TableModel.cpp" "NamedVector3.h" "NamedVector3.cpp")

# Link against the JUCE module
target_link_libraries(MyJUCEApp
    PRIVATE
        # GuiAppData            # If we'd created a binary data target, we'd link to it here
        juce::juce_gui_extra
    PUBLIC
		CommonCode
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

message(STATUS "Linking with library : ${CommonCodeIncludeDir}")

target_include_directories(
	MyJUCEApp PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${CommonCodeIncludeDir}"
)