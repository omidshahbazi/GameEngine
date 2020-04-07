// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Texture.h>
#include <Rendering\IDevice.h>
#include <Rendering\PixelBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		Texture::Texture(IDevice* Device, Handle Handle, Types Type, Formats Format, const Vector2I& Dimension) :
			NativeType(Device, Handle),
			m_Type(Type),
			m_Format(Format),
			m_Dimension(Dimension),
			m_Buffer(nullptr)
		{
			GenerateBuffer();
		}

		bool Texture::SetVerticalWrapping(WrapModes Mode)
		{
			return GetDevice()->SetTextureVerticalWrapping(GetHandle(), m_Type, Mode);
		}

		bool Texture::SetHorizontalWrapping(WrapModes Mode)
		{
			return GetDevice()->SetTextureHorizontalWrapping(GetHandle(), m_Type, Mode);
		}

		bool Texture::SetMinifyFilter(MinifyFilters Filter)
		{
			return GetDevice()->SetTextureMinifyFilter(GetHandle(), m_Type, Filter);
		}

		bool Texture::SetMagnifyFilter(MagnfyFilters Filter)
		{
			return GetDevice()->SetTextureMagnifyFilter(GetHandle(), m_Type, Filter);
		}

		bool Texture::GenerateMipMaps(void)
		{
			return GetDevice()->GenerateTextureMipMap(GetHandle(), m_Type);
		}

		void Texture::GenerateBuffer(void)
		{
			NativeType::Handle bufferHandle;
			if (!GetDevice()->CreateBuffer(bufferHandle))
				return;

			const uint32 bufferSize = GetBufferSize(m_Format, m_Dimension);

			GetDevice()->AttachBufferData(bufferHandle, IDevice::BufferTypes::PixelPack, IDevice::BufferUsages::StaticCopy, bufferSize, GetHandle(), m_Type, m_Format, 0);

			m_Buffer = ReinterpretCast(PixelBuffer*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, sizeof(PixelBuffer)));

			Construct_Macro(PixelBuffer, m_Buffer, GetDevice(), bufferHandle, bufferSize, GetChannelSize(m_Format), GetChannelCount(m_Format));
		}

		uint8 Texture::GetChannelSize(Formats Format)
		{
			switch (Format)
			{
			case Engine::Rendering::Texture::Formats::R8:
			case Engine::Rendering::Texture::Formats::RG8:
			case Engine::Rendering::Texture::Formats::RGB8:
			case Engine::Rendering::Texture::Formats::RGBA8:
				return 1;

			case Engine::Rendering::Texture::Formats::R16:
			case Engine::Rendering::Texture::Formats::R16F:
			case Engine::Rendering::Texture::Formats::Depth16:
			case Engine::Rendering::Texture::Formats::RG16:
			case Engine::Rendering::Texture::Formats::RG16F:
			case Engine::Rendering::Texture::Formats::RGB16:
			case Engine::Rendering::Texture::Formats::RGB16F:
			case Engine::Rendering::Texture::Formats::RGBA16:
			case Engine::Rendering::Texture::Formats::RGBA16F:
				return 2;

			case Engine::Rendering::Texture::Formats::Depth24:
			case Engine::Rendering::Texture::Formats::Stencil24F:
				return 3;

			case Engine::Rendering::Texture::Formats::R32:
			case Engine::Rendering::Texture::Formats::R32F:
			case Engine::Rendering::Texture::Formats::Depth32:
			case Engine::Rendering::Texture::Formats::Depth32F:
			case Engine::Rendering::Texture::Formats::Stencil32F:
			case Engine::Rendering::Texture::Formats::RG32:
			case Engine::Rendering::Texture::Formats::RG32F:
			case Engine::Rendering::Texture::Formats::RGB32:
			case Engine::Rendering::Texture::Formats::RGB32F:
			case Engine::Rendering::Texture::Formats::RGBA32:
			case Engine::Rendering::Texture::Formats::RGBA32F:
				return 4;
			}

			return 0;
		}

		uint8 Texture::GetChannelCount(Formats Format)
		{
			switch (Format)
			{
			case Engine::Rendering::Texture::Formats::R8:
			case Engine::Rendering::Texture::Formats::R16:
			case Engine::Rendering::Texture::Formats::R16F:
			case Engine::Rendering::Texture::Formats::Depth16:
			case Engine::Rendering::Texture::Formats::Depth24:
			case Engine::Rendering::Texture::Formats::Stencil24F:
			case Engine::Rendering::Texture::Formats::R32:
			case Engine::Rendering::Texture::Formats::R32F:
			case Engine::Rendering::Texture::Formats::Depth32:
			case Engine::Rendering::Texture::Formats::Depth32F:
			case Engine::Rendering::Texture::Formats::Stencil32F:
				return 1;

			case Engine::Rendering::Texture::Formats::RG8:
			case Engine::Rendering::Texture::Formats::RG16:
			case Engine::Rendering::Texture::Formats::RG16F:
			case Engine::Rendering::Texture::Formats::RG32:
			case Engine::Rendering::Texture::Formats::RG32F:
				return 2;

			case Engine::Rendering::Texture::Formats::RGB8:
			case Engine::Rendering::Texture::Formats::RGB16:
			case Engine::Rendering::Texture::Formats::RGB16F:
			case Engine::Rendering::Texture::Formats::RGB32:
			case Engine::Rendering::Texture::Formats::RGB32F:
				return 3;

			case Engine::Rendering::Texture::Formats::RGBA8:
			case Engine::Rendering::Texture::Formats::RGBA16:
			case Engine::Rendering::Texture::Formats::RGBA16F:
			case Engine::Rendering::Texture::Formats::RGBA32:
			case Engine::Rendering::Texture::Formats::RGBA32F:
				return 4;
			}

			return 0;
		}

		uint32 Texture::GetBufferSize(Formats Format, const Vector2I& Dimension)
		{
			return GetChannelSize(Format) * GetChannelCount(Format) * Dimension.X * Dimension.Y;
		}
	}
}