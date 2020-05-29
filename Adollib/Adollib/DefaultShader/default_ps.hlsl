#include "default.hlsli"

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 main(PSInput input) : SV_TARGET
{
	return 1;

	float3 L = normalize(LightDir.xyz);
	float3 E = normalize(EyePos.xyz - input.wPos);

	// 環境光
	float3 Ambient = AmbientColor.rgb;//
	//float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
	float4 color = float4(1, 0.2, 0.4, 1);
	float3 C = LightColor.rgb;		//入射光(色と強さ)

	// 法線
	float3 N = normalize(input.wNormal);

	// 拡散反射
	float3 Kd = float3(1, 1, 1);
	float3 D = Diffuse(N, L, C, Kd);

	// 鏡面反射
	float3 Ks = float3(1, 1, 1);
	float3 S = BlinnPhongSpecular(N, L, C, E, Ks, 20);


	//ポイントライト
	float3 PL;
	float3 PC;
	float influence;
	float3 PD = (float3)0;
	float3 PS = (float3)0;
	for (int i = 0; i < POINTMAX; i++) {
		if (PointLight[i].type == 0) continue;
		PL = input.wPos - PointLight[i].pos.xyz;
		float d = length(PL);

		float r = PointLight[i].range;
		if (d > r) continue;
		influence = saturate(1.0f - d / r);
		PL = normalize(PL);
		PC = PointLight[i].color.rgb;
		PD += Diffuse(N, PL, PC, Kd) * influence * influence;
		PS += BlinnPhongSpecular(N, PL, PC, E, Ks, 20) * influence * influence;
	}
	//スポットライト
	float3 SL;
	float3 SC;
	float influence2;
	float3 SD = (float3)0;
	float3 SS = (float3)0;
	for (int j = 0; j < SPOTMAX; j++) {
		if (SpotLight[j].type == 0) continue;

		SL = input.wPos - SpotLight[j].pos.xyz;
		float d = length(SL);

		float r = SpotLight[j].range;
		if (d > r) continue;
		float3 SFront = normalize(SpotLight[j].dir.xyz);
		SL = normalize(SL);
		float angle = dot(SL, SFront);
		float area = SpotLight[j].inner_corn - SpotLight[j].outer_corn;
		float influence3 = SpotLight[j].inner_corn - angle;
		influence3 = saturate(1.0 - influence3 / area);
		influence2 = 1;
		SC = SpotLight[j].color.rgb;
		SD += Diffuse(N, SL, SC, Kd) * influence2 * influence2 * influence3;
		SS += BlinnPhongSpecular(N, SL, SC, E, Ks, 20) * influence2 * influence2 * influence3;
	}


	return color * input.Color * float4(Ambient + D + S + PD + PS + SD + SS , 1.0f);
}