#ifndef MAINAPP_H_
#define MAINAPP_H_

#include <memory>

/** Forward Declarations */
namespace Utils
{
    class Log;
    class ConfigParser;
}
class Renderer;
class Debug;
class EventHandler;

class MainApp
{
public:
    /**
     * @brief Get - create and return a Singleton instance of the MainApp class.
     * @return - a reference to the MainApp Singleton class.
     */
    static MainApp& Get();

    /** Delete copyt-constructor, move-constructor and assignment operators */
    MainApp(MainApp const&) = delete;
    void operator = (MainApp const&) = delete;
    MainApp(MainApp &&) = delete;
    void operator = (MainApp &&) = delete;

    /**
     * @brief Init - initialize member fields.
     * @param argc - number of command line arguments.
     * @param argv - an array of command line arguments.
     * argv[0] - program name.
     * argv[1] - path to a JSON configuration file.
     * @return if the initialization was successfult.
     */
    bool Init(int argc, char* argv[]);
    /**
     * @brief Run - main execution loop.
     */
    void Run();
    /**
     * @brief Deinit - free systems and resources.
     */
    void Deinit();
public:
    std::unique_ptr<Utils::Log> ptrLog;
    std::unique_ptr<Utils::ConfigParser> ptrConfigParser;
    std::unique_ptr<Renderer> ptrRenderer;
    std::unique_ptr<Debug> ptrDebug;
    std::unique_ptr<EventHandler> ptrEventHandler;
private:
    /** Constructor and destructor are private, so this class can't be
     * instantiated outside of the Get() method */
    MainApp();
    ~MainApp() = default;
};

#endif // MAINAPP_H_
