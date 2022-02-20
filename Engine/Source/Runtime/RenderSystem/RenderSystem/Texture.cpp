// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Texture.h>
#include <RenderSystem\PixelBuffer.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderSystem\Private\CommandBufferHelper.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>

namespace Engine
{
	using namespace RenderCommon::Private;
	using namespace RenderDevice;

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
			if (!GetDevice()->GetTextureFootprint(GetHandle(), m_Footprint).Wait())
				return;

			GenerateBuffer();

			SetName("Texture");
		}

		Texture::~Texture(void)
		{
			if (m_Buffer != nullptr)
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_Buffer);
		}

		void Texture::SetNameInternal(const WString& Name)
		{
			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Texture, GetName().GetValue());

			if (m_Buffer != nullptr)
				m_Buffer->SetName(Name + L"_PixelBuffer");
		}

		bool Texture::SetVerticalWrapping(TextureWrapModes Mode)
		{
			GetDevice()->SetTextureVerticalWrapping(GetHandle(), Mode);

			return true;
		}

		bool Texture::SetHorizontalWrapping(TextureWrapModes Mode)
		{
			GetDevice()->SetTextureHorizontalWrapping(GetHandle(), Mode);

			return true;
		}

		bool Texture::SetMinifyFilter(TextureMinifyFilters Filter)
		{
			GetDevice()->SetTextureMinifyFilter(GetHandle(), Filter);

			return true;
		}

		bool Texture::SetMagnifyFilter(TextureMagnfyFilters Filter)
		{
			GetDevice()->SetTextureMagnifyFilter(GetHandle(), Filter);

			return true;
		}

		bool Texture::GenerateMipMaps(void)
		{
			ICommandBuffer* cb = nullptr;
			if (!GetDevice()->CreateCommandBuffer(cb).Wait())
				return false;

			cb->BeginEvent(L"GenerateMipMapCommandBuffer");
			cb->GenerateMipMap(GetHandle());
			cb->EndEvent();

			return CommandBufferHelper::SubmitAndDestroy(GetDevice(), cb);
		}

		void Texture::GenerateBuffer(void)
		{
			const uint32 bufferSize = GetBufferSize();
			if (bufferSize == 0)
				return;

			ResourceHandle bufferHandle;
			if (!GetDevice()->CreateBuffer(GPUBufferTypes::Pixel, bufferSize, bufferHandle).Wait())
				return;

			ICommandBuffer* cb = nullptr;
			if (!GetDevice()->CreateCommandBuffer(cb).Wait())
				return;

			cb->BeginEvent(L"CopyCommandBuffer");
			cb->CopyBuffer(GetHandle(), bufferHandle);
			cb->EndEvent();

			if (!CommandBufferHelper::SubmitAndDestroy(GetDevice(), cb))
				return;

			m_Buffer = RenderSystemAllocators::RenderSystemAllocator_Allocate<PixelBuffer>();

			ConstructMacro(PixelBuffer, m_Buffer, this, bufferHandle);
		}
	}
}