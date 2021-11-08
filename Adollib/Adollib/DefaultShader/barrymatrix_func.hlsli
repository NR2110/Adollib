
#ifndef BaryMatrixFunc
float2x2 BaryMatrix2x2 = float2x2
(
 0, 2,
 3, 1
) * 0.25f;

static const float4x4 BarymMtrix4x4 =
{
    1.0 / 17.0, 9.0 / 17.0, 3.0 / 17.0, 11.0 / 17.0,
	13.0 / 17.0, 5.0 / 17.0, 15.0 / 17.0, 7.0 / 17.0,
	4.0 / 17.0, 12.0 / 17.0, 2.0 / 17.0, 10.0 / 17.0,
	16.0 / 17.0, 8.0 / 17.0, 14.0 / 17.0, 6.0 / 17.0
};


float Barrymatrix(float2 xy)
{
    return BarymMtrix4x4[xy.x % 4][xy.y % 4];
}


#define BaryMatrixFunc
#endif