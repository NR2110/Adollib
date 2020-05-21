
//--------------------------------------------
//	拡散反射関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率(0～1.0)

//ランバートシェーディング
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// 負の値を０にする
	return K * C * D;
}

//--------------------------------------------
//	鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//ハーフベクトル
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}

//--------------------------------------------
//	鏡面反射関数（フォン）
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

//フォンシェーディング
float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	float3 R = reflect(L, N);
	R = normalize(R);
	float3 S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}

//--------------------------------------------
//	フォグ
//--------------------------------------------
//color		:現在の色
//viewPos	:視点
//Pos		:ピクセルの位置
//fogColor	:フォグの色
//Near		:フォグの近距離
//Far		:フォグの遠距離
float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, float Near, float Far)
{
	//フォグ計算
	float Len = length(viewPos - Pos);
	float fogalpha = saturate((Len - Near) / (Far - Near));
	return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}


//--------------------------------------------
//	半球ライト
//--------------------------------------------

float3 HemiSphereLight(float3 N, float3 SkyColor, float3 GroundColor)
{
	float skyblend = (N.y + 1.0) * 0.5;
	float groundblend = 1.0 - skyblend;
	return  SkyColor * skyblend + GroundColor * groundblend;
}

//--------------------------------------------
//	トゥーンシェーダ用ライト
//--------------------------------------------
//３階調
float3 ToonLight(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	//D = max(0, D);			// 負の値を０にする
	if (D < 0.3) D = 0.2;
	else if (D < 0.9) D = 0.6;
	else D = 1.4;


	return K * C * D;
}


//--------------------------------------------
//	トゥーンシェーダ用拡散反射関数
//--------------------------------------------
//２階調
float3 ToonDiffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	//D = max(0, D);			// 負の値を０にする
	if (D < 0.3) D = 0.2;
	else D = 0.6;


	return K * C * D;
}
//--------------------------------------------
//	トゥーンシェーダ用鏡面反射関数
//--------------------------------------------
//２階調

float3 ToonSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	float3 R = reflect(L, N);
	R = normalize(R);
	float S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	if (S > 0.5) S = 0.8;
	else S = 0.4;
	return S * K * C;
}

//--------------------------------------------
//	シャドーマッピング用関数
//--------------------------------------------

//ワールド座標をライト空間座標に変換
float3	GetShadowTex(float4x4 vp, float3 wPos)
{
	// 正規化デバイス座標系
	float4	wvpPos;
	wvpPos.xyz = wPos;
	wvpPos.w = 1;
	wvpPos = mul(vp, wvpPos);
	wvpPos /= wvpPos.w;
	// テクスチャ座標系
	wvpPos.y = -wvpPos.y;
	wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;

	return wvpPos.xyz;
}
//シャドーマップからライト空間座標に変換とZ値比較
float3	GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
{	// シャドウマップから深度を取り出す
	float d = st.Sample(ss, Tex.xy).r;
	// シャドウマップの深度値と現実の深度の比較
	Scolor = (Tex.z - d > Bias) ? Scolor : 1.0;
	return Scolor;
}

