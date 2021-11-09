
#ifndef  BayerMatrixFunc

static float bayer(in int iter, in float2 rc)
{
    float sum = 0.0;
    for (int i = 0; i < 4; ++i)
    {
        if (i >= iter)
            break;
        float2 bsize = float2(pow(2.0, float(i + 1)), pow(2.0, float(i + 1)));
        float2 t = (rc % bsize) / bsize;
        int idx = int(dot(floor(t * 2.0), float2(2.0, 1.0)));
        float b = 0.0;
        if (idx == 0)
        {
            b = 0.0;
        }
        else if (idx == 1)
        {
            b = 2.0;
        }
        else if (idx == 2)
        {
            b = 3.0;
        }
        else
        {
            b = 1.0;
        }
        sum += b * pow(4.0, float(iter - i - 1));
    }
    float phi = pow(4.0, float(iter)) + 1.0;
    return (sum + 1.0) / phi;
}

static const float2x2 BayerMatrix2x2 =
{
    bayer(2, float2(0, 0)), bayer(2, float2(1, 0)),
    bayer(2, float2(0, 1)), bayer(2, float2(1, 1))
};

static const float4x4 BayerMtrix4x4 =
{
 //   1.0 / 17.0, 9.0 / 17.0, 3.0 / 17.0, 11.0 / 17.0,
	//13.0 / 17.0, 5.0 / 17.0, 15.0 / 17.0, 7.0 / 17.0,
	//4.0 / 17.0, 12.0 / 17.0, 2.0 / 17.0, 10.0 / 17.0,
	//16.0 / 17.0, 8.0 / 17.0, 14.0 / 17.0, 6.0 / 17.0
    bayer(4, float2(0, 0)), bayer(4, float2(1, 0)), bayer(4, float2(2, 0)), bayer(4, float2(3, 0)),
    bayer(4, float2(0, 1)), bayer(4, float2(1, 1)), bayer(4, float2(2, 1)), bayer(4, float2(3, 1)),
    bayer(4, float2(0, 2)), bayer(4, float2(1, 2)), bayer(4, float2(2, 2)), bayer(4, float2(3, 2)),
    bayer(4, float2(0, 3)), bayer(4, float2(1, 3)), bayer(4, float2(2, 3)), bayer(4, float2(3, 3))
};

static const float BayerMtrix8x8[8][8] =
{
    bayer(8, float2(0, 0)), bayer(8, float2(1, 0)), bayer(8, float2(2, 0)), bayer(8, float2(3, 0)), bayer(8, float2(4, 0)), bayer(8, float2(5, 0)), bayer(8, float2(6, 0)), bayer(8, float2(7, 0)),
    bayer(8, float2(0, 1)), bayer(8, float2(1, 1)), bayer(8, float2(2, 1)), bayer(8, float2(3, 1)), bayer(8, float2(4, 1)), bayer(8, float2(5, 1)), bayer(8, float2(6, 1)), bayer(8, float2(7, 1)),
    bayer(8, float2(0, 2)), bayer(8, float2(1, 2)), bayer(8, float2(2, 2)), bayer(8, float2(3, 2)), bayer(8, float2(4, 2)), bayer(8, float2(5, 2)), bayer(8, float2(6, 2)), bayer(8, float2(7, 2)),
    bayer(8, float2(0, 3)), bayer(8, float2(1, 3)), bayer(8, float2(2, 3)), bayer(8, float2(3, 3)), bayer(8, float2(4, 3)), bayer(8, float2(5, 3)), bayer(8, float2(6, 3)), bayer(8, float2(7, 3)),
    bayer(8, float2(0, 4)), bayer(8, float2(1, 4)), bayer(8, float2(2, 4)), bayer(8, float2(3, 4)), bayer(8, float2(4, 4)), bayer(8, float2(5, 4)), bayer(8, float2(6, 4)), bayer(8, float2(7, 4)),
    bayer(8, float2(0, 5)), bayer(8, float2(1, 5)), bayer(8, float2(2, 5)), bayer(8, float2(3, 5)), bayer(8, float2(4, 5)), bayer(8, float2(5, 5)), bayer(8, float2(6, 5)), bayer(8, float2(7, 5)),
    bayer(8, float2(0, 6)), bayer(8, float2(1, 6)), bayer(8, float2(2, 6)), bayer(8, float2(3, 6)), bayer(8, float2(4, 6)), bayer(8, float2(5, 6)), bayer(8, float2(6, 6)), bayer(8, float2(7, 6)),
    bayer(8, float2(0, 7)), bayer(8, float2(1, 7)), bayer(8, float2(2, 7)), bayer(8, float2(3, 7)), bayer(8, float2(4, 7)), bayer(8, float2(5, 7)), bayer(8, float2(6, 7)), bayer(8, float2(7, 7))
};

static const float BayerMtrix16x16[16][16] =
{
    bayer(16, float2(0, 0)),  bayer(16, float2(1, 0)),  bayer(16, float2(2, 0)),  bayer(16, float2(3, 0)),  bayer(16, float2(4, 0)),  bayer(16, float2(5, 0)),  bayer(16, float2(6, 0)),  bayer(16, float2(7, 0)),   bayer(16, float2(8, 0)),  bayer(16, float2(9, 0)),  bayer(16, float2(10, 0)),  bayer(16, float2(11, 0)),  bayer(16, float2(12, 0)),  bayer(16, float2(13, 0)),  bayer(16, float2(14, 0)),  bayer(16, float2(15, 0)),
    bayer(16, float2(0, 1)),  bayer(16, float2(1, 1)),  bayer(16, float2(2, 1)),  bayer(16, float2(3, 1)),  bayer(16, float2(4, 1)),  bayer(16, float2(5, 1)),  bayer(16, float2(6, 1)),  bayer(16, float2(7, 1)),   bayer(16, float2(8, 1)),  bayer(16, float2(9, 1)),  bayer(16, float2(10, 1)),  bayer(16, float2(11, 1)),  bayer(16, float2(12, 1)),  bayer(16, float2(13, 1)),  bayer(16, float2(14, 1)),  bayer(16, float2(15, 1)),
    bayer(16, float2(0, 2)),  bayer(16, float2(1, 2)),  bayer(16, float2(2, 2)),  bayer(16, float2(3, 2)),  bayer(16, float2(4, 2)),  bayer(16, float2(5, 2)),  bayer(16, float2(6, 2)),  bayer(16, float2(7, 2)),   bayer(16, float2(8, 2)),  bayer(16, float2(9, 2)),  bayer(16, float2(10, 2)),  bayer(16, float2(11, 2)),  bayer(16, float2(12, 2)),  bayer(16, float2(13, 2)),  bayer(16, float2(14, 2)),  bayer(16, float2(15, 2)),
    bayer(16, float2(0, 3)),  bayer(16, float2(1, 3)),  bayer(16, float2(2, 3)),  bayer(16, float2(3, 3)),  bayer(16, float2(4, 3)),  bayer(16, float2(5, 3)),  bayer(16, float2(6, 3)),  bayer(16, float2(7, 3)),   bayer(16, float2(8, 3)),  bayer(16, float2(9, 3)),  bayer(16, float2(10, 3)),  bayer(16, float2(11, 3)),  bayer(16, float2(12, 3)),  bayer(16, float2(13, 3)),  bayer(16, float2(14, 3)),  bayer(16, float2(15, 3)),
    bayer(16, float2(0, 4)),  bayer(16, float2(1, 4)),  bayer(16, float2(2, 4)),  bayer(16, float2(3, 4)),  bayer(16, float2(4, 4)),  bayer(16, float2(5, 4)),  bayer(16, float2(6, 4)),  bayer(16, float2(7, 4)),   bayer(16, float2(8, 4)),  bayer(16, float2(9, 4)),  bayer(16, float2(10, 4)),  bayer(16, float2(11, 4)),  bayer(16, float2(12, 4)),  bayer(16, float2(13, 4)),  bayer(16, float2(14, 4)),  bayer(16, float2(15, 4)),
    bayer(16, float2(0, 5)),  bayer(16, float2(1, 5)),  bayer(16, float2(2, 5)),  bayer(16, float2(3, 5)),  bayer(16, float2(4, 5)),  bayer(16, float2(5, 5)),  bayer(16, float2(6, 5)),  bayer(16, float2(7, 5)),   bayer(16, float2(8, 5)),  bayer(16, float2(9, 5)),  bayer(16, float2(10, 5)),  bayer(16, float2(11, 5)),  bayer(16, float2(12, 5)),  bayer(16, float2(13, 5)),  bayer(16, float2(14, 5)),  bayer(16, float2(15, 5)),
    bayer(16, float2(0, 6)),  bayer(16, float2(1, 6)),  bayer(16, float2(2, 6)),  bayer(16, float2(3, 6)),  bayer(16, float2(4, 6)),  bayer(16, float2(5, 6)),  bayer(16, float2(6, 6)),  bayer(16, float2(7, 6)),   bayer(16, float2(8, 6)),  bayer(16, float2(9, 6)),  bayer(16, float2(10, 6)),  bayer(16, float2(11, 6)),  bayer(16, float2(12, 6)),  bayer(16, float2(13, 6)),  bayer(16, float2(14, 6)),  bayer(16, float2(15, 6)),
    bayer(16, float2(0, 7)),  bayer(16, float2(1, 7)),  bayer(16, float2(2, 7)),  bayer(16, float2(3, 7)),  bayer(16, float2(4, 7)),  bayer(16, float2(5, 7)),  bayer(16, float2(6, 7)),  bayer(16, float2(7, 7)),   bayer(16, float2(8, 7)),  bayer(16, float2(9, 7)),  bayer(16, float2(10, 7)),  bayer(16, float2(11, 7)),  bayer(16, float2(12, 7)),  bayer(16, float2(13, 7)),  bayer(16, float2(14, 7)),  bayer(16, float2(15, 7)),
    bayer(16, float2(0, 8)),  bayer(16, float2(1, 8)),  bayer(16, float2(2, 8)),  bayer(16, float2(3, 8)),  bayer(16, float2(4, 8)),  bayer(16, float2(5, 8)),  bayer(16, float2(6, 8)),  bayer(16, float2(7, 8)),   bayer(16, float2(8, 8)),  bayer(16, float2(9, 8)),  bayer(16, float2(10, 8)),  bayer(16, float2(11, 8)),  bayer(16, float2(12, 8)),  bayer(16, float2(13, 8)),  bayer(16, float2(14, 8)),  bayer(16, float2(15, 8)),
    bayer(16, float2(0, 9)),  bayer(16, float2(1, 9)),  bayer(16, float2(2, 9)),  bayer(16, float2(3, 9)),  bayer(16, float2(4, 9)),  bayer(16, float2(5, 9)),  bayer(16, float2(6, 9)),  bayer(16, float2(7, 9)),   bayer(16, float2(8, 9)),  bayer(16, float2(9, 9)),  bayer(16, float2(10, 9)),  bayer(16, float2(11, 9)),  bayer(16, float2(12, 9)),  bayer(16, float2(13, 9)),  bayer(16, float2(14, 9)),  bayer(16, float2(15, 9)),
    bayer(16, float2(0, 10)), bayer(16, float2(1, 10)), bayer(16, float2(2, 10)), bayer(16, float2(3, 10)), bayer(16, float2(4, 10)), bayer(16, float2(5, 10)), bayer(16, float2(6, 10)), bayer(16, float2(7, 10)),  bayer(16, float2(8, 10)), bayer(16, float2(9, 10)), bayer(16, float2(10, 10)), bayer(16, float2(11, 10)), bayer(16, float2(12, 10)), bayer(16, float2(13, 10)), bayer(16, float2(14, 10)), bayer(16, float2(15, 10)),
    bayer(16, float2(0, 11)), bayer(16, float2(1, 11)), bayer(16, float2(2, 11)), bayer(16, float2(3, 11)), bayer(16, float2(4, 11)), bayer(16, float2(5, 11)), bayer(16, float2(6, 11)), bayer(16, float2(7, 11)),  bayer(16, float2(8, 11)), bayer(16, float2(9, 11)), bayer(16, float2(10, 11)), bayer(16, float2(11, 11)), bayer(16, float2(12, 11)), bayer(16, float2(13, 11)), bayer(16, float2(14, 11)), bayer(16, float2(15, 11)),
    bayer(16, float2(0, 12)), bayer(16, float2(1, 12)), bayer(16, float2(2, 12)), bayer(16, float2(3, 12)), bayer(16, float2(4, 12)), bayer(16, float2(5, 12)), bayer(16, float2(6, 12)), bayer(16, float2(7, 12)),  bayer(16, float2(8, 12)), bayer(16, float2(9, 12)), bayer(16, float2(10, 12)), bayer(16, float2(11, 12)), bayer(16, float2(12, 12)), bayer(16, float2(13, 12)), bayer(16, float2(14, 12)), bayer(16, float2(15, 12)),
    bayer(16, float2(0, 13)), bayer(16, float2(1, 13)), bayer(16, float2(2, 13)), bayer(16, float2(3, 13)), bayer(16, float2(4, 13)), bayer(16, float2(5, 13)), bayer(16, float2(6, 13)), bayer(16, float2(7, 13)),  bayer(16, float2(8, 13)), bayer(16, float2(9, 13)), bayer(16, float2(10, 13)), bayer(16, float2(11, 13)), bayer(16, float2(12, 13)), bayer(16, float2(13, 13)), bayer(16, float2(14, 13)), bayer(16, float2(15, 13)),
    bayer(16, float2(0, 14)), bayer(16, float2(1, 14)), bayer(16, float2(2, 14)), bayer(16, float2(3, 14)), bayer(16, float2(4, 14)), bayer(16, float2(5, 14)), bayer(16, float2(6, 14)), bayer(16, float2(7, 14)),  bayer(16, float2(8, 14)), bayer(16, float2(9, 14)), bayer(16, float2(10, 14)), bayer(16, float2(11, 14)), bayer(16, float2(12, 14)), bayer(16, float2(13, 14)), bayer(16, float2(14, 14)), bayer(16, float2(15, 14)),
    bayer(16, float2(0, 15)), bayer(16, float2(1, 15)), bayer(16, float2(2, 15)), bayer(16, float2(3, 15)), bayer(16, float2(4, 15)), bayer(16, float2(5, 15)), bayer(16, float2(6, 15)), bayer(16, float2(7, 15)),  bayer(16, float2(8, 15)), bayer(16, float2(9, 15)), bayer(16, float2(10, 15)), bayer(16, float2(11, 15)), bayer(16, float2(12, 15)), bayer(16, float2(13, 15)), bayer(16, float2(14, 15)), bayer(16, float2(15, 15))
};


float Bayermatrix(float2 xy)
{
    //return BayerMtrix8x8[xy.x % 8][xy.y % 8];
    return BayerMtrix16x16[xy.x % 16][xy.y % 16];

}


#define BaryMatrixFunc
#endif