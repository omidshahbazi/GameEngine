// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <Rendering\GPUBuffer.h>
#include <Rendering\Vertex.h>

namespace Engine
{
	namespace Rendering
	{
		class SubMesh;

		class RENDERING_API VertexBuffer : public GPUBuffer
		{
			friend class SubMesh;

		protected:
			VertexBuffer(SubMesh* SubMesh, Handle Handle);

		public:
			void Lock(Access Access = Access::ReadOnly);
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