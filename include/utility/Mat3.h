#pragma once
#include <array>


//quick chatgpt cpde 
struct Vec3 {
    float x, y, z;
};

struct Mat3 {
    std::array<float, 9> m;

    static Mat3 identity() {
        return Mat3{ {
            1,0,0,
            0,1,0,
            0,0,1
        } };
    }

    Mat3 operator*(Mat3 const& o) const {
        Mat3 r{};
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                float sum = 0;
                for (int k = 0; k < 3; ++k) {
                    sum += m[i * 3 + k] * o.m[k * 3 + j];
                }
                r.m[i * 3 + j] = sum;
            }
        }
        return r;
    }

    Vec3 operator*(Vec3 const& v) const {
        return {
            m[0] * v.x + m[1] * v.y + m[2] * v.z,
            m[3] * v.x + m[4] * v.y + m[5] * v.z,
            m[6] * v.x + m[7] * v.y + m[8] * v.z
        };
    }
};

inline Mat3 translate(float tx, float ty) {
    return Mat3{ {
        1, 0, tx,
        0, 1, ty,
        0, 0, 1
    } };
}

inline Mat3 scale(float sx, float sy) {
    return Mat3{ {
        sx,  0, 0,
         0, sy, 0,
         0,  0, 1
    } };
}

inline Mat3 rotate_deg(float deg) {
    float r = deg * 3.14159265f / 180.f;
    float c = std::cos(r), s = std::sin(r);
    return Mat3{ {
         c, -s, 0,
         s,  c, 0,
         0,  0, 1
    } };
}