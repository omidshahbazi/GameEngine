// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include <Containers\Vector.h>
#include <Containers\Strings.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class Program;
		class Material;
		class Mesh;
		class Window;
		struct Color;

		namespace Private
		{
			namespace Commands
			{
				class CommandBase;
			}
		}

		using namespace Private::Commands;

		class RENDERING_API DeviceInterface
		{
		public:
			enum class Type
			{
				OpenGL
			};

			typedef Vector<Texture*> TextureList;
			typedef Vector<RenderTarget*> RenderTargetList;
			typedef Vector<Program*> ProgramVector;
			typedef Vector<Window*> WindowVector;
			typedef Vector<CommandBase*> CommandList;

		public:
			DeviceInterface(Type Type);
			~DeviceInterface(void);

			void Initialize(void);

			void SetSampleCount(uint8 Count);

			void SetForwardCompatible(bool Value);

			Texture *CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, Texture::Formats Format);
			void DestroyTexture(Texture *Texture);

			RenderTarget *CreateRenderTarget(const RenderTargetInfo *Info);
			void DestroyRenderTarget(RenderTarget * RenderTarget);
			void SetRenderTarget(RenderTarget * RenderTarget);

			Program *CreateProgram(const String &Shader);
			void DestroyProgram(Program *Program);

			Mesh *CreateMesh(const MeshInfo *Info, IDevice::BufferUsages Usage);
			void DestroyMesh(Mesh *Mesh);

			Window *CreateWindow(uint16 Width, uint16 Height, cstr Title);
			void DestroyWindow(Window *Window);

			void Clear(IDevice::ClearFlags Flags, Color Color);

			void DrawMesh(MeshHandle *Mesh, const Matrix4F &Transform, ProgramHandle *Program);
			void DrawMesh(MeshHandle *Mesh, const Matrix4F &Transform, Material *Material);

			void SubmitCommands(void);

			void BeginRender(void);
			void EndRender(void);

			IDevice *GetDevice(void) const
			{
				return m_Device;
			}

		private:
			void InitializeDevice(void);

			void EraseCommands(void);

		private:
			Type m_Type;
			IDevice *m_Device;
			TextureList m_Textures;
			RenderTargetList m_RenderTargets;
			ProgramVector m_Programs;
			WindowVector m_Windows;
			CommandList m_Commands;
		};
	}
}

#endif