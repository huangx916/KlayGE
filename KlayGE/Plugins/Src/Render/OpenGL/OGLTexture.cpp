#include <KlayGE/KlayGE.hpp>
#include <KlayGE/ThrowErr.hpp>
#include <KlayGE/Memory.hpp>
#include <KlayGE/RenderEngine.hpp>
#include <KlayGE/RenderFactory.hpp>
#include <KlayGE/Texture.hpp>

#include <gl/gl.h>

#include <KlayGE/OpenGL/OGLTexture.hpp>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

namespace
{
	using namespace KlayGE;

	void Convert(GLint& intenalFormat, GLenum& glformat, KlayGE::PixelFormat pf)
	{
		switch (pf)
		{
		case PF_L8:
			intenalFormat = GL_LUMINANCE8;
			glformat = GL_LUMINANCE;
			break;

		case PF_A8:
			intenalFormat = GL_ALPHA8;
			glformat = GL_ALPHA;
			break;

		case PF_A4L4:
			intenalFormat = GL_LUMINANCE4_ALPHA4;
			glformat = GL_LUMINANCE_ALPHA;
			break;

		case PF_R5G6B5:
			intenalFormat = GL_RGB5;
			glformat = GL_BGR;
			break;

		case PF_A4R4G4B4:
			intenalFormat = GL_RGBA4;
			glformat = GL_BGRA;
			break;

		case PF_X8R8G8B8:
			intenalFormat = GL_RGB8;
			glformat = GL_BGR;
			break;

		case PF_A8R8G8B8:
			intenalFormat = GL_RGBA8;
			glformat = GL_BGRA;
			break;

		case PF_A2R10G10B10:
			intenalFormat = GL_RGB10_A2;
			glformat = GL_BGRA;
			break;
		}
	}
}

namespace KlayGE
{
	OGLTexture::OGLTexture(U32 width, U32 height,
								U16 numMipMaps, PixelFormat format, TextureUsage usage)
								: texture_(0)
	{
		numMipMaps_ = numMipMaps;
		format_		= format;
		width_		= width;
		height_		= height;

		bpp_ = PixelFormatBits(format);

		usage_ = usage;
	}

	OGLTexture::~OGLTexture()
	{
	}

	const std::wstring& OGLTexture::Name() const
	{
		static std::wstring name(L"OpenGL Texture");
		return name;
	}

	void OGLTexture::CopyToTexture(Texture& target)
	{
		OGLTexture& other(static_cast<OGLTexture&>(target));
		glCopyTexImage2D(other.GLTexture(), 0, 3, 0, 0, other.Width(), other.Height(), 0);
	}

	void OGLTexture::CopyMemoryToTexture(void* data, PixelFormat pf,
		U32 width, U32 height, U32 xOffset, U32 yOffset)
	{
		U16 bpp(PixelFormatBits(pf));

		if (0 == width)
		{
			width = width_;
		}
		if (0 == height)
		{
			height = height_;
		}

		GLint glinternalFormat;
		GLenum glformat;
		Convert(glinternalFormat, glformat, pf);

		if ((0 == texture_) || (width > width_) || (height > height_) || (pf != format_))
		{
			if (texture_ != 0)
			{
				glDeleteTextures(1, &texture_);
			}

			glGenTextures(1, &texture_);
			glBindTexture(GL_TEXTURE_2D, texture_);

			glTexImage2D(GL_TEXTURE_2D, 0, glinternalFormat, width, height, 0, glformat, GL_UNSIGNED_BYTE, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, texture_);
			glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, width, height, glformat, GL_UNSIGNED_BYTE, data);
		}
	}

	void OGLTexture::CustomAttribute(const std::string& name, void* pData)
	{
		if ("IsTexture" == name)
		{
			bool* b = reinterpret_cast<bool*>(pData);
			*b = true;

			return;
		}
	}
}
