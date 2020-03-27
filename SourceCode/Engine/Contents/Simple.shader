float3 pos : POSITION;
float2 uv : UV;
const texture2D difTex;
const matrix4 _MVP;

float4 VertexMain()
{
	return _MVP * float4(pos, 1);
}

float4 FragmentMain()
{
	return texture(difTex, uv);
}