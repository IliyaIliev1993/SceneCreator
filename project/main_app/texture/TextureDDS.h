#ifndef TEXTURE_DDS_H_
#define TEXTURE_DDS_H_

#include <main_app/texture/Texture.h>
#include <main_app/texture/DDSFormat.h>

class TextureDDS : public Texture
{
public:
    TextureDDS(const std::string& strPath = "");
    virtual bool Load(const std::string& strPath) override;
private:
    bool CreateAndBind(std::ifstream& fileStream);
private:
	DDS_HEADERS m_headers;
};
#endif // TEXTURE_DDS_H
