// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include <RenderSystem\GPUBuffer.h>

namespace Engine
{
	namespace RenderSystem
	{
		class SubMesh;

		class RENDERSYSTEM_API IndexBuffer : public GPUBuffer
		{
			friend class SubMesh;

		protected:
			IndexBuffer(SubMesh* SubMesh, ResourceHandle Handle);

		public:
			void Lock(GPUBufferAccess Access = GPUBufferAccess::ReadOnly);
			void Unlock(void);

			void Move(uint32 Count);

			uint32& Get(void);
			const uint32& Get(void) const;

		private:
			SubMesh* m_SubMesh;
		};
	}
}

#endif