// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPUTE_PROGRAM_H
#define COMPUTE_PROGRAM_H

#include <ResourceCommon\Resource.h>
#include <RenderSystem\Private\ConstantData.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace ResourceCommon;
	using namespace Containers;

	namespace RenderSystem
	{
		using namespace Private;
		
		class ConstantBuffer;

		class RENDERSYSTEM_API ComputeProgram
		{
			friend class CommandBuffer;

		public:
			//typedef MetaConstantData<ConstantBuffer*> BufferMetaConstantData;
			//typedef MetaConstantData<TextureResource*> TextureMetaConstantData;
			//typedef Map<ProgramConstantHash, BufferMetaConstantData> BufferMetaDataMap;
			//typedef Map<ProgramConstantHash, TextureMetaConstantData> TextureMetaDataMap;

		public:
			ComputeProgram(void);

			const ProgramResource* GetProgram(void) const
			{
				return m_Program;
			}

		private:
			//INLINE const BufferMetaDataMap& GetBuffers(void) const
			//{
			//	return m_Buffers;
			//}

			//INLINE const TextureMetaDataMap& GetTextures(void) const
			//{
			//	return m_Textures;
			//}

		private:
			ProgramResource* m_Program;
			//BufferMetaDataMap m_Buffers;
			//TextureMetaDataMap m_Textures;
		};
	}
}

#endif