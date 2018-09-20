
#include <Common\PrimitiveTypes.h>
#include <TestReflection.Reflection.h>

REFLECTION_STRUCT()
struct DummyStruct
{
	DUMMYSTRUCT_OBJECT()

public:
	DummyStruct(int R, int G, int B)
	{
	}
};
