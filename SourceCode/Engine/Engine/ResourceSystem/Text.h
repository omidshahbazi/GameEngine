// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_H
#define TEXT_H

namespace Engine
{
	namespace Containers
	{
		template<typename T>
		class Buffer;
	}

	using namespace Containers;

	namespace ResourceSystem
	{
		class Text
		{
		public:
			Text(Text &Text) = delete;

			Text(Buffer<byte> *Buffer) :
				m_Buffer(Buffer)
			{
			}

		private:
			Buffer<byte> * m_Buffer;
		};
	}
}

#endif