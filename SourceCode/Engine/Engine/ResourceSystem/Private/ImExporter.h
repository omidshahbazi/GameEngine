// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IM_EXPORTER_BASE_H
#define IM_EXPORTER_BASE_H

#include <Containers\Strings.h>
#include <Common\Definitions.h>
#include <ImExporter.Reflection.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		namespace Private
		{
			REFLECTION_STRUCT()
			struct TextSettings
			{
				TEXTSETTINGS_OBJECT()

			public:
				REFLECTION_PROPERTY()
					int32 type;
			};

			class RESOURCESYSTEM_API ImExporter
			{
			public:
				static bool ImportText(const WString& Path, TextSettings* Settings);
			};
		}
	}
}
#endif