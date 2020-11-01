#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>

namespace Utils
{
std::string GetFileExtension(const std::string& strFilePath);
/**
 * @brief GetDirectoryFiles - return a list of all files, contained in the
 * directory.
 * @param strDirPath - path to directory which to search for files.
 * @param vecStrFiles - an input parameter which will be filled with all the
 * files found in strDirPath.
 * @return true if the files were retrieved successfully.
 */
bool GetDirectoryFiles(std::string strDirPath,
                        std::vector<std::string>& vecStrFiles);
/**
 * @brief CheckOpenGLError - Check if an OpenGL error occurred.
 * @return true if an OpenGL error has occurred.
 */
bool CheckOpenGLError();

/**
 * @brief ProcessProgamName - format the current program name from the command
 * lint
 * @param strProgramName - name of the current program
 * @return - the name, formatted without the ./ prefix
 */
std::string ProcessProgamName(const std::string& strProgramName);

/**
 * @brief ToNum : convert an enum or enum class to an integer value.
 */
template<typename T, std::enable_if_t<std::is_enum<T>::value && !std::is_convertible<T, int>::value>* = nullptr>
constexpr typename std::underlying_type<T>::type ToNum(T val) {
    return static_cast<typename std::underlying_type<T>::type>(val);
}
}

#endif // UTILS_H
