#pragma once

#include <glm\matrix.hpp>
#include <vector>

class Framework;
class Component;

class GameObject
{
	friend class Framework;

public:
	typedef std::vector<Component*> ComponentList;

public:
	GameObject(Framework *Framework);

	bool AddComponent(Component *Component);

	inline Framework *GetFramework(void) const
	{
		return m_Framework;
	}

	inline const ComponentList GetComponents(void) const
	{
		return m_Component;
	}

	void SetPosition(const glm::vec3 &Value);

	void SetScale(const glm::vec3 &Value);

	void SetRotation(float Angle);

	inline const glm::mat4 &GetLocalTransformMatrix(void) const
	{
		return m_LocalTransfomrMatrix;
	}

	inline const glm::mat4 &GetWorldTransformMatrix(void) const
	{
		return m_WorldTransfomrMatrix;
	}

private:
	inline void UpdateWorldMatrix(const glm::mat4 &VPMatrix)
	{
		m_WorldTransfomrMatrix = VPMatrix * m_LocalTransfomrMatrix;
	}

private:
	Framework *m_Framework;

	glm::mat4 m_LocalTransfomrMatrix;
	glm::mat4 m_WorldTransfomrMatrix;

	ComponentList m_Component;
};