/**
 * File Name: MyQuaternion.cpp
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 20/5/2025
 * Description: This is the MyQuaternion.cpp
 * Copyright (c) 2025 Alexandre Kévin DE FREITAS MARTINS
 * Version: 1.0.0
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the 'Software'), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "MyQuaternion.h"

namespace cyclone {
    MyQuaternion::MyQuaternion(const float angle, const Vector3 &axis)
        : Quaternion() {
        constexpr float degrees2Radians = 3.141592f / 180;

        const double cosAng = cos(degrees2Radians * angle / 2.0);

        const double sinAng = sin(degrees2Radians * angle / 2.0);

        const double norm = sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);

        i = axis[0] / norm;
        j = axis[1] / norm;
        k = axis[2] / norm;
        r = cosAng; //w
        i *= sinAng; //x
        j *= sinAng; //y
        k *= sinAng; //z
    }

    MyQuaternion MyQuaternion::slerp(const MyQuaternion &q1, const MyQuaternion &q2, float u) {
        MyQuaternion result;

        float dotProd = q1.r * q2.r + q1.i * q2.i + q1.j * q2.j + q1.k * q2.k;

        float theta;

        if (dotProd < 0) {
            theta = acos(-dotProd);
        } else {
            theta = acos(dotProd);
        }

        const float sinTheta = sin(theta);

        if (fabs(sinTheta) < TRIG_ANGLE_TOL) {
            result = q1;

            return (result);
        }

        float coeff1 = sin((1.0 - u) * theta) / sinTheta;
        float coeff2 = sin(u * theta) / sinTheta;

        if (dotProd < 0) {
            result.r = -coeff1 * q1.r + coeff2 * q2.r;
            result.i = -coeff1 * q1.i + coeff2 * q2.i;
            result.j = -coeff1 * q1.j + coeff2 * q2.j;
            result.k = -coeff1 * q1.k + coeff2 * q2.k;
        } else {
            result.r = coeff1 * q1.r + coeff2 * q2.r;
            result.i = coeff1 * q1.i + coeff2 * q2.i;
            result.j = coeff1 * q1.j + coeff2 * q2.j;
            result.k = coeff1 * q1.k + coeff2 * q2.k;
        }

        return (result);
    }
}
