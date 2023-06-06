#include "Cube.h"
#include "ReadException.h"
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : m_InputChooser("Select Input File", File(File::getSpecialLocation(File::userDocumentsDirectory)), "*.json", false)
    , m_OutputChooser("Select Output File", File(File::getSpecialLocation(File::userDocumentsDirectory)), "*.obj", false)
{
    //Set size
    constexpr int width{ 800 }, height{ 600 };

    setSize(width, height);
    setFramesPerSecond(60);

    //Add table
    addAndMakeVisible(m_DataTable);
    m_pTableModel = new TableModel();
    
    constexpr int nrColumns{ 4 };
    constexpr int columnWidth{ (width / nrColumns) - 1 };
    constexpr int halfWidth{ columnWidth / 2 };

    m_DataTable.setModel(m_pTableModel);
    m_DataTable.getHeader().addColumn("Vector", 1, columnWidth, halfWidth, columnWidth);
    m_DataTable.getHeader().addColumn("x", 2, columnWidth, halfWidth, columnWidth);
    m_DataTable.getHeader().addColumn("y", 3, columnWidth, halfWidth, columnWidth);
    m_DataTable.getHeader().addColumn("z", 4, columnWidth, halfWidth, columnWidth);

    //Add error label
    addAndMakeVisible(m_ErrorLabel);
    m_ErrorLabel.setText("Text input:", juce::dontSendNotification);
    m_ErrorLabel.setVisible(false);

    //Add buttons
    m_InputButton.setButtonText("Input");
    addAndMakeVisible(m_InputButton);

    m_OutputButton.setButtonText("Output");
    addAndMakeVisible(m_OutputButton);

    m_OkButton.setButtonText("OK");
    addAndMakeVisible(m_OkButton);

    m_CancelButton.setButtonText("Cancel");
    addAndMakeVisible(m_CancelButton);

    //Add on click events
    m_InputButton.onClick =
        [&]()
    {
        m_InputChooser.launchAsync(FileBrowserComponent::openMode | 
            FileBrowserComponent::useTreeView | 
            FileBrowserComponent::canSelectFiles,

            [&](const juce::FileChooser& fileChooser)
            {
                const auto selectedFile{ fileChooser.getResult().getFullPathName() };

                //Input selected
                if (selectedFile.isNotEmpty())
                {
                    m_InputPath = selectedFile.toWideCharPointer();
                }
            });
    };

    m_OutputButton.onClick =
        [&]()
    {
        m_OutputChooser.launchAsync(FileBrowserComponent::openMode | 
            FileBrowserComponent::canSelectFiles | 
            FileBrowserComponent::canSelectDirectories | 
            FileBrowserComponent::useTreeView,

            [&](const juce::FileChooser& fileChooser)
            {
                auto selectedFile{ fileChooser.getResult().getFullPathName() };

                //Nothing selected
                if (selectedFile.isEmpty())
                {
                    return;
                }
                
                m_OutputPath = selectedFile.toWideCharPointer();

                //Selected a folder
                if (m_OutputPath.find(L".obj") == std::wstring::npos)
                {
                    m_OutputPath.append(L"\\MinecraftScene.obj");
                }
            });
    };

    m_OkButton.onClick =
        [&]()
    {
        m_ErrorLabel.setVisible(false);

        if (m_InputPath == L"")
        {
            m_ErrorLabel.setText("No input file selected", juce::dontSendNotification);
            m_ErrorLabel.setVisible(true);

            return;
        }

        if (m_OutputPath == L"")
        {
            m_OutputPath = m_InputPath;
            m_OutputPath.erase(m_OutputPath.find(L"json")).append(L"obj");
        }

        std::vector<Cube> cubes{};

        try
        {
            Cube::ParseCubes(m_InputPath, cubes);
        }

        catch (const ReadException& exception)
        {
            m_ErrorLabel.setText(exception.What(), juce::dontSendNotification);
            m_ErrorLabel.setVisible(true);

            m_pTableModel->Clear();
            m_DataTable.updateContent();
            return;
        }

        //Change the path, if given in the argument

        //Save cubes to a file
        Cube::CubesToObj(m_OutputPath, cubes);

        PrintContent(m_OutputPath);
    };

    m_CancelButton.onClick =
        [&]()
    {
        m_InputPath = L"";
        m_OutputPath = L"";

        m_ErrorLabel.setVisible(false);

        m_pTableModel->Clear();
        m_DataTable.updateContent();
    };
}

MainComponent::~MainComponent()
{
    delete m_pTableModel;
}

//==============================================================================
void MainComponent::update()
{
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    const int buttonHeight{ getHeight() - 50 };
    constexpr int width{ 80 }, height{ 40 }, spacing{ 5 };
    m_DataTable.setBounds(0, 0, getWidth(), getHeight() - height * 1.5f);
    
    m_InputButton.setBounds(spacing, buttonHeight, width, height);
    m_OutputButton.setBounds(2 * spacing + width, buttonHeight, width, height);

    const int okButtonOffset{ getWidth() - 2 * (width + spacing) };

    m_OkButton.setBounds(okButtonOffset, buttonHeight, width, height);
    m_CancelButton.setBounds(okButtonOffset + width + spacing, buttonHeight, width, height);

    m_ErrorLabel.setBounds(spacing * 3 + width * 2, getHeight() - 130, getWidth(), 200);
}

void MainComponent::PrintContent(const std::wstring& filePath)
{
    m_pTableModel->Clear();

    std::ifstream inputStream{ filePath };

    std::string line{};
    float x{}, y{}, z{};

    while (!inputStream.eof())
    {
        inputStream >> line;

        if (line == "v")
        {
            inputStream >> x >> y >> z;
            m_pTableModel->PushBack({ "v", x, y, z });
        }
        else if (line == "vn")
        {
            inputStream >> x >> y >> z;
            m_pTableModel->PushBack({ "vn", x, y, z });
        }
        else if (line == "vt")
        {
            inputStream >> x >> y;
            m_pTableModel->PushBack({ "vt", x, y });
        }
    }

    m_DataTable.updateContent();
}