// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Texture.h>
#include <Rendering\PixelBuffer.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		Texture::Texture(ThreadedDevice* Device, Handle Handle, Types Type, Formats Format, const Vector2I& Dimension) :
			NativeType(Device, Handle),
			m_Type(Type),
			m_Format(Format),
			m_Dimension(Dimension),
			m_Buffer(nullptr)
		{
			GenerateBuffer();
		}

		Texture::~Texture(void)
		{
			if (m_Buffer != nullptr)
				RenderingAllocators::RenderingSystemAllocator_Deallocate(m_Buffer);
		}

		bool Texture::SetVerticalWrapping(WrapModes Mode)
		{
			GetDevice()->SetTextureVerticalWrapping(GetHandle(), m_Type, Mode);

			return true;
		}

		bool Texture::SetHorizontalWrapping(WrapModes Mode)
		{
			GetDevice()->SetTextureHorizontalWrapping(GetHandle(), m_Type, Mode);

			return true;
		}

		bool Texture::SetMinifyFilter(MinifyFilters Filter)
		{
			GetDevice()->SetTextureMinifyFilter(GetHandle(), m_Type, Filter);

			return true;
		}

		bool Texture::SetMagnifyFilter(MagnfyFilters Filter)
		{
			GetDevice()->SetTextureMagnifyFilter(GetHandle(), m_Type, Filter);

			return true;
		}

		bool Texture::GenerateMipMaps(void)
		{
			GetDevice()->GenerateTextureMipMap(GetHandle(), m_Type);

			return true;
		}

		void Texture::GenerateBuffer(void)
		{
			GPUBuffer::Handle bufferHandle;
			if (!GetDevice()->CreateBuffer(bufferHandle).Wait())
				return;

			const uint32 bufferSize = GetBufferSize(m_Format, m_Dimension);
			if (bufferSize == 0)
				return;

			if (!GetDevice()->AttachBufferData(bufferHandle, GPUBuffer::Types::PixelPack, GPUBuffer::Usages::StaticCopy, bufferSize, GetHandle(), m_Type, m_Format, 0).Wait())
				return;

			m_Buffer = RenderingAllocators::RenderingSystemAllocator_Allocate<PixelBuffer>();

			ConstructMacro(PixelBuffer, m_Buffer, this, bufferHandle);
		}

		uint8 Texture::GetChannelSize(Formats Format)
		{
			switch (Format)
			{
			case Formats::R8:
			case Formats::RG8:
			case Formats::RGB8:
			case Formats::RGBA8:
				return 1;

			case Formats::R16:
			case Formats::R16F:
			case Formats::Depth16:
			case Formats::RG16:
			case Formats::RG16F:
			case Formats::RGB16:
			case Formats::RGB16F:
			case Formats::RGBA16:
			case Formats::RGBA16F:
				return 2;

			case Formats::Depth24:
			case Formats::Stencil24F:
				return 3;

			case Formats::R32:
			case Formats::R32F:
			case Formats::Depth32:
			case Formats::Depth32F:
			case Formats::Stencil32F:
			case Formats::RG32:
			case Formats::RG32F:
			case Formats::RGB32:
			case Formats::RGB32F:
			case Formats::RGBA32:
			case Formats::RGBA32F:
				return 4;
			}

			return 0;
		}

		uint8 Texture::GetChannelCount(Formats Format)
		{
			switch (Format)
			{
			case Formats::R8:
			case Formats::R16:
			case Formats::R16F:
			case Formats::Depth16:
			case Formats::Depth24:
			case Formats::Stencil24F:
			case Formats::R32:
			case Formats::R32F:
			case Formats::Depth32:
			case Formats::Depth32F:
			case Formats::Stencil32F:
				return 1;

			case Formats::RG8:
			case Formats::RG16:
			case Formats::RG16F:
			case Formats::RG32:
			case Formats::RG32F:
				return 2;

			case Formats::RGB8:
			case Formats::RGB16:
			case Formats::RGB16F:
			case Formats::RGB32:
			case Formats::RGB32F:
				return 3;

			case Formats::RGBA8:
			case Formats::RGBA16:
			case Formats::RGBA16F:
			case Formats::RGBA32:
			case Formats::RGBA32F:
				return 4;
			}

			return 0;
		}

		uint8 Texture::GetPixelSize(Formats Format)
		{
			return GetChannelSize(Format) * GetChannelCount(Format);
		}

		uint32 Texture::GetRowPitch(Formats Format, int32 Width)
		{
			return GetPixelSize(Format) * Width;
		}

		uint32 Texture::GetBufferSize(Formats Format, const Vector2I& Dimension)
		{
			return GetRowPitch(Format, Dimension.X) * Dimension.Y;
		}
	}
}