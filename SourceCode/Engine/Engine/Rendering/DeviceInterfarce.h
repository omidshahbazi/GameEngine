// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef RENDERING_INTERFACE_H
#define RENDERING_INTERFACE_H

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
		class Window;
		struct Color;

		class RENDERING_API DeviceInterfarce
		{
		public:
			enum class Type
			{
				OpenGL
			};

			typedef Vector<Texture*> TextureVector;
			typedef Vector<Program*> ProgramVector;
			typedef Vector<Window*> WindowVector;

		public:
			DeviceInterfarce(Type Type);
			~DeviceInterfarce(void);

			void Initialize(void);

			void SetSampleCount(uint8 Count);

			void SetForwardCompatible(bool Value);

			void SetProfilingEnabled(bool Value);

			void SetClearColor(Color Color);

			void SetClearFlags(IDevice::ClearFlags Flags);

			Texture *CreateTexture2D(const byte *Data, uint32 Width, uint32 Height);
			void DestroyTexture2D(Texture *Texture);

			Program *CreateProgram(const String &Shader);
			void DestroyProgram(Program *Program);

			Window *CreateWindow(uint16 Width, uint16 Height, cstr Title);
			void DestroyWindow(Window *Window);

			void Update(void);

		private:
			void InitializeDevice(void);

		private:
			Type m_Type;
			IDevice *m_Device;
			TextureVector m_Textures;
			ProgramVector m_Programs;
			WindowVector m_Windows;


			IDevice::ClearFlags m_ClearFlags;
		};
	}
}

#endif