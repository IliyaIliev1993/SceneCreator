#include <iostream>
#include <main_app/MainApp.h>

int main(int argc, char* argv[])
{
    if(!MainApp::Get().Init(argc, argv))
    {
        std::cout << "Failed to Init MainApp!\n";
        return -1;
    }
    MainApp::Get().Run();
    MainApp::Get().Deinit();
    return 0;
}

