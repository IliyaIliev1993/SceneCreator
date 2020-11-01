#pragma once

#include <cstdint>

/**
  DDS File Layout:
    A DDS file is a binary file that contains the following information:
    -> A DWORD (magic number) containing the four character code value 'DDS ' (0x20534444).
    -> A description of the data in the file.

    The data is described with a header description using DDS_HEADER;
    the pixel format is defined using DDS_PIXEL_FORMAT
    {
        DWORD               dwMagic;
        DDS_HEADER          header;
    }

    If the DDS_PIXEL_FORMAT dwFlags is set to DDPF_FOURCC and dwFourCC is set to
    "DX10" an additional DDS_HEADER_DXT10 structure will be present to
    accommodate texture arrays or DXGI formats that cannot be expressed as an
    RGB pixel foramt such as floating point formats, sRGB formats etc. When the
    DDS_HEADER_DXT10 structure is present the entire data description will looks
    like this:
    {
        DWORD               dwMagic;
        DDS_HEADER          header;
        DDS_HEADER_DXT10    header10;
    }

    -> A pointer to an array of bytes that contains the main surface data:
    {
        BYTE bdata[]
    }
    -> A pointer to an array of bytes that contains the remaining surfaces such as;
    mipmap levels, faces in a cube map, depths in a volume texture:
    {
        BYTE bdata2[]
    }
===============================================================================
  The D3DX library (for example, D3DX11.lib) and other similar libraries
    unreliably or inconsistently provide the pitch value in the
    dwPitchOrLinearSize member of the DDS_HEADER structure. Therefore, when you
    read and write to DDS files, we recommend that you compute the pitch in one
    of the following ways for the indicated formats:

    For block-compressed formats, compute the pitch as:
    max( 1, ((width+3)/4) ) * block-size
    The block-size is 8 bytes for DXT1, BC1, and BC4 formats, and 16 bytes for
    other block-compressed formats.

    For R8G8_B8G8, G8R8_G8B8, legacy UYVY-packed, and legacy YUY2-packed formats,
    compute the pitch as:
    ((width+1) >> 1) * 4

    For other formats, compute the pitch as:
    ( width * bits-per-pixel + 7 ) / 8
    You divide by 8 for byte alignment.

    Note:
    The pitch value that you calculate does not always equal the pitch that the
    runtime supplies, which is DWORD-aligned in some situations and byte-aligned
    in other situations. Therefore, we recommend that you copy a scan line at a
    time rather than try to copy the whole image in one copy.
===============================================================================
  DDS Variants:
    There are many tools that create and consume DDS files, but they can vary in
    the details of what they require in the header. Writers should populate the
    headers as fully as possible, and readers should check the minimal values
    for maximum compatibilty. To validate a DDS file, a reader should ensure the
    file is at least 128 bytes long to accommodate the magic value and basic
    header, the magic value is 0x20534444 ("DDS "), the DDS_HEADER size is 124,
    and the DDS_PIXEL_FORMAT in the header size is 32. If the DDS_PIXEL_FORMAT
    dwFlags is set to DDPF_FOURCC and a dwFourCC is set to "DX10", then the total
    file size needs to be at least 148 bytes.

    There are some common variants in use where the pixel format is set to a
    DDPF_FOURCC code where dwFourCC is set to a D3DFORMAT or DXGI_FORMAT
    enumeration value. There is no way to tell if an enumeration value is a
    D3DFORMAT or a DXGI_FORMAT, so it is highly recommended that the "DX10"
    extension and DDS_HEADER_DXT10 header is used instead to store the
    dxgiFormat when the basic DDS_PIXEL_FORMAT cannot express the format.

    The standard DDS_PIXEL_FORMAT should be preferred for maximum compatibilty
    to store RGB uncompressed data and DXT1-5 data as not all DDS tools support
    the DX10 extension.
===============================================================================
  DDS Texture Example:
    For an uncompressed texture, use the DDSD_PITCH and DDPF_RGB flags;
    for a compressed texture, use the DDSD_LINEARSIZE and DDPF_FOURCC flags.
    For a mipmapped texture, use the DDSD_MIPMAPCOUNT, DDSCAPS_MIPMAP, and
    DDSCAPS_COMPLEX flags also as well as the mipmap count member. If mipmaps
    are generated, all levels down to 1-by-1 are usually written.

    For a compressed texture, the size of each mipmap level image is typically
    one-fourth the size of the previous, with a minimum of 8 (DXT1) or 16 (DXT2-5)
    bytes (for square textures). Use the following formula to calculate the size
    of each level for a non-square texture:

    max(1, ( (width + 3) / 4 ) ) x max(1, ( (height + 3) / 4 ) ) x 8(DXT1) or 16(DXT2-5)

    This table lists the amount of space taken up by each layer for the same
    texture using a DXGI compression format (in this case BC7_UNORM) that
    therefore requires the extended header:
    ___________________________________________________________________________
    |           DDS Components                                     |  # Bytes |
    ---------------------------------------------------------------+----------|
    | header (FourCC set to "DX10")                                |    128   |
    | extended header (DXGI format set to DXGI_FORMAT_BC3_UNORM)   |    20    |
    | 256-by-64 main image                                         |    16384 |
    | 128-by-32 mipmap image                                       |    4096  |
    | 64-by-16 mipmap image                                        |    1024  |
    | 32-by-8 mipmap image                                         |    256   |
    | 16-by-4 mipmap image                                         |    64    |
    | 8-by-2 mipmap image                                          |    32    |
    | 4-by-1 mipmap image                                          |    16    |
    | 2-by-1 mipmap image                                          |    16    |
    | 1-by-1 mipmap image                                          |    16    |
    |______________________________________________________________|__________|
 */

using dword = unsigned int;

const dword DDS_MAGIC = 0x20534444;//"DDS "
const dword DDPF_FOURCC = 0x4;
const dword DDPF_RGB = 0x40;

constexpr dword fourCcToDword(const char *value)
{
    return value[0] + (value[1] << 8) + (value[2] << 16) + (value[3] << 24);
}

constexpr dword DXT1 = fourCcToDword("DXT1");
constexpr dword DXT2 = fourCcToDword("DXT2");
constexpr dword DXT3 = fourCcToDword("DXT3");
constexpr dword DXT4 = fourCcToDword("DXT4");
constexpr dword DXT5 = fourCcToDword("DXT5");
constexpr dword DX10 = fourCcToDword("DX10");

/**
 * @brief The D3D10_RESOURCE_DIMENSION enum - Identifies the type of resource
 * being used
 */
enum class D3D10_RESOURCE_DIMENSION : uint8_t
{
    /* Resource is of unknown type. */
	D3D10_RESOURCE_DIMENSION_UNKNOWN = 0,
    /* Resource is a buffer. */
	D3D10_RESOURCE_DIMENSION_BUFFER = 1,
    /* Resource is a 1D texture. The dwWidth member of DDS_HEADER specifies the
     * size of the texture. Typically, you set the dwHeight member of
     * DDS_HEADER to 1; you also must set the DDSD_HEIGHT flag in the dwFlags
     * member of DDS_HEADER. */
	D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
    /* Resource is a 2D texture with an area specified by the dwWidth and
     * dwHeight members of DDS_HEADER. You can also use this type to identify a
     * cube-map texture. For more information about how to identify a cube-map
     * texture, see miscFlag and arraySize members. */
	D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
    /* Resource is a 3D texture with a volume specified by the dwWidth,
     * dwHeight, and dwDepth members of DDS_HEADER. You also must set the
     * DDSD_DEPTH flag in the dwFlags member of DDS_HEADER. */
	D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4,
	Total
};

enum class DXGI_FORMAT : uint32_t
{
	DXGI_FORMAT_UNKNOWN = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	DXGI_FORMAT_R32G32B32_UINT = 7,
	DXGI_FORMAT_R32G32B32_SINT = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	DXGI_FORMAT_R32G32_TYPELESS = 15,
	DXGI_FORMAT_R32G32_FLOAT = 16,
	DXGI_FORMAT_R32G32_UINT = 17,
	DXGI_FORMAT_R32G32_SINT = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	DXGI_FORMAT_R16G16_TYPELESS = 33,
	DXGI_FORMAT_R16G16_FLOAT = 34,
	DXGI_FORMAT_R16G16_UNORM = 35,
	DXGI_FORMAT_R16G16_UINT = 36,
	DXGI_FORMAT_R16G16_SNORM = 37,
	DXGI_FORMAT_R16G16_SINT = 38,
	DXGI_FORMAT_R32_TYPELESS = 39,
	DXGI_FORMAT_D32_FLOAT = 40,
	DXGI_FORMAT_R32_FLOAT = 41,
	DXGI_FORMAT_R32_UINT = 42,
	DXGI_FORMAT_R32_SINT = 43,
	DXGI_FORMAT_R24G8_TYPELESS = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	DXGI_FORMAT_R8G8_TYPELESS = 48,
	DXGI_FORMAT_R8G8_UNORM = 49,
	DXGI_FORMAT_R8G8_UINT = 50,
	DXGI_FORMAT_R8G8_SNORM = 51,
	DXGI_FORMAT_R8G8_SINT = 52,
	DXGI_FORMAT_R16_TYPELESS = 53,
	DXGI_FORMAT_R16_FLOAT = 54,
	DXGI_FORMAT_D16_UNORM = 55,
	DXGI_FORMAT_R16_UNORM = 56,
	DXGI_FORMAT_R16_UINT = 57,
	DXGI_FORMAT_R16_SNORM = 58,
	DXGI_FORMAT_R16_SINT = 59,
	DXGI_FORMAT_R8_TYPELESS = 60,
	DXGI_FORMAT_R8_UNORM = 61,
	DXGI_FORMAT_R8_UINT = 62,
	DXGI_FORMAT_R8_SNORM = 63,
	DXGI_FORMAT_R8_SINT = 64,
	DXGI_FORMAT_A8_UNORM = 65,
	DXGI_FORMAT_R1_UNORM = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	DXGI_FORMAT_BC1_TYPELESS = 70,
	DXGI_FORMAT_BC1_UNORM = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	DXGI_FORMAT_BC2_TYPELESS = 73,
	DXGI_FORMAT_BC2_UNORM = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	DXGI_FORMAT_BC3_TYPELESS = 76,
	DXGI_FORMAT_BC3_UNORM = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	DXGI_FORMAT_BC4_TYPELESS = 79,
	DXGI_FORMAT_BC4_UNORM = 80,
	DXGI_FORMAT_BC4_SNORM = 81,
	DXGI_FORMAT_BC5_TYPELESS = 82,
	DXGI_FORMAT_BC5_UNORM = 83,
	DXGI_FORMAT_BC5_SNORM = 84,
	DXGI_FORMAT_B5G6R5_UNORM = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	DXGI_FORMAT_BC6H_TYPELESS = 94,
	DXGI_FORMAT_BC6H_UF16 = 95,
	DXGI_FORMAT_BC6H_SF16 = 96,
	DXGI_FORMAT_BC7_TYPELESS = 97,
	DXGI_FORMAT_BC7_UNORM = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	DXGI_FORMAT_FORCE_UINT = 0xffffffffU,
	Total = DXGI_FORMAT_B4G4R4A4_UNORM + 2
};

/**
 * @brief The DDS_PIXEL_FORMAT struct - surface pixel format.
 * To store DXGI formats such as floating-point data, use a dwFlags of
 * DDPF_FOURCC and set dwFourCC to 'D','X','1','0'. Use the DDS_HEADER_DXT10
 * extension header to store the DXGI format in the dxgiFormat member.

 * Note that there are non-standard variants of DDS files where dwFlags has
 * DDPF_FOURCC and the dwFourCC value is set directly to a D3DFORMAT or
 * DXGI_FORMAT enumeration value. It is not possible to disambiguate the
 * D3DFORMAT versus DXGI_FORMAT values using this non-standard scheme, so the
 * DX10 extension header is recommended instead.
 */
struct DDS_PIXEL_FORMAT
{
    /**
     * @brief dwSize - Structure size; set to 32 (bytes).
     */
    dword dwSize;
    /**
     * @brief dwFlags - Values which indicate what type of data is in the surface.
    ____________________________________________________________________________
    | Flag              |   Description                         |  Value       |
    |-------------------|---------------------------------------|--------------|
    | DDPF_ALPHAPIXELS  | Texture contains alpha data;          |   0x1        |
    |                   | dwRGBAlphaBitMask contains valid data.|              |
    |                   |                                       |              |
    |-------------------|---------------------------------------|--------------|
    | DDPF_ALPHA        | Used in some older DDS files for      |   0x2        |
    |                   | alpha channel only uncompressed data  |              |
    |                   | (dwRGBBitCount contains the alpha     |              |
    |                   | channel bitcount; dwABitMask contains |              |
    |                   | valid data)                           |              |
    |-------------------|---------------------------------------|--------------|
    | DDPF_FOURCC       | Texture contains compressed RGB data; |   0x4        |
    |                   |  dwFourCC contains valid data.        |              |
    |-------------------|---------------------------------------|--------------|
    | DDPF_RGB          | Texture contains uncompressed RGB     |   0x40       |
    |                   | data; dwRGBBitCount and the RGB masks |              |
    |                   | (dwRBitMask, dwGBitMask, dwBBitMask)  |              |
    |                   | contain valid data.                   |              |
    |-------------------|---------------------------------------|--------------|
    | DDPF_YUV          | Used in some older DDS files for YUV  |   0x200      |
    |                   | uncompressed data (dwRGBBitCount      |              |
    |                   | contains the YUV bit count; dwRBitMask|              |
    |                   | contains the Y mask, dwGBitMask       |              |
    |                   | contains the U mask, dwBBitMask       |              |
    |                   | contains the V mask)                  |              |
    |-------------------|---------------------------------------|--------------|
    | DDPF_LUMINANCE    | Used in some older DDS files for      |  0x20000     |
    |                   | single channel color uncompressed     |              |
    |                   | data (dwRGBBitCount contains the      |              |
    |                   | luminance channel bit count;          |              |
    |                   | dwRBitMask contains the channel mask).|              |
    |                   | Can be combined with DDPF_ALPHAPIXELS |              |
    |                   | for a two channel DDS file.           |              |
    ---------------------------------------------------------------------------|
     */
    dword dwFlags;
    /**
     * @brief dwFourCC - Four-character codes for specifying compressed or
     * custom formats. Possible values include: DXT1, DXT2, DXT3, DXT4, or DXT5.
     * A FourCC of DX10 indicates the prescense of the DDS_HEADER_DXT10 extended
     * header, and the dxgiFormat member of that structure indicates the true
     * format. When using a four-character code, dwFlags must include
     * DDPF_FOURCC.
     */
    dword dwFourCC;
    /**
     * @brief dwRGBBitCount - Number of bits in an RGB (possibly including
     * alpha) format. Valid when dwFlags includes DDPF_RGB, DDPF_LUMINANCE,
     * or DDPF_YUV.
     */
    dword dwRGBBitCount;
    /**
     * @brief dwRBitMask - Red (or lumiannce or Y) mask for reading color data.
     * For instance, given the A8R8G8B8 format, the red mask would be 0x00ff0000.
     */
    dword dwRBitMask;
    /**
     * @brief dwGBitMask - Green (or U) mask for reading color data. For
     * instance, given the A8R8G8B8 format, the green mask would be 0x0000ff00.
     */
    dword dwGBitMask;
    /**
     * @brief dwBBitMask - Blue (or V) mask for reading color data. For
     * instance, given the A8R8G8B8 format, the blue mask would be 0x000000ff.
     */
    dword dwBBitMask;
    /**
     * @brief dwABitMask - Alpha mask for reading alpha data. dwFlags must
     * include DDPF_ALPHAPIXELS or DDPF_ALPHA. For instance, given the A8R8G8B8
     * format, the alpha mask would be 0xff000000.
     */
    dword dwABitMask;
};

/**
 * @brief The DDS_HEADER struct - Describes a DDS file header.
    Remarks
    Include flags in dwFlags for the members of the structure that
    contain valid data.

    Use this structure in combination with a DDS_HEADER_DXT10 to store a
    resource array in a DDS file. For more information, see texture arrays.
 */
struct DDS_HEADER
{
    /**
     * @brief dwSize - Size of structure. This member must be set to 124.
     */
    dword dwSize;
    /**
     * @brief dwFlags - Flags to indicate which members contain valid data.
    ________________________________________________________________________
    | Flag             |   Description                         |  Value     |
    |------------------|---------------------------------------|------------|
    | DDSD_CAPS        |   Required in every .dds file.        |  0x1       |
    |-----------------------------------------------------------------------|
    | DDSD_HEIGHT      |   Required in every .dds file.        |  0x2       |
    |------------------|---------------------------------------|------------|
    | DDSD_WIDTH       |   Required in every .dds file.        |  0x4       |
    |------------------|---------------------------------------|------------|
    | DDSD_PITCH       |   Required when pitch is provided for |            |
    |                  |   an uncompressed texture.            |  0x8       |
    |------------------|---------------------------------------|------------|
    | DDSD_PIXELFORMAT |   Required in every .dds file.        |  0x1000    |
    |------------------|---------------------------------------|------------|
    | DDSD_MIPMAPCOUNT |   Required in a mipmapped texture.    |  0x20000   |
    |------------------|---------------------------------------|------------|
    | DDSD_LINEARSIZE  |   Required when pitch is provided for |            |
    |                  |   a compressed texture.               |  0x80000   |
    |------------------|---------------------------------------|------------|
    | DDSD_DEPTH       |   Required in a depth texture.        |  0x800000  |
    -------------------------------------------------------------------------
    Note: When you write .dds files, you should set the DDSD_CAPS and
    DDSD_PIXELFORMAT flags, and for mipmapped textures you should also set the
    DDSD_MIPMAPCOUNT flag. However, when you read a .dds file, you should not
    rely on the DDSD_CAPS, DDSD_PIXELFORMAT, and DDSD_MIPMAPCOUNT flags being
    set because some writers of such a file might not set these flags.
     */
    dword dwFlags;
    /**
     * @brief dwHeight - Surface height (in pixels).
     */
    dword dwHeight;
    /**
     * @brief dwWidth - Surface width (in pixels).
     */
    dword dwWidth;
    /**
     * @brief dwPitchOrLinearSize - The pitch or number of bytes per scan line
     * in an uncompressed texture; the total number of bytes in the top level
     * texture for a compressed texture.
     */
    dword dwPitchOrLinearSize;
    /**
     * @brief dwDepth - Depth of a volume texture (in pixels), otherwise unused.
     */
    dword dwDepth;
    /**
     * @brief dwMipMapCount - Number of mipmap levels, otherwise unused.
     */
    dword dwMipMapCount;
    /**
     * @brief dwReserved1 - Unused.
     */
    dword dwReserved1[11];
    /**
     * @brief ddspf - The pixel format (see DDS_PIXEL_FORMAT).
     */
    DDS_PIXEL_FORMAT ddspf;
    /**
     * @brief dwCaps - Specifies the complexity of the surfaces stored.
     __________________________________________________________________________
    | Flag             |   Description                            |  Value     |
    |------------------|------------------------------------------|------------|
    | DDSCAPS_COMPLEX  |   Optional; must be used on any file     |            |
    |                  |   that contains more than one surface    |            |
    |                  |   (a mipmap, a cubic environment map,    |            |
    |                  |   or mipmapped volume texture).          |  0x8       |
    |------------------|------------------------------------------|------------|
    | DDSCAPS_MIPMAP   |   Optional; should be used for a mipmap. |  0x400000  |
    |------------------|------------------------------------------|------------|
    | DDSCAPS_TEXTURE  |   Required.                              |  0x1000    |
    ----------------------------------------------------------------------------
    Note:
    When you write .dds files, you should set the DDSCAPS_TEXTURE flag, and for
    multiple surfaces you should also set the DDSCAPS_COMPLEX flag. However,
    when you read a .dds file, you should not rely on the DDSCAPS_TEXTURE and
    DDSCAPS_COMPLEX flags being set because some writers of such a file might
    not set these flags.
     */
    dword dwCaps;
    /**
     * @brief dwCaps2 - Additional detail about the surfaces stored.
    ____________________________________________________________________________
    | Flag                       |   Description                  |  Value     |
    |----------------------------|--------------------------------|------------|
    | DDSCAPS2_CUBEMAP           |  Required for a cube map.      |   0x200    |
    |----------------------------|--------------------------------|------------|
    | DDSCAPS2_CUBEMAP_POSITIVEX |  Required when these surfaces  |            |
    |                            |  are stored in a cube map.     |   0x400    |
    |----------------------------|--------------------------------|------------|
    | DDSCAPS2_CUBEMAP_NEGATIVEX |  Required when these surfaces  |            |
    |                            |  are stored in a cube map.     |   0x800    |
    |----------------------------|--------------------------------|------------|
    | DDSCAPS2_CUBEMAP_POSITIVEY |  Required when these surfaces  |            |
    |                            |  are stored in a cube map.     |   0x1000   |
    |----------------------------|--------------------------------|------------|
    | DDSCAPS2_CUBEMAP_NEGATIVEY |  Required when these surfaces  |            |
    |                            |  are stored in a cube map.     |   0x2000   |
    |----------------------------|--------------------------------|------------|
    | DDSCAPS2_CUBEMAP_POSITIVEZ |  Required when these surfaces  |            |
    |                            |  are stored in a cube map.     |   0x4000   |
    |----------------------------|--------------------------------|------------|
    | DDSCAPS2_CUBEMAP_NEGATIVEZ |  Required when these surfaces  |            |
    |                            |  are stored in a cube map.     |   0x8000   |
    |----------------------------|--------------------------------|------------|
    | DDSCAPS2_VOLUME            |  Required for a volume texture.|  0x200000  |
    ----------------------------------------------------------------------------
     */
    dword dwCaps2;
    /**
     * @brief dwCaps3 - Unused.
     */
    dword dwCaps3;
    /**
     * @brief dwCaps4 - Unused.
     */
    dword dwCaps4;
    /**
     * @brief dwReserved2 - Unused.
     */
    dword dwReserved2;
};

/**
 * @brief The DDS_HEADER_DXT10 struct - DDS header extension to handle resource
 * arrays, DXGI pixel formats that don't map to the legacy Microsoft DirectDraw
 * pixel format structures, and additional metadata.
 *
 * This header is present if the dwFourCC member of the DDS_PIXEL_FORMAT
 * structure is set to 'DX10'.
 */
struct DDS_HEADER_DXT10
{
    /**
     * @brief dxgiFormat - The surface pixel format.
     */
    DXGI_FORMAT dxgiFormat;
    /**
     * @brief resourceDimension - Identifies the type of resource.
     */
    D3D10_RESOURCE_DIMENSION resourceDimension;
    /**
     * @brief miscFlag - Identifies other, less common options for resources.
     */
    unsigned int miscFlag;
    /**
     * @brief arraySize - The number of elements in the array.
       For a 2D texture that is also a cube-map texture, this number represents
       the number of cubes. This number is the same as the number in the
       NumCubes member of D3D10_TEXCUBE_ARRAY_SRV1 or D3D11_TEXCUBE_ARRAY_SRV).
       In this case, the DDS file contains arraySize*6 2D textures. For more
       information about this case, see the miscFlag description.
     */
    unsigned int arraySize;
    /**
     * @brief miscFlags2 - Contains additional metadata (formerly was reserved).
     * The lower 3 bits indicate the alpha mode of the associated resource.
     * The upper 29 bits are reserved and are typically 0.
     *
    _____________________________________________________________________________
    | Flag                          |   Description                      | Value|
    |-------------------------------+------------------------------------+------|
    | DDS_ALPHA_MODE_UNKNOWN        | Alpha channel content is unknown.  | 0x00 |
    |                               | This is the value for legacy files,|      |
    |                               | which typically is assumed to be   |      |
    |                               | 'straight' alpha.                  |      |
    |-------------------------------+------------------------------------+------|
    | DDS_ALPHA_MODE_STRAIGHT       | Any alpha channel content is       | 0x01 |
    |                               | presumed to use straight alpha.    |      |
    |-------------------------------+------------------------------------+------|
    | DDS_ALPHA_MODE_PREMULTIPLIED  | Any alpha channel content is using | 0x02 |
    |                               | premultiplied alpha. The only      |      |
    |                               | legacy file formats that indicate  |      |
    |                               | this information are 'DX2' and     |      |
    |                               | 'DX4'.                             |      |
    |-------------------------------+------------------------------------+------|
    | DDS_ALPHA_MODE_OPAQUE         | Any alpha channel content is all   | 0x03 |
    |                               | set to fully opaque.               |      |
    |-------------------------------+------------------------------------+------|
    | DDS_ALPHA_MODE_CUSTOM         | Any alpha channel content is being | 0x04 |
    |                               | used as a 4th channel and is not   |      |
    |                               | intended to represent transparency |      |
    |                               | (straight or premultiplied).       |      |
    --------------------------------+------------------------------------+-------
     */
    unsigned int miscFlags2;
};

struct DDS_HEADERS
{
	DDS_HEADER header;
	DDS_HEADER_DXT10 header10;
};
