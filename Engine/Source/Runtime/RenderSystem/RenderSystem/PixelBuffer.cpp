// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\PixelBuffer.h>
#include <RenderSystem\Texture.h>
#include <RenderSystem\Private\ThreadedDevice.h>

namespace Engine
{
	namespace RenderSystem
	{
		PixelBuffer::PixelBuffer(Texture* Texture, ResourceHandle Handle) :
			GPUBuffer(Texture->GetDevice(), Handle, Texture->GetBufferSize()),
			m_Texture(Texture)
		{
		}

		void PixelBuffer::Reset(void)
		{
			GPUBuffer::Reset();

			m_CurrentMoveIndex = 0;
		}

		void PixelBuffer::Lock(GPUBufferAccess Access)
		{
			GPUBuffer::Lock(Access);

			m_CurrentMoveIndex = 0;
		}

		void PixelBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();

			if (GetLastLockAccess() == GPUBufferAccess::ReadOnly)
				return;

			CopyTo(m_Texture->GetHandle());
		}

		void PixelBuffer::Move(uint32 Count)
		{
			const Vector2I dimension = m_Texture->GetDimension();

			uint32 index = m_CurrentMoveIndex + Count;

			Seek(index % dimension.X, index / dimension.X);
		}

		void PixelBuffer::Seek(uint32 X, uint32 Y)
		{
			const Vector2I dimension = m_Texture->GetDimension();
			m_CurrentMoveIndex = X + (Y * dimension.X);

			GPUBuffer::Seek((m_Texture->GetRowPitch() * (m_CurrentMoveIndex / dimension.X)) + (m_Texture->GetPixelSize() * (m_CurrentMoveIndex % dimension.X)));
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