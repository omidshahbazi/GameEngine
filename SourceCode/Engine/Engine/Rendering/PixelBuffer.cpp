// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\PixelBuffer.h>
#include <Rendering\Texture.h>
#include <Rendering\Private\ThreadedDevice.h>

namespace Engine
{
	namespace Rendering
	{
		PixelBuffer::PixelBuffer(Texture* Texture, Handle Handle) :
			GPUBuffer(Texture->GetDevice(), Handle, Texture->GetBufferSize(), Types::PixelUnpack),
			m_Texture(Texture),
			m_LastLockAccess(Access::ReadOnly)
		{
		}

		void PixelBuffer::Lock(Access Access)
		{
			m_LastLockAccess = Access;

			GPUBuffer::Lock(m_LastLockAccess);
		}

		void PixelBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();

			if (m_LastLockAccess == Access::ReadOnly)
				return;

			const auto& dimension = m_Texture->GetDimension();

			GetDevice()->CopyFromBuffer(GetHandle(), GetType(), m_Texture->GetHandle(), m_Texture->GetType(), dimension.X, dimension.Y, m_Texture->GetFormat());
		}

		void PixelBuffer::Move(uint32 X, uint32 Y)
		{
			const auto& dimension = m_Texture->GetDimension();

			Move(X + (Y * dimension.X));
		}

		void PixelBuffer::Move(uint32 Count)
		{
			GPUBuffer::Move(m_Texture->GetPixelSize() * Count);
		}

		ColorUI8& PixelBuffer::GetColorUI8Pixel(void)
		{
			return *ReinterpretCast(ColorUI8*, GetCurrentBuffer());
		}

		const ColorUI8& PixelBuffer::GetColorUI8Pixel(void) const
		{
			return *ReinterpretCast(ColorUI8*, GetCurrentBuffer());
		}

		ColorUI16& PixelBuffer::GetColorUI16Pixel(void)
		{
			return *ReinterpretCast(ColorUI16*, GetCurrentBuffer());
		}

		const ColorUI16& PixelBuffer::GetColorUI16Pixel(void) const
		{
			return *ReinterpretCast(ColorUI16*, GetCurrentBuffer());
		}

		ColorUI32& PixelBuffer::GetColorUI32Pixel(void)
		{
			return *ReinterpretCast(ColorUI32*, GetCurrentBuffer());
		}

		const ColorUI32& PixelBuffer::GetColorUI32Pixel(void) const
		{
			return *ReinterpretCast(ColorUI32*, GetCurrentBuffer());
		}
	}
}