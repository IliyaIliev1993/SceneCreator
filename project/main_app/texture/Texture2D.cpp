#include <main_app/texture/Texture2D.h>

#include <sstream>
#include <iostream>
#include <main_app/debug/Log.h>

Texture2D::Texture2D(const std::string& strPath)
    : Texture(strPath)
{
    LOG_INFO("Creating a Texture2D with path: \"{0}\"", strPath);
}
