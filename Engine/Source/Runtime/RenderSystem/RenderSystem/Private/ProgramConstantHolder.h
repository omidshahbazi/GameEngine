//// Copyright 2016-2020 ?????????????. All Rights Reserved.
//#pragma once
//#ifndef PROGRAM_CONSTANT_HOLDER_H
//#define PROGRAM_CONSTANT_HOLDER_H
//
//#include <RenderCommon\RenderCommon.h>
//#include <Containers\Strings.h>
//#include <Containers\AnyDataType.h>
//#include <Containers\Map.h>
//#include <MathContainers\MathContainers.h>
//#include <ResourceCommon\Resource.h>
//
//namespace Engine
//{
//	using namespace RenderCommon;
//	using namespace ResourceCommon;
//	using namespace Containers;
//	using namespace MathContainers;
//
//	namespace RenderSystem
//	{
//		class ConstantBuffer;
//		class ProgramConstantSupplier;
//
//		namespace Private
//		{
//			class RENDERSYSTEM_API ProgramConstantHolder
//			{
//				friend class ProgramConstantSupplier;
//				friend class CommandBuffer;
//
//
//				typedef ConstantDataBase<ConstantBuffer*> BufferConstantDataBase;
//				typedef ConstantDataBase<TextureResource*> TextureConstantDataBase;
//
//				typedef Map<ConstantHash, BufferConstantDataBase> BufferDataBaseMap;
//				typedef Map<ConstantHash, TextureConstantDataBase> TextureDataBaseMap;
//
//			};
//		}
//	}
//}
//
//#endif