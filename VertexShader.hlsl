
cbuffer MyConstants
{
	matrix mat;
};

struct VSOut {
	float3 color : Color;
	float4 pos : SV_Position;
};


VSOut main(float3 p : Position, float3 color : Color)
{
	
	float3 pos = float3(p.x, p.y, p.z);

	//rotation shenanigans
	//pos.x = p.x * cosa - p.y * sina;//z
	//pos.y = p.x * sina + p.y * cosa;//x

	//pos.z = p.z * cosa - p.x * sina;
	//pos.x = p.z * sina + p.x * cosa;

	//pos.z = pos.z + 3.0f;
	//pos.x = pos.x*3/4/pos.z;
	//pos.y = pos.y/pos.z;

	VSOut output;

	output.pos = mul(float4(pos, 1.0f), mat);
	output.color = color;
	
	return output;
}