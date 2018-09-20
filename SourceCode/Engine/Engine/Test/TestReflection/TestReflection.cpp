
#include <TestReflection.h>

void main()
{
	const Engine::Reflection::Type &type = DummyStruct::GetType();

	auto name = type.GetName();
}