// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_DATA_H
#define CONSTANT_DATA_H

#include <RenderCommon\RenderCommon.h>
#include <Containers\Map.h>
#include <ResourceCommon\Resource.h>

namespace Engine
{
	using namespace RenderCommon;
	using namespace Containers;
	using namespace ResourceCommon;

	namespace RenderSystem
	{
		namespace Private
		{
			typedef Map<ProgramConstantHash, ProgramConstantHandle> ProgramConstantHandleMap;

			template<typename T>
			using ProgramBufferMap = Map<ProgramConstantHash, T*>;

			typedef Map<ProgramConstantHash, TextureResource*> ProgramTextureMap;
		}
	}
}

#endif