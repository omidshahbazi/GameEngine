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

		uint32 GetTextureBufferSize(Texture::Formats Format, const Vector2I& Dimension)
		{
			uint32 channelCount = 0;

			switch (Format)
			{
			case Engine::Rendering::Texture::Formats::R8:
				channelCount = 1;
				break;

			case Engine::Rendering::Texture::Formats::R16:
			case Engine::Rendering::Texture::Formats::R16F:
			case Engine::Rendering::Texture::Formats::RG8:
			case Engine::Rendering::Texture::Formats::Depth16:
				channelCount = 2;
				break;

			case Engine::Rendering::Texture::Formats::RGB8:
			case Engine::Rendering::Texture::Formats::Depth24:
			case Engine::Rendering::Texture::Formats::Stencil24F:
				channelCount = 3;
				break;

			case Engine::Rendering::Texture::Formats::R32:
			case Engine::Rendering::Texture::Formats::R32F:
			case Engine::Rendering::Texture::Formats::RG16:
			case Engine::Rendering::Texture::Formats::RG16F:
			case Engine::Rendering::Texture::Formats::RGBA8:
			case Engine::Rendering::Texture::Formats::Depth32:
			case Engine::Rendering::Texture::Formats::Depth32F:
			case Engine::Rendering::Texture::Formats::Stencil32F:
				channelCount = 4;
				break;

			case Engine::Rendering::Texture::Formats::RGB16:
			case Engine::Rendering::Texture::Formats::RGB16F:
				channelCount = 6;
				break;

			case Engine::Rendering::Texture::Formats::RG32:
			case Engine::Rendering::Texture::Formats::RG32F:
			case Engine::Rendering::Texture::Formats::RGBA16:
			case Engine::Rendering::Texture::Formats::RGBA16F:
				channelCount = 8;
				break;

			case Engine::Rendering::Texture::Formats::RGB32:
			case Engine::Rendering::Texture::Formats::RGB32F:
				channelCount = 12;
				break;

			case Engine::Rendering::Texture::Formats::RGBA32:
			case Engine::Rendering::Texture::Formats::RGBA32F:
				channelCount = 16;
				break;
			}

			return channelCount * Dimension.X * Dimension.Y;
		}

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

			GetDevice()->AttachBufferData(bufferHandle, IDevice::BufferTypes::PixelPack, IDevice::BufferUsages::StaticCopy, GetTextureBufferSize(m_Format, m_Dimension), GetHandle(), m_Type, m_Format, 0);

			m_Buffer = ReinterpretCast(PixelBuffer*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, sizeof(PixelBuffer)));

			Construct_Macro(PixelBuffer, m_Buffer, GetDevice(), bufferHandle);

			void* buff = m_Buffer->Map();

			m_Buffer->Unmap();
		}
	}
}