float3 pos : POSITION;
float2 uv : UV;
const float2 elemDim;
const float2 texDim;
const float4 texBorder;
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

float ProcessCoord(float Coord, float ElementBorderMin, float ElementBorderMax, float TextureBorderMin, float TextureBorderMax)
{
	if (Coord < ElementBorderMin)
		return Map(Coord, 0, ElementBorderMin, 0, TextureBorderMin);

	if (1 - ElementBorderMax < Coord)
		return Map(Coord, 1 - ElementBorderMax, 1, 1 - TextureBorderMax, 1);

	return Map(Coord, ElementBorderMin, 1 - ElementBorderMax, TextureBorderMin, 1 - TextureBorderMax);
}

float4 FragmentMain()
{
	float4 texBorder4 = float4(texBorder.x, texBorder.y, texBorder.x, texBorder.y);
	float4 texDim4 = float4(texDim.x, texDim.y, texDim.x, texDim.y);
	float4 texBorderUV = (texBorder4 / texDim4);
	float4 elemBorderUV = texBorderUV * (texDim4 / float4(elemDim.x, elemDim.y, elemDim.x, elemDim.y));

	return texture(difTex, 
		float2(
			ProcessCoord(uv.x, elemBorderUV.x, elemBorderUV.z, texBorderUV.x, texBorderUV.z),
			ProcessCoord(uv.y, elemBorderUV.y, elemBorderUV.w, texBorderUV.y, texBorderUV.w)));
}