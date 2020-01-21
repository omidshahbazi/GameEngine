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
			WrapperGenerator(const String& FilePath, const String& OutputBaseFileName) :
				m_FilePath(FilePath),
				m_OutputBaseFileName(OutputBaseFileName)
			{
			}

			bool Generate(void);

		private:
			String m_FilePath;
			String m_OutputBaseFileName;
			String m_OutputClassName;
		};

	}
}
#endif
