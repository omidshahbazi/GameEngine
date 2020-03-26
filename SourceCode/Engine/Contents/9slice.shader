float3 pos : POSITION;
float2 uv : UV;
const float2 border;
const texture2D difTex;
const matrix4 _MVP;

float4 VertexMain()
{
	return _MVP * float4(pos, 1);
}

float MapCoord(float Coord, float Border)
{
	if (Coord < Border)
		return 0;

	if (Coord < 1 - Border)
		return 0;

	return Coord;
}

float2 MapUV()
{
	return float2(
		MapCoord(uv.x, border.x),
		MapCoord(uv.y, border.y));
}

float4 FragmentMain()
{
	float2 newUV = uv;

	if (border.x < uv.x && uv.x < 1 - border.x)
		newUV.x = 0.5;

	if (border.y < uv.y && uv.y < 1 - border.y)
		newUV.y = 0.5;

	return texture(difTex, newUV);

	return texture(difTex, MapUV());
}