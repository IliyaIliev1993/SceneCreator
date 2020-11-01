#ifndef LOG_H_
#define LOG_H_

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <memory>
#include <main_app/MainApp.h>

namespace Utils
{
class Log
{
public:
    bool Init(const std::string& strTitle);
    std::shared_ptr<spdlog::logger>& GetLogger() { return m_ptrLogger; }
private:
    std::shared_ptr<spdlog::logger> m_ptrLogger;
};
}

/* Log macros */
#define LOG_TRACE(...)         MainApp::Get().ptrLog->GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)          MainApp::Get().ptrLog->GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)          MainApp::Get().ptrLog->GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)         MainApp::Get().ptrLog->GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)      MainApp::Get().ptrLog->GetLogger()->critical(__VA_ARGS__)


#endif // LOG_H_
