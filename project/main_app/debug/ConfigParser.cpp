#include "ConfigParser.h"
#include <fstream>
#include <sstream>

#include <main_app/debug/Log.h>

namespace Utils
{
bool ConfigParser::Init(const std::string& strFilePath)
{
    LOG_INFO("ConfiParser Loding JSON Configuration from \"{0}\"",
             strFilePath);
    OptionalDoc optDoc = Load(strFilePath);
    if(!optDoc)
    {
        return false;
    }

    OptionalString optTitle = GetFieldString(optDoc.value(), "title");
    if(!optTitle)
    {
        return false;
    }

    OptionalObject optScreenResolution = GetFieldObject(optDoc.value(),
                                                        "screen_resolution");
    if(!optScreenResolution)
    {
        return false;
    }

    OptionalUInt optWidth = GetFieldUInt(optScreenResolution.value(), "width");
    if(!optWidth)
    {
        return false;
    }

    OptionalUInt optHeight = GetFieldUInt(optScreenResolution.value(), "height");
    if(!optHeight)
    {
        return false;
    }

    OptionalUInt optDepth = GetFieldUInt(optScreenResolution.value(), "depth");
    if(!optDepth)
    {
        return false;
    }

    OptionalString optXMLSavePath = GetFieldString(optDoc.value(), "xml_save_path");
    if(!optXMLSavePath)
    {
        return false;
    }

    m_data.strTitle = optTitle.value();
    m_data.screenResolution.nWidth = optWidth.value();
    m_data.screenResolution.nHeight = optHeight.value();
    m_data.screenResolution.nDepth = optDepth.value();
    m_data.strXMLSavePath = optXMLSavePath.value();

    std::stringstream streamMsg;
    streamMsg << "ConfigData:\n{\n\t\"title\" : \""
                << m_data.strTitle << "\",\n"
                << "\t\"screen_resolution\" : {\n"
                << "\t\t\"width\" : "
                << m_data.screenResolution.nWidth << ",\n"
                << "\t\t\"height\" : "
                << m_data.screenResolution.nHeight << ",\n"
                << "\t\t\"depth\" : "
                << m_data.screenResolution.nDepth << "\n\t},\n"
                << "\t\"xml_save_path\" : \""
                << m_data.strXMLSavePath << "\"\n}";
    LOG_TRACE(streamMsg.str());
    return true;
}

OptionalDoc ConfigParser::Load(const std::string &strFilePath)
{
    std::ifstream fileStream(strFilePath);
    if(!fileStream.is_open())
    {
        LOG_ERROR("ConfigParser File \"{0}\" not found!", strFilePath);
        return {};
    }

    std::stringstream streamData;
    streamData << fileStream.rdbuf();
    std::string strContents = streamData.str();

    rapidjson::StringStream jsonStream(strContents.c_str());
    rapidjson::Document jsonDoc;
    jsonDoc.ParseStream(jsonStream);

    if(!jsonDoc.IsObject())
    {
        LOG_ERROR("ConfigParser File \"{0}\" is not a valid JSON!",
                  strFilePath);
        return {};
    }

    return jsonDoc;
}

OptionalString ConfigParser::GetFieldString(const rapidjson::Document& jsonDoc,
                                            const std::string& strFieldName)
{
    if(!jsonDoc.HasMember(strFieldName.c_str()))
    {
        LOG_ERROR("ConfigParser Document doesn't have a string field \"{0}\"",
                  strFieldName);
        return {};
    }
    const auto& jsonField = jsonDoc[strFieldName.c_str()];
    if(!jsonField.IsString())
    {
        LOG_ERROR("ConfigParser Document has an invalid string field \"{0}\"!",
                  strFieldName);
        return {};
    }
    return jsonField.GetString();
}

OptionalString ConfigParser::GetFieldString(const rapidjson::Document::ConstObject& jsonObject,
                                            const std::string& strFieldName)
{
    if(!jsonObject.HasMember(strFieldName.c_str()))
    {
        LOG_ERROR("ConfigParser Object doesn't have a string field \"{0}\"",
                  strFieldName);
        return {};
    }

    const auto& jsonField = jsonObject[strFieldName.c_str()];
    if(!jsonField.IsString())
    {
        LOG_ERROR("ConfigParser Object has an invalid string field \"{0}\"!",
                  strFieldName);
        return {};
    }

    return jsonField.GetString();
}

OptionalUInt ConfigParser::GetFieldUInt(const rapidjson::Document& jsonDoc,
                                      const std::string& strFieldName)
{
    if(!jsonDoc.HasMember(strFieldName.c_str()))
    {
        LOG_ERROR("ConfigParser Document doesn't have an unsigned int field \"{0}\"",
                  strFieldName);
        return {};
    }

    const auto& jsonField = jsonDoc[strFieldName.c_str()];
    if(!jsonField.IsInt())
    {
        LOG_ERROR("ConfigParser Document has an invalid unsigned int field \"{0}\"!",
                  strFieldName);
        return {};
    }
    return jsonField.GetUint();
}

OptionalUInt ConfigParser::GetFieldUInt(const rapidjson::Document::ConstObject& jsonObject,
                                      const std::string& strFieldName)
{
    if(!jsonObject.HasMember(strFieldName.c_str()))
    {
        LOG_ERROR("ConfigParser Object doesn't have an unsigned int field \"{0}\"",
                  strFieldName);
        return {};
    }

    const auto& jsonField = jsonObject[strFieldName.c_str()];
    if(!jsonField.IsUint())
    {
        LOG_ERROR("ConfigParser Object has an invalid unsigned int field \"{0}\"!",
                  strFieldName);
        return {};
    }

    return jsonField.GetUint();
}

OptionalObject ConfigParser::GetFieldObject(const rapidjson::Document& jsonDoc,
                                       const std::string& strFieldName)
{
    if(!jsonDoc.HasMember(strFieldName.c_str()))
    {
        LOG_ERROR("ConfigParser Document doesn't have an object field \"{0}\"",
                  strFieldName);
        return {};
    }

    const auto& jsonField = jsonDoc[strFieldName.c_str()];
    if(!jsonDoc["screen_resolution"].IsObject())
    {
        LOG_ERROR("ConfigParser Document has an invalid object field {0}!",
                  strFieldName);
        return {};
    }
    return jsonField.GetObject();
}

}
