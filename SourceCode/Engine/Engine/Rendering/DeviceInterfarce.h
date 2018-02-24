// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef RENDERING_INTERFACE_H
#define RENDERING_INTERFACE_H

namespace Engine
{
	namespace Rendering
	{
		class IDevice;
		class Texture;
		class Program;
		class Window;

		class RENDERING_API DeviceInterfarce
		{
		public:
			enum class Type
			{
				OpenGL
			};

		public:
			DeviceInterfarce(Type Type);
			~DeviceInterfarce(void);

			void Initialize(void);

			void SetSampleCount(uint8 Count);

			void SetForwardCompatible(bool Value);

			void SetProfilingEnabled(bool Value);

			Texture *CreateTexture2D(const byte *Data, uint32 Width, uint32 Height);

			Program *CreateProgram(cstr VertexShader, cstr FragmentShader);

			Window *CreateWindow(uint16 Width, uint16 Height, cstr Title);
			
		private:
			void InitializeDevice(void);

		private:
			Type m_Type;
			IDevice *m_Device;
		};
	}
}

#endif