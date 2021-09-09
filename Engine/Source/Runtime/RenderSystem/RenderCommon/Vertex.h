// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VERTEX_H
#define VERTEX_H

#include <Containers\Vector.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Containers;
	using namespace MathContainers;

	namespace RenderCommon
	{
		struct Vertex
		{
		public:
			Vertex(void)
			{
			}

			Vertex(const Vector3F& Position) :
				Position(Position)
			{
			}

			Vertex(const Vector3F& Position, const Vector2F& UV) :
				Position(Position),
				UV(UV)
			{
			}

			Vertex(const Vector3F& Position, const Vector3F& Normal, const Vector2F& UV) :
				Position(Position),
				Normal(Normal),
				UV(UV)
			{
			}

			Vector3F Position;
			Vector3F Normal;
			Vector2F UV;
		};
	}
}

#endif