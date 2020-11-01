#include "MainApp.h"

#include <main_app/debug/Log.h>
#include <main_app/debug/ConfigParser.h>
#include <main_app/debug/Debug.h>
#include <main_app/renderer/Renderer.h>
#include <main_app/event_handler/EventHandler.h>

MainApp::MainApp()
    : ptrLog(new Utils::Log)
    , ptrConfigParser(new Utils::ConfigParser)
    , ptrRenderer(new Renderer)
    , ptrDebug(new Debug)
    , ptrEventHandler(new EventHandler)
{

}

MainApp& MainApp::Get()
{
    static MainApp mainApp;
    return mainApp;
}

bool MainApp::Init(int argc, char* argv[])
{
    std::string strConfigPath = (argc > 1) ? argv[1] : "../src/config.json";
    if(!ptrLog->Init(Utils::ProcessProgamName(argv[0])))
    {
        std::cout << "MainApp: Failed to initialize Log!\n";
        return false;
    }
    if(!ptrConfigParser->Init(strConfigPath))
    {
        LOG_ERROR("MainApp: Failed to initialize ConfigParser!");
        return false;
    }

    auto configData = ptrConfigParser->GetData();

    if(!ptrRenderer->InitSystem(configData.strTitle,
                                configData.screenResolution.nWidth,
                                configData.screenResolution.nHeight,
                                configData.screenResolution.nDepth))
    {
        LOG_ERROR("MainApp: Failed to initialize Renderer!");
        return false;
    }
    ptrRenderer->CreateAndFill2DBuffers();
    ptrRenderer->CreateAndFill3DBuffers();

    if(!ptrDebug->Init(configData.strXMLSavePath))
    {
        LOG_ERROR("MainApp: Failed to initialize Debug!");
        return false;
    }
    return true;
}

void MainApp::Run()
{
    while(!glfwWindowShouldClose(ptrRenderer->GetWindow()))
    {
        /* Debug New frame calculation */
        ptrDebug->NewFrame();

        /* Handle Event */
        ptrEventHandler->ProcessInput(ptrRenderer->GetWindow());

        /* Clear Color */
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Debug Process and Draw */
        ptrDebug->Process();
        ptrRenderer->CallEventsAndSwapBuffers();
    }
}

void MainApp::Deinit()
{
    ptrDebug->Deinit();
    ptrRenderer->Deinit();
}
