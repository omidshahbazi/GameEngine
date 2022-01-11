// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Texture.h>
#include <RenderSystem\PixelBuffer.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		using namespace Private;

		Texture::Texture(ThreadedDevice* Device, ResourceHandle Handle, TextureTypes Type, Formats Format, const Vector2I& Dimension) :
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
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_Buffer);
		}

		void Texture::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Texture, GetName().GetValue());
		}

		bool Texture::SetVerticalWrapping(TextureWrapModes Mode)
		{
			GetDevice()->SetTextureVerticalWrapping(GetHandle(), m_Type, Mode);

			return true;
		}

		bool Texture::SetHorizontalWrapping(TextureWrapModes Mode)
		{
			GetDevice()->SetTextureHorizontalWrapping(GetHandle(), m_Type, Mode);

			return true;
		}

		bool Texture::SetMinifyFilter(TextureMinifyFilters Filter)
		{
			GetDevice()->SetTextureMinifyFilter(GetHandle(), m_Type, Filter);

			return true;
		}

		bool Texture::SetMagnifyFilter(TextureMagnfyFilters Filter)
		{
			GetDevice()->SetTextureMagnifyFilter(GetHandle(), m_Type, Filter);

			return true;
		}

		bool Texture::GenerateMipMaps(void)
		{
			//GetDevice()->GenerateTextureMipMap(GetHandle(), m_Type);

			return true;
		}

		void Texture::GenerateBuffer(void)
		{
			const uint32 bufferSize = GetBufferSize();
			if (bufferSize == 0)
				return;

			ResourceHandle bufferHandle;
			if (!GetDevice()->CreateBuffer(bufferHandle).Wait())
				return;

			if (!GetDevice()->CopyFromTextureToBuffer(bufferHandle, GetHandle(), bufferSize, m_Type, m_Format, 0).Wait())
				return;

			m_Buffer = RenderSystemAllocators::RenderSystemAllocator_Allocate<PixelBuffer>();

			ConstructMacro(PixelBuffer, m_Buffer, this, bufferHandle);
		}
	}
}