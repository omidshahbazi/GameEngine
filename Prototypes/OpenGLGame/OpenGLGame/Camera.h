#pragma once

#include <glm\matrix.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Component.h"
#include "Color.h"

enum class ProjectionMode
{
	Perpective = 0,
	Ortho
};

enum class PolygonModes
{
	Points = 0,
	Wireframe,
	Fill
};

class Camera : public Component
{
	friend class Framework;

public:
	Camera(void) :
		m_ClearColor(0, 0, 0, 1),
		m_ProjectionMode(ProjectionMode::Perpective),
		m_PolygonMode(PolygonModes::Fill),
		m_NearClipDistance(0.0F),
		m_FarClipDistance(100.0F),
		m_AspectRatio(0.0F),
		m_FieldOfView(60.0F)
	{
	}

	inline void SetProjectionMode(ProjectionMode Mode)
	{
		m_ProjectionMode = Mode;
	}
	inline ProjectionMode GetProjectionMode(void) const
	{
		return m_ProjectionMode;
	}

	inline void SetPolygonMode(PolygonModes Mode)
	{
		m_PolygonMode = Mode;
	}
	inline PolygonModes GetPolygonMode(void) const
	{
		return m_PolygonMode;
	}

	inline void SetClearColor(Color Color)
	{
		m_ClearColor = Color;
	}
	inline Color GetClearColor(void) const
	{
		return m_ClearColor;
	}

	inline const glm::mat4 &GetProjectionMatrix(void) const
	{
		return m_ProjectionMatrix;
	}

private:
	void OnDeviceResized(int Width, int Height)
	{
		if (m_ProjectionMode == ProjectionMode::Perpective)
		{
			m_AspectRatio = (float)Width / Height;
			m_ProjectionMatrix = glm::perspective(glm::radians(m_FieldOfView), m_AspectRatio, m_NearClipDistance, m_FarClipDistance);
		}
		else
			m_ProjectionMatrix = glm::ortho(0.0F, (float)Width, (float)Height, (float)0, m_NearClipDistance, m_FarClipDistance);
	}
	
private:
	ProjectionMode m_ProjectionMode;
	PolygonModes m_PolygonMode;
	Color m_ClearColor;
	float m_NearClipDistance;
	float m_FarClipDistance;
	float m_AspectRatio;
	float m_FieldOfView;

	glm::mat4 m_ProjectionMatrix;
};