// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_COMMON_H
#define OPEN_GL_COMMON_H

#include <Containers\Map.h>
#include <RenderDevice\IDevice.h>
#include <GLEW\include\GL\glew.h>
#include <Platform\PlatformGL.h>

namespace Engine
{
	using namespace Containers;
	using namespace Platform;
	using namespace RenderDevice;

	namespace OpenGLRenderDevice
	{
		namespace Private
		{
			struct BufferInfo
			{
			public:
				uint32 Handle;
				uint32 Size;
				GPUBufferTypes Type;
				bool IsIntermediate;
			};

			struct TextureBufferInfo : public BufferInfo
			{
			public:
				TextureTypes TextureType;
				uint32 Width;
				uint32 Height;
				Formats Format;
			};

			struct MeshBufferInfo : public BufferInfo
			{
			public:
				BufferInfo* IndexBufferObject;
				VertexLayouts Layout;
			};

			struct ProgramInfo
			{
			public:
				uint32 Handle;
			};

			struct RenderTargetBufferInfo
			{
			public:
				ResourceHandle Handle;
				IDevice::TextureList Textures;
			};

			struct RenderContextInfo
			{
			public:
				typedef Map<MeshBufferInfo*, ResourceHandle> MeshVertexArrayMap;

			public:
				PlatformGL::ContextHandle ContextHandle;
				PlatformGL::WGLContextHandle WGLContextHandle;

				MeshVertexArrayMap VertexArrays;

				bool IsActive;
			};

			typedef Map<ResourceHandle, RenderContextInfo*> RenderContextMap;

			INLINE uint32 GetBufferType(GPUBufferTypes Type)
			{
				switch (Type)
				{
				case GPUBufferTypes::Constant:
					return GL_UNIFORM_BUFFER;
				case GPUBufferTypes::Vertex:
					return GL_ARRAY_BUFFER;
				case GPUBufferTypes::Index:
					return GL_ELEMENT_ARRAY_BUFFER;
				case GPUBufferTypes::Pixel:
					return GL_PIXEL_UNPACK_BUFFER;
				}

				return 0;
			}

			INLINE uint32 GetTextureType(TextureTypes Type)
			{
				switch (Type)
				{
				case TextureTypes::TwoD:
					return GL_TEXTURE_2D;
				}

				return 0;
			}

			INLINE uint32 GetTextureFormat(Formats Format)
			{
				switch (Format)
				{
				case Formats::R8:
					return GL_RED;
				case Formats::R16:
					return GL_RED;
				case Formats::R32:
					return GL_RED;
				case Formats::R16F:
					return GL_RED;
				case Formats::R32F:
					return GL_RED;
				case Formats::RG8:
					return GL_RG;
				case Formats::RG16:
					return GL_RG;
				case Formats::RG32:
					return GL_RG;
				case Formats::RG16F:
					return GL_RG;
				case Formats::RG32F:
					return GL_RG;
				case Formats::RGB8:
					return GL_RGB;
				case Formats::RGB16:
					return GL_RGB;
				case Formats::RGB32:
					return GL_RGB;
				case Formats::RGB16F:
					return GL_RGB;
				case Formats::RGB32F:
					return GL_RGB;
				case Formats::RGBA8:
					return GL_RGBA;
				case Formats::RGBA16:
					return GL_RGBA;
				case Formats::RGBA32:
					return GL_RGBA;
				case Formats::RGBA16F:
					return GL_RGBA;
				case Formats::RGBA32F:
					return GL_RGBA;
				case Formats::Depth16:
					return GL_DEPTH_COMPONENT;
				case Formats::Depth24:
					return GL_DEPTH_COMPONENT;
				case Formats::Depth32:
					return GL_DEPTH_COMPONENT;
				case Formats::Depth32F:
					return GL_DEPTH_COMPONENT;
				case Formats::DepthStencil24F:
					return GL_DEPTH_STENCIL;
				case Formats::DepthStencil32F:
					return GL_DEPTH_STENCIL;
				}

				return 0;
			}

			INLINE uint32 GetTexturePixelType(Formats Format)
			{
				switch (Format)
				{
				case Formats::R8:
					return GL_UNSIGNED_BYTE;
				case Formats::R16:
					return GL_UNSIGNED_SHORT;
				case Formats::R32:
					return GL_UNSIGNED_INT;
				case Formats::R16F:
					return GL_HALF_FLOAT;
				case Formats::R32F:
					return GL_FLOAT;
				case Formats::RG8:
					return GL_UNSIGNED_BYTE;
				case Formats::RG16:
					return GL_UNSIGNED_SHORT;
				case Formats::RG32:
					return GL_UNSIGNED_INT;
				case Formats::RG16F:
					return GL_HALF_FLOAT;
				case Formats::RG32F:
					return GL_FLOAT;
				case Formats::RGB8:
					return GL_UNSIGNED_BYTE;
				case Formats::RGB16:
					return GL_UNSIGNED_INT;
				case Formats::RGB32:
					return GL_UNSIGNED_INT;
				case Formats::RGB16F:
					return GL_HALF_FLOAT;
				case Formats::RGB32F:
					return GL_FLOAT;
				case Formats::RGBA8:
					return GL_UNSIGNED_BYTE;
				case Formats::RGBA16:
					return GL_UNSIGNED_SHORT;
				case Formats::RGBA32:
					return GL_UNSIGNED_INT;
				case Formats::RGBA16F:
					return GL_HALF_FLOAT;
				case Formats::RGBA32F:
					return GL_FLOAT;
				case Formats::Depth16:
					return GL_UNSIGNED_SHORT;
				case Formats::Depth24:
					return GL_UNSIGNED_INT;
				case Formats::Depth32:
					return GL_UNSIGNED_INT;
				case Formats::Depth32F:
					return GL_FLOAT;
				case Formats::DepthStencil24F:
					return GL_UNSIGNED_INT_24_8;
				case Formats::DepthStencil32F:
					return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
				}

				return 0;
			}

			INLINE uint32 GetBufferAccess(GPUBufferAccess Type)
			{
				switch (Type)
				{
				case GPUBufferAccess::ReadOnly:
					return GL_READ_ONLY;
				case GPUBufferAccess::WriteOnly:
					return GL_WRITE_ONLY;
				case GPUBufferAccess::ReadAndWrite:
					return GL_READ_WRITE;
				}

				return 0;
			}

			INLINE bool LockBufferInternal(BufferInfo* Info, GPUBufferAccess Access, byte** Buffer)
			{
				uint32 target = GetBufferType(Info->Type);

				glBindBuffer(target, Info->Handle);

				void* buffer = glMapBuffer(target, GetBufferAccess(Access));

				if (buffer == nullptr)
					return false;

				*Buffer = ReinterpretCast(byte*, buffer);

				return true;
			}

			INLINE bool UnlockBufferInternal(BufferInfo* Info)
			{
				uint32 target = GetBufferType(Info->Type);

				glBindBuffer(target, Info->Handle);

				glUnmapBuffer(target);

				glBindBuffer(target, 0);

				return true;
			}
		}
	}
}

#endif