#pragma once

struct Color
{
public:
	Color(char R, char G, char B) :
		Color(R, G, B, 255)
	{
	}

	Color(char R, char G, char B, char A) :
		R(R),
		G(G),
		B(B),
		A(A)
	{
	}

public:
	inline float GetNormalizeR(void)
	{
		return (R / 255.0F);
	}

	inline float GetNormalizeG(void)
	{
		return (G / 255.0F);
	}

	inline float GetNormalizeB(void)
	{
		return (B / 255.0F);
	}

	inline float GetNormalizeA(void)
	{
		return (A / 255.0F);
	}

public:
	char R, G, B, A;
};