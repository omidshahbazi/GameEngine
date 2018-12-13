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
		class Texture;
		class Program;
		class Mesh;
		class Window;
		struct Color;

		namespace Private
		{
			namespace Commands
			{
				class Command;
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

			typedef Vector<Texture*> TextureVector;
			typedef Vector<Program*> ProgramVector;
			typedef Vector<Window*> WindowVector;
			typedef Vector<Command*> CommandList;

		public:
			DeviceInterface(Type Type);
			~DeviceInterface(void);

			void Initialize(void);

			void SetSampleCount(uint8 Count);

			void SetForwardCompatible(bool Value);

			void SetClearColor(Color Color);

			void SetClearFlags(IDevice::ClearFlags Flags);

			Texture *CreateTexture2D(const byte *Data, uint32 Width, uint32 Height);
			void DestroyTexture2D(Texture *Texture);

			Program *CreateProgram(const String &Shader);
			void DestroyProgram(Program *Program);

			Mesh *CreateMesh(MeshInfo *Info, IDevice::BufferUsages Usage);
			void DestroyMesh(Mesh *Mesh);

			Window *CreateWindow(uint16 Width, uint16 Height, cstr Title);
			void DestroyWindow(Window *Window);

			void DrawMesh(Mesh *Mesh, Program *Program);

			void BeginRender(void);

			void EndRender(void);

		private:
			void InitializeDevice(void);

			void SupplyProgramPresetConstants(Program *Program);

		private:
			Type m_Type;
			IDevice *m_Device;
			TextureVector m_Textures;
			ProgramVector m_Programs;
			WindowVector m_Windows;
			CommandList m_Commands;
		};
	}
}

#endif