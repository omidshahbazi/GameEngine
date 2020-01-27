// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef WRAPPER_GENERATOR_H
#define WRAPPER_GENERATOR_H
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;

	namespace WrapperTool
	{
		class WRAPPERTOOL_API WrapperGenerator
		{
		public:
			WrapperGenerator(const String& ModuleFileName, const String& ModuleAPI, const String& FilePath, const String& OutputBaseFileName) :
				m_ModuleFileName(ModuleFileName),
				m_ModuleAPI(ModuleAPI),
				m_FilePath(FilePath),
				m_OutputBaseFileName(OutputBaseFileName)
			{
			}

			bool Generate(void);

		private:
			String m_ModuleFileName;
			String m_ModuleAPI;
			String m_FilePath;
			String m_OutputBaseFileName;
			String m_OutputClassName;
		};

	}
}
#endif
