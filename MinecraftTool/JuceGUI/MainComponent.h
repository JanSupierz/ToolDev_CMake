#pragma once

#include <JuceHeader.h>
#include "TableModel.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AnimatedAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void update() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    juce::TableListBox m_DataTable;
    TableModel* m_pTableModel;

    juce::FileChooser m_InputChooser;
    juce::FileChooser m_OutputChooser;

    juce::Label m_ErrorLabel;

    juce::TextButton m_InputButton;
    juce::TextButton m_OutputButton;
    juce::TextButton m_OkButton;
    juce::TextButton m_CancelButton;

    std::wstring m_InputPath;
    std::wstring m_OutputPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

    void PrintContent(const std::wstring& filePath);
};
