#include <main_app/texture/TextureDDS.h>
#include <GL/glext.h>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include <main_app/debug/Utils.h>
#include <main_app/debug/Log.h>

TextureDDS::TextureDDS(const std::string& strPath)
    : Texture(strPath),
      m_headers{}
{
    SetFlipFormat(EFlipFormat::eFlipVertical);
    LOG_INFO("Creating a TextureDDS with path: \"{0}\"", strPath);
}

bool TextureDDS::Load(const std::string& strPath)
{
    m_strPath = strPath;
    std::ifstream ddsfile {strPath,
	                       std::ios::binary | std::ios::in | std::ios::ate};

    auto const nFileSize = ddsfile.tellg();
    auto const nChunkSize = sizeof(DDS_HEADER) + sizeof(uint32_t);

    if (nFileSize < nChunkSize)
	{
        LOG_ERROR("TextureDDS: \"{0}\" isn't a valid DDS file!", strPath);
        return false;
	}

	ddsfile.seekg(0);
    uint32_t un32MagicNumber;
    if (!(ddsfile.read(reinterpret_cast<char *>(&un32MagicNumber),
                       sizeof(un32MagicNumber)) && (un32MagicNumber == DDS_MAGIC)))
	{
        LOG_ERROR("TextureDDS: \"{0}\" isn't a valid DDS file!", strPath);
        return false;
	}

	DDS_HEADER header = {};
	ddsfile.read(reinterpret_cast<char *>(&header), sizeof(DDS_HEADER));

    /* Read the additional DX10 header, if applicable and available */
	DDS_HEADER_DXT10 ddsDX10Header = {};
	if ((header.ddspf.dwFourCC == DX10) &&
        (nFileSize >= (nChunkSize + sizeof(DDS_HEADER_DXT10))))
	{
		ddsfile.read(reinterpret_cast<char *>(&ddsDX10Header),
		             sizeof(ddsDX10Header));
	}

	m_headers = {header, ddsDX10Header};

    if(!CreateAndBind(ddsfile))
    {
        LOG_ERROR("TextureDDS: Cannot load texture \"{0}\"!", strPath);
        return false;
    }
    return true;
}

bool TextureDDS::CreateAndBind(std::ifstream& fileStream)
{
	if(m_headers.header.ddspf.dwFlags & DDPF_FOURCC)
	{
        int nBlockSize = 0;
        GLenum glFormat = GL_FALSE;

		switch(m_headers.header.ddspf.dwFourCC)
		{
		case DXT1:
		{
            nBlockSize = 8;
			glFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		}
		break;

		case DXT2:
		{
            nBlockSize = 16;
		}
		break;

		case DXT3:
		{
            nBlockSize = 16;
			glFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		}
		break;

		case DXT4:
		{
            nBlockSize = 16;
		}
		break;

		case DXT5:
		{
            nBlockSize = 16;
			glFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
		break;

		case DX10:
		{
            nBlockSize = 16;
			switch(m_headers.header10.dxgiFormat)
			{
			case DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM:
				glFormat = GL_COMPRESSED_RGBA_BPTC_UNORM;
				break;
			default:
                LOG_ERROR("TextureDDS: \"{0}\" - Unknown DXGI format!", m_strPath);
                return false;
			}
		}
		break;
		}

        m_nWidth = m_headers.header.dwWidth;
        m_nHeight = m_headers.header.dwHeight;

        m_nOriginalWidth = m_nWidth;
        m_nOriginalHeight = m_nHeight;

        SetSourceW(m_nWidth);
        SetSourceH(m_nHeight);

        int nTotalSize = std::max(1, ((m_nWidth + 3) / 4)) *
            std::max(1, ((m_nHeight + 3) / 4)) * nBlockSize;

        std::vector<char> vecData(nTotalSize);

        fileStream.read(&vecData[0], nTotalSize);

        /* Create a OpenGL texture identifier */
        glGenTextures(1, &m_unTextureID);
        glBindTexture(GL_TEXTURE_2D, m_unTextureID);

        /* Setup filtering parameters for display */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glCompressedTexImage2D(
            GL_TEXTURE_2D, 0, glFormat, m_nWidth, m_nHeight, 0, nTotalSize, &vecData[0]);
        fileStream.close();

        if(Utils::CheckOpenGLError())
        {
            return false;
        }

        LOG_INFO("Loaded TextureDDS: \"{0}\"", m_strPath);
        return true;
	}

    return false;
}
