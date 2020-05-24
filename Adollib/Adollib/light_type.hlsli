//-----------------------------------------
//	ポイントライト構造体
//-----------------------------------------
struct POINTLIGHT {
	float index;//使ってない
	float range;
	float type;
	float dumy;
	float4 pos;
	float4 color;
};
#define POINTMAX 32
//-----------------------------------------
//	スポットライト構造体
//-----------------------------------------
struct SPOTLIGHT {
	float index; //使ってない
	float range;//光の届く範囲
	float type; //有効か無効か
	float inner_corn;
	float outer_corn;
	float dumy0;
	float dumy1;
	float dumy2;
	float4 pos;
	float4 color;
	float4 dir;
};
#define SPOTMAX 32
//-----------------------------------------
//	丸影構造体
//-----------------------------------------

struct CIRCLESHADOW {
	float index;//使ってない
	float range;//光の届く範囲
	float type; //有効か無効か
	float inner;
	float4 pos;
};
#define CIRCLEMAX 32

