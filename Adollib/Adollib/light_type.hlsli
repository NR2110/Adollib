//-----------------------------------------
//	�|�C���g���C�g�\����
//-----------------------------------------
struct POINTLIGHT {
	float index;//�g���ĂȂ�
	float range;
	float type;
	float dumy;
	float4 pos;
	float4 color;
};
#define POINTMAX 32
//-----------------------------------------
//	�X�|�b�g���C�g�\����
//-----------------------------------------
struct SPOTLIGHT {
	float index; //�g���ĂȂ�
	float range;//���̓͂��͈�
	float type; //�L����������
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
//	�ۉe�\����
//-----------------------------------------

struct CIRCLESHADOW {
	float index;//�g���ĂȂ�
	float range;//���̓͂��͈�
	float type; //�L����������
	float inner;
	float4 pos;
};
#define CIRCLEMAX 32

