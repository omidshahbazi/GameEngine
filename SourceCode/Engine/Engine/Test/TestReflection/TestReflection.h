
#include <Common\PrimitiveTypes.h>
#include <TestReflection.Reflection.h>

REFLECTION_STRUCT()
struct DummyStruct
{
	DUMMYSTRUCT_OBJECT()

public:
	DummyStruct()
	{
	}

	REFLECTION_FUNCTION()
	void Set(int R, int G, int B)
	{
		this->R = R;
		this->G = G;
		this->B = B;
	}

public:
	REFLECTION_PROPERTY()
	int R;
	REFLECTION_PROPERTY()
	int G;
	REFLECTION_PROPERTY()
	int B;
};
