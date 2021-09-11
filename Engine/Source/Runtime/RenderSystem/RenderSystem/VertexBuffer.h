// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <RenderSystem\GPUBuffer.h>
#include <RenderCommon\Vertex.h>

namespace Engine
{
	namespace RenderSystem
	{
		class SubMesh;

		class RENDERSYSTEM_API VertexBuffer : public GPUBuffer
		{
			friend class SubMesh;

		protected:
			VertexBuffer(SubMesh* SubMesh, ResourceHandle Handle);

		public:
			void Lock(GPUBufferAccess Access = GPUBufferAccess::ReadOnly);
			void Unlock(void);

			void Move(uint32 Count);

			Vertex& Get(void);
			const Vertex& Get(void) const;

		private:
			SubMesh* m_SubMesh;
		};
	}
}

#endif