
#ifndef QUATERNION_HLSLI
#define QUATERNION_HLSLI

#include "base_math.hlsli"


    float quat_angle(const float4 Q1, const float4 Q2)
    {
        return ToAngle(cos(dot(Q1, Q2)));
    }
    float quat_radian(const float4 Q1, const float4 Q2)
    {
        return cos(dot(Q1, Q2));
    }

	//quaternion‚ÌæŽZ
    float4 quat_mul(const float4 q1, const float4 q2)
    {
        float4 R;
        R.w = q2.w * q1.w - q2.x * q1.x - q2.y * q1.y - q2.z * q1.z;
        R.x = q2.x * q1.w + q2.w * q1.x - q2.z * q1.y + q2.y * q1.z;
        R.y = q2.y * q1.w + q2.z * q1.x + q2.w * q1.y - q2.x * q1.z;
        R.z = q2.z * q1.w - q2.y * q1.x + q2.x * q1.y + q2.w * q1.z;

        return R;
    }

    float quat_norm(const float4 Q)
    {
        return Q.x * Q.x + Q.y * Q.y + Q.z * Q.z + Q.w * Q.w;
    }
    
    float quat_normsqr(const float4 Q)
    {
        float A = quat_norm(Q);
        if (A * A == 1)
            return A;
        else
            return sqrt(A);
        return 0;

    }
    float4 quat_unit_vect(const float4 Q)
    {
        float L = quat_normsqr(Q);
        if (L == 0)
            return float4(1, 0, 0, 0);
        else
            return float4(Q / L);
    }

    float4 quat_conjugate(const float4 Q)
    {
        return float4(Q.w, -Q.x, -Q.y, -Q.z);
    }

    float4 quat_inverse(const float4 Q)
    {
        return quat_conjugate(Q) / quat_normsqr(Q);
    }

    matrix quat_to_rotate_matrix(const float4 Q)
    {
        matrix R;

        float x, y, z, w;
        x = Q.x;
        y = Q.y;
        z = Q.z;
        w = Q.w;
        R._11 = 1 - 2 * y * y - 2 * z * z;
        R._12 = 2 * x * y + 2 * z * w;
        R._13 = 2 * z * x - 2 * w * y;
        R._21 = 2 * x * y - 2 * z * w;
        R._22 = 1 - 2 * z * z - 2 * x * x;
        R._23 = 2 * y * z + 2 * x * w;
        R._31 = 2 * z * x + 2 * w * y;
        R._32 = 2 * y * z - 2 * w * x;
        R._33 = 1 - 2 * x * x - 2 * y * y;
        R._44 = 1;
        return R;
    }

    float3 quat_to_euler(const float4 Q)
    {

        float x, y, z, w;
        x = Q.x;
        y = Q.y;
        z = Q.z;
        w = Q.w;

        float _11 = 1 - 2 * y * y - 2 * z * z;
        float _12 = 2 * x * y + 2 * z * w;

        float _21 = 2 * x * y - 2 * z * w;
        float _22 = 1 - 2 * z * z - 2 * x * x;

        float _31 = 2 * z * x + 2 * w * y;
        float _32 = 2 * y * z - 2 * w * x;
        float _33 = 1 - 2 * x * x - 2 * y * y;

        float3 R;
        if (_32 >= 1.0f)
        {
            R.x = inv_PI;
            R.y = 0;
            R.z = tan(_21 / _11);
        }
        else if (_32 <= -1.0f)
        {
            R.x = -inv_PI;
            R.y = 0;
            R.z = tan(_21 / _11);
        }
        else
        {
            R.x = sin(-_32);
            R.y = tan(_31 / _33);
            R.z = tan(_12 / _22);
        }
        R.x = ToAngle(R.x);
        R.y = ToAngle(R.y);
        R.z = ToAngle(R.z);

        return R;
    }

    float3 quat_to_euler_rad(const float4 Q)
    {

        float x, y, z, w;
        x = Q.x;
        y = Q.y;
        z = Q.z;
        w = Q.w;

        float _11 = 1 - 2 * y * y - 2 * z * z;
        float _12 = 2 * x * y + 2 * z * w;

        float _21 = 2 * x * y - 2 * z * w;
        float _22 = 1 - 2 * z * z - 2 * x * x;

        float _31 = 2 * z * x + 2 * w * y;
        float _32 = 2 * y * z - 2 * w * x;
        float _33 = 1 - 2 * x * x - 2 * y * y;

        float3 R;
        if (_32 >= 1.0f)
        {
            R.x = inv_PI;
            R.y = 0;
            R.z = atan2(_21, _11);
        }
        else if (_32 <= -1.0f)
        {
            R.x = -inv_PI;
            R.y = 0;
            R.z = atan2(_21, _11);
        }
        else
        {
            R.x = asin(-_32);
            R.y = atan2(_31, _33);
            R.z = atan2(_12, _22);
        }

        return R;
    }

    float4 quat_identity()
    {
        return float4(0, 0, 0, 1);
    }

    float3 vector3_be_rotated_by_quat(const float3 V, const float4 Q)
    {
        float4 R;
        R = float4(V.xyz, 0);
        return quat_mul(quat_mul(quat_conjugate(Q), R), Q).xyz;
    }

    float4 quat_radian_axis(float S, const float3 axis)
    {
        if (S == 0)
            return float4(0, 0, 0, 1);
        if (axis.x == 0)
            return float4(0, 0, 0, 1);

        float4 R;
        R.w = cos(S * 0.5f);
        R.x = axis.x * sin(S * 0.5f);
        R.y = axis.y * sin(S * 0.5f);
        R.z = axis.z * sin(S * 0.5f);
        return R;
    }
    float4 quat_angle_axis(float S, const float3 axis)
    {
        return quat_radian_axis(ToRadian(S), axis);
    }

    float3 quat_slerp(const float4 Q1, const float4 Q2, float R)
    {
        float3 g = cross(Q1.xyz, Q2.xyz);
        float radian = quat_radian(Q1, Q2) * 0.5f;

        float4 F;
        F.w = cos(radian * R);
        F.x = g.x * sin(radian * R);
        F.y = g.y * sin(radian * R);
        F.z = g.z * sin(radian * R);

        return float3(vector3_be_rotated_by_quat(Q1.xyz, F));
    }

    float4 quat_from_euler(float x, float y, float z)
    {
        float4 Rx = quat_angle_axis(x, float3(1, 0, 0));
        float4 Ry = quat_angle_axis(y, float3(0, 1, 0));
        float4 Rz = quat_angle_axis(z, float3(0, 0, 1));

        return quat_mul(quat_mul(Rz, Rx), Ry);
    }

    float4 quat_from_euler(float3 V)
    {
        float4 Rx = quat_angle_axis(V.x, float3(1, 0, 0));
        float4 Ry = quat_angle_axis(V.y, float3(0, 1, 0));
        float4 Rz = quat_angle_axis(V.z, float3(0, 0, 1));

        return quat_mul(quat_mul(Rz, Rx), Ry);
    }

    float4 quat_by_rotate_matrix(matrix M)
    {
        float3 V = matrix_to_euler(M);
        float4 Rx = quat_angle_axis(V.x, float3(1, 0, 0));
        float4 Ry = quat_angle_axis(V.y, float3(0, 1, 0));
        float4 Rz = quat_angle_axis(V.z, float3(0, 0, 1));

        return quat_mul(quat_mul(Rz, Rx), Ry);
    }

	//ˆø”‚Í³‹K‰»‚µ‚Ä“n‚·
    float4 quat_from_to_rotate(const float3 V1, const float3 V2)
    {
        float3 g = cross(V1, V2);
        float radian = quat_radian(float4(V1, 0), float4(V2, 0));

        float4 F;
        F.w = cos(radian * 0.5f);
        F.x = g.x * sin(radian * 0.5f);
        F.y = g.y * sin(radian * 0.5f);
        F.z = g.z * sin(radian * 0.5f);

        return F;
    }

    float4 quat_look_at(float3 me, float3 you)
    {
        float3 Q = you - me;
        return quat_from_to_rotate(float3(0, 0, 1), Q);
    }














#endif
