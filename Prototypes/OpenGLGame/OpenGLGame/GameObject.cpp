
#include <glm\gtc\matrix_transform.hpp>
#include "GameObject.h"
#include "Component.h"
#include "Framework.h"

GameObject::GameObject(Framework *Framework) :
	m_Framework(Framework),
	m_LocalTransfomrMatrix(1.0F),
	m_WorldTransfomrMatrix(1.0F)
{
}

bool GameObject::AddComponent(Component *Component)
{
	if (!Component->SetGameObject(this))
		return false;

	m_Component.push_back(Component);

	m_Framework->OnComponentAdded(Component);

	return true;
}

void GameObject::SetPosition(const glm::vec3 &Value)
{
	m_LocalTransfomrMatrix = glm::translate(m_LocalTransfomrMatrix, Value);
}

void GameObject::SetScale(const glm::vec3 &Value)
{
	m_LocalTransfomrMatrix = glm::scale(m_LocalTransfomrMatrix, Value);
}

void GameObject::SetRotation(const glm::vec3 &Value)
{
	m_LocalTransfomrMatrix = glm::rotate(m_LocalTransfomrMatrix, glm::radians(45.0F), glm::vec3(0, 0, 1));;
}