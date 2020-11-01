#include <main_app/debug/Utils.h>
#include <type_traits>
#include <algorithm>
#include <experimental/filesystem>
#include <glad/glad.h>

#include <main_app/debug/Log.h>
namespace fs = std::experimental::filesystem;

namespace Utils
{
std::string GetFileExtension(const std::string &strFilePath)
{
    std::error_code errorCode;
    fs::path filePath(strFilePath);
    std::string strExtension = "";

    if(fs::exists(strFilePath, errorCode) &&
            (fs::is_regular_file(strFilePath, errorCode) ||
             fs::is_block_file(strFilePath, errorCode) ||
             fs::is_character_file(strFilePath, errorCode)))
    {
        strExtension = filePath.extension().string();
    }
    else
    {
        strExtension = "";
    }

    return strExtension;
}

bool GetDirectoryFiles(std::string strDirPath, std::vector<std::string> &vecStrFiles)
{
    std::error_code errorCode;
    fs::path folderPath(strDirPath);
    if(!fs::exists(strDirPath, errorCode) ||
            !fs::is_directory(folderPath, errorCode))
    {
        LOG_ERROR("Utils::GetDirectoryFiles(): Cannot create animation from non-existing folder: \"{0}\"",
                  strDirPath);
        return false;
    }

    size_t nFilesCount = std::distance(fs::directory_iterator(folderPath, errorCode),
                                       fs::directory_iterator{});
    std::vector<fs::path> vecOrderedPaths;
    if(nFilesCount > 0)
    {
        vecOrderedPaths.resize(nFilesCount);
    }

    std::copy(fs::directory_iterator(folderPath, errorCode),
              fs::directory_iterator{}, std::back_inserter(vecOrderedPaths));
    std::sort(vecOrderedPaths.begin(), vecOrderedPaths.end());

    std::vector<std::string> vecValidFileExtensions = {".png", ".dds"};
    for(const auto& path : vecOrderedPaths)
    {
        const auto& strFilePath = path.string();
        const auto& strFileExtension = Utils::GetFileExtension(strFilePath);
        for(const auto& strCurrentExtension: vecValidFileExtensions)
        {
            if(strFileExtension.compare(strCurrentExtension) == 0)
            {
                vecStrFiles.emplace_back(strFilePath);
                LOG_INFO("Reading File ...: \"{0}\"",
                          strDirPath);
                break;
            }
        }
    }
    return true;
}

bool CheckOpenGLError()
{
    bool isErrorFound = false;
    GLuint unGLError = glGetError();
    static auto ErrorAsString= [](GLuint unGLError)
    {
        static std::string strError;
        switch (unGLError) {
        case GL_NO_ERROR:
            strError = "GL_NO_ERROR: No error has been recorded. The value "
                       "of this symbolic constant is guaranteed to be 0.";
            break;
        case GL_INVALID_ENUM:
            strError = "GL_INVALID_ENUM: An unacceptable value is specified"
                       " for an enumerated argument. The offending command "
                       "is ignored and has no other side effect than to set"
                       " the error flag.";
            break;
        case GL_INVALID_VALUE:
            strError = "GL_INVALID_VALUE: A numeric argument is out of "
                       "range. The offending command is ignored and has no "
                       "other side effect than to set the error flag.";
            break;
        case GL_INVALID_OPERATION:
            strError = "GL_INVALID_OPERATION: The specified operation is "
                       "not allowed in the current state. The offending "
                       "command is ignored and has no other side effect "
                       "than to set the error flag.";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            strError = "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer "
                       "object is not complete. The offending command is "
                       "ignored and has no other side effect than to set "
                       "the error flag.";
            break;
        case GL_OUT_OF_MEMORY:
            strError = "GL_OUT_OF_MEMORY: There is not enough memory left "
                       "to execute the command. The state of the GL is "
                       "undefined, except for the state of the error flags,"
                       " after this error is recorded.";
            break;
        case GL_STACK_UNDERFLOW:
            strError = "GL_STACK_UNDERFLOW: An attempt has been made to "
                       "perform an operation that would cause an internal "
                       "stack to underflow.";
            break;
        case GL_STACK_OVERFLOW:
            strError = "GL_STACK_OVERFLOW: An attempt has been made to "
                       "perform an operation that would cause an internal "
                       "stack to overflow.";
            break;
        default:
            break;
        }
        return strError;
    };

    while(unGLError != GL_NO_ERROR)
    {
        LOG_ERROR("CheckOpenGLError(): \"{0}\", message: \"{1}\"",
                  unGLError,  ErrorAsString(unGLError));
        isErrorFound = true;
        unGLError = glGetError();
    }
    return isErrorFound;
}

std::string ProcessProgamName(const std::string& strProgramName)
{
    std::string strFormattedName = strProgramName;
    size_t nSlashPos = strProgramName.find_first_of("/");
    if(nSlashPos != std::string::npos)
    {
        strFormattedName = strFormattedName.substr(nSlashPos + 1,
                                                   strFormattedName.length() - nSlashPos - 1);
    }

    return strFormattedName;
}

}
