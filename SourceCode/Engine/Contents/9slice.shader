float3 pos : POSITION;
float2 uv : UV;
float2 elemDim;
float2 texDim;
float2 texBorder;
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
	float2 elementBorder = texBorder / texDim;

	return float2(
		MapCoord(uv.x, elementBorder.x),
		MapCoord(uv.y, elementBorder.x));
}
 
float4 FragmentMain()
{
	//return texture(difTex, MapUV());


	float2 elementBorder = texBorder / texDim;

	if (uv.x < elementBorder.x)
		return float4(1, 0, 0, 1);

	if (uv.x < 1 - elementBorder.x)
		return float4(1, 0, 0, 1);

	if (uv.y < elementBorder.y)
		return float4(1, 0, 0, 1);

	if (uv.y < 1 - elementBorder.y)
		return float4(1, 0, 0, 1);

	return texture(difTex, uv);
}