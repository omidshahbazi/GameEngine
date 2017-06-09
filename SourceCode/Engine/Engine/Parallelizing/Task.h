// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <functional>

#ifndef TASK_H
#define TASK_H

namespace Engine
{
	namespace Parallelizing
	{
		class Task
		{
		public:
			typedef std::function<void(void)> Procedure;

			Task(Procedure Procedure) :
				m_Procedure(Procedure)
			{ }

			void Do(void)
			{
				m_Procedure();
			}

		private:
			Procedure m_Procedure;
		};
	}
}

#endif