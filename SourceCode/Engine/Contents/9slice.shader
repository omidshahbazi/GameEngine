float3 pos : POSITION;
float2 uv : UV; 
//float2 elemDim; 
//float2 texDim;
//float2 sliceBorder;
const texture2D difTex;
const matrix4 _MVP; 

float4 VertexMain()
{ 
	return _MVP * float4(pos, 1); 
}

float4 FragmentMain()
{ 
	//float2 br = sliceBorder / texDim;
	return texture(difTex, uv);
}