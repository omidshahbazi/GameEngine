float3 pos : POSITION;
float2 uv : UV;
const float2 elemDim;
const float2 texDim;
const float2 texBorder;
const texture2D difTex;
const matrix4 _MVP;

float4 VertexMain()
{
	return _MVP * float4(pos, 1);
}

float Map(float Value, float OriginalMin, float OriginalMax, float NewMin, float NewMax)
{
	return (((Value - OriginalMin) / (OriginalMax - OriginalMin)) * (NewMax - NewMin)) + NewMin;
}

float ProcessCoord(float Coord, float ElementBorder, float TextureBorder)
{
	if (Coord < ElementBorder)
		return Map(Coord, 0, ElementBorder, 0, TextureBorder);

	if (1 - ElementBorder < Coord)
		return Map(Coord, 1 - ElementBorder, 1, 1 - TextureBorder, 1);

	return Map(Coord, ElementBorder, 1 - ElementBorder, TextureBorder, 1 - TextureBorder);
}

float4 FragmentMain()
{
	float2 texBorderUV = (texBorder / texDim);
	float2 elemBorderUV = texBorderUV * (texDim / elemDim);

	return texture(difTex, float2(ProcessCoord(uv.x, elemBorderUV.x, texBorderUV.x), ProcessCoord(uv.y, elemBorderUV.y, texBorderUV.y)));
}