#pragma once

class GameObject;

class Component
{
public:
	Component(void) :
		m_GameObject(nullptr)
	{
	}

	virtual void Update(void)
	{
	}

	virtual void Render(void)
	{
	}

	inline GameObject *GetGameObject(void) const
	{
		return m_GameObject;
	}

	inline bool SetGameObject(GameObject *GameObject)
	{
		if (m_GameObject != nullptr)
			return false;

		m_GameObject = GameObject;
	
		return true;
	}

private:
	GameObject *m_GameObject;
};