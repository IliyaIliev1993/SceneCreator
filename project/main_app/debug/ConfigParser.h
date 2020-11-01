#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <rapidjson/document.h>
#include <experimental/optional>

namespace std
{
    template<class T>
    using optional = std::experimental::optional<T>;
}

namespace Utils
{
/** Optional Init */
using OptionalUInt = std::optional<unsigned int>;
/** Optional String */
using OptionalString = std::optional<std::string>;
/* Optional JSON Object */
using OptionalObject = std::optional<rapidjson::Document::ConstObject>;
/* Optional JSON Document */
using OptionalDoc = std::optional<rapidjson::Document>;

/**
 * @brief The ConfigParser class - read a JSON configuration file and use it
 * to initialize this application.
 */
class ConfigParser
{
public:
    /**
     * @brief The Data struct - data container for the contents of the
     * config.json file.
     */
    struct Data
    {
        /**
         * @brief The ScreenResolution struct - corresponds to the
         * "screen_resoulution" field in config.json. Holds information about
         * the screen resolution.
         */
        struct ScreenResolution
        {
            /**
             * @brief nWidth - the width of the window. Corresponds to the
             * "width" field of "screen_resolution".
             * Note: Must be a positive value!
             */
            unsigned int nWidth = 0;
            /**
             * @brief nHeight - the heignt of the window. Corresponds to the
             * "height" field of "screen_resolution".
             * Note: Must be a positive value!
             */
            unsigned int nHeight = 0;
            /**
             * @brief nDepth - the depth of the window. Corresponds to the
             * "depth" field of "screen_resolution".
             * Note: Must be a positive value!
             */
            unsigned int nDepth = 0;
        };
        /**
         * @brief strTitle - the title of the window. Corresponds to the "title"
         * field in config.json.
         */
        std::string strTitle = "";
        /**
         * @brief screenResolution - holds information about the screen
         * resolution of the window.
         */
        ScreenResolution screenResolution;
        /**
         * @brief strXMLSavePath - default save path for an .xml resources file.
         * Corresponds to the "xml_save_path" field in config.json.
         */
        std::string strXMLSavePath = "";
    };

    /** Default constructor */
    ConfigParser() = default;
    /**
     * @brief Init - initialize the Data structure from the contents of the
     * config.json file.
     * @param strFilePath - location to a *.json configuration file, holding
     * the relevant settings for this application.
     * @return if the initialization was successful.
     */
    bool Init(const std::string& strFilePath);
    /**
     * @brief GetData - return the current settings for the application.
     * @return m_data.
     */
    const Data& GetData() const { return m_data; }
private:
    /**
     * @brief Load - load a *.json configuration file.
     * @param strFilePath - path to the JSON configuration file.
     * @return a valid JSON document object, holding all the fields that were
     * read, if the initialization was
     * successful.
     */
    OptionalDoc Load(const std::string& strFilePath);
    /**
     * @brief GetFieldString - return the value of a string field from a JSON
     * document.
     * @param jsonDoc - JSON document object, holding all the fields of the
     * JSON configuration file.
     * @param strFieldName - name of the field to be retrieved.
     * @return a valid string value, if the field was found.
     */
    OptionalString GetFieldString(const rapidjson::Document& jsonDoc,
                        const std::string& strFieldName);
    /**
     * @brief GetFieldString - return the value of a string field from a plain
     * JSON object.
     * @param jsonObject - a plain JSON object, holding the field we need.
     * @param strFieldName - name of the field to be retrieved.
     * @return a valid string value, if the field was found.
     */
    OptionalString GetFieldString(const rapidjson::Document::ConstObject& jsonObject,
                        const std::string& strFieldName);
    /**
     * @brief GetFieldUInt - return the value of an unsigned integer field from
     * a JSON document.
     * @param jsonDoc - JSON document object, holding all the fields of the
     * JSON configuration file.
     * @param strFieldName - name of the field to be retrieved.
     * @return a valid unsigned integer value, if the field was found.
     */
    OptionalUInt GetFieldUInt(const rapidjson::Document& jsonDoc,
                            const std::string& strFieldName);
    /**
     * @brief GetFieldUInt - return the value of an unsigned integer field from
     * a JSON document.
     * @param jsonObject - a plain JSON object, holding the field we need.
     * @param strFieldName - name of the field to be retrieved.
     * @return a valid unsigned integer value, if the field was found.
     */
    OptionalUInt GetFieldUInt(const rapidjson::Document::ConstObject& jsonObject,
                            const std::string& strFieldName);
    /**
     * @brief GetFieldString - return a JSON Object, so we can examine its fields.
     * @param jsonDoc - JSON document object, holding all the fields of the
     * JSON configuration file.
     * @param strFieldName - name of the field Object to be retrieved.
     * @return a valid JSON Object, if the field was found.
     */
    OptionalObject GetFieldObject(const rapidjson::Document& jsonDoc,
                   const std::string& strFieldName);
private:
    /**
     * @brief m_data - current configuration settings for the application.
     */
    Data m_data;
};
}

#endif // CONFIGPARSER_H
