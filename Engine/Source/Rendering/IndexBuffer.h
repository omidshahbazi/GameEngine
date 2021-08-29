// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class SubMesh;

		class RENDERING_API IndexBuffer : public GPUBuffer
		{
			friend class SubMesh;

		protected:
			IndexBuffer(SubMesh* SubMesh, Handle Handle);

		public:
			void Lock(Access Access = Access::ReadOnly);
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