// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_CONSTANT_SUPPLIER_H
#define SHADER_CONSTANT_SUPPLIER_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\ShaderDataType.h>
#include <Containers\AnyDataType.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <Rendering\DeviceInterface.h>
#include <functional>
#include <memory>

namespace Engine
{
	using namespace Containers;
	using namespace Utility;

	namespace Rendering
	{
		class IDevice;

		namespace Private
		{
			namespace Commands
			{
				class DrawCommand;
			}
		}

		class Shader;

		using namespace Private;
		using namespace Private::Commands;

		class RENDERING_API ShaderConstantSupplier : private DeviceInterface::IListener
		{
			SINGLETON_DECLARATION(ShaderConstantSupplier);

			friend class DeviceInterface;
			friend class DrawCommand;

		public:
			typedef std::function<const AnyDataType& (void)> FetchConstantFunction;
			typedef std::shared_ptr<FetchConstantFunction> FetchConstantFunctionFunctionPtr;

		private:
			struct ConstantSupplierInfo
			{
			public:
				~ConstantSupplierInfo(void)
				{
					Function = nullptr;
				}

				ShaderDataType::Types DataType;
				FetchConstantFunctionFunctionPtr Function;
			};

			typedef Map<String, ConstantSupplierInfo> InfoMap;

		private:
			ShaderConstantSupplier(void) :
				m_Initialized(false)
			{
			}

			void Initialize(DeviceInterface* DeviceInterface);

		public:

			void RegisterFloatConstant(const String& Name, FetchConstantFunction Function);
			void RegisterFloat2Constant(const String& Name, FetchConstantFunction Function);
			void RegisterFloat3Constant(const String& Name, FetchConstantFunction Function);
			void RegisterMatrix4Constant(const String& Name, FetchConstantFunction Function);
			void RegisterTextureConstant(const String& Name, FetchConstantFunction Function);

		private:
			void SupplyConstants(Shader* Shader) const;

			void OnWindowChanged(Window* Window) override;
			void OnWindowResized(Window* Window) override;

		private:
		private:
			bool m_Initialized;

			InfoMap m_Infos;
			Vector2I m_FrameSize;

		};
	}
}

#endif