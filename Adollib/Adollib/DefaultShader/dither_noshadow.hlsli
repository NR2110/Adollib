#include "default.hlsli"

cbuffer CBPerDither : register(b6)
{
    float dither_start;
    float dither_length;
    float2 CBPerDither_dummy;
};