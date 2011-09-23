cbuffer cbPerObject
{
	float4x4 g_World;
	float4 g_Color;
};

cbuffer cbOnInitialize
{
	float4x4 g_ViewProjection;
};

float4 VS(float3 iPositionL	: POSITION) : SV_POSITION
{
	// Transform to homogeneous clip space.
	return mul(float4(iPositionL, 1.0f), mul(g_World, g_ViewProjection));
}

float4 PS(float4 iPositionH : SV_POSITION) : SV_Target
{       
	return g_Color;
}

technique10 BlockOutTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}
