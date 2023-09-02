/*
MIT License

Copyright (c) 2022 Evgenii Sopov (mrseakg@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <vector>

class SimpleNeuralNetworkMeshPoint {
    public:
        SimpleNeuralNetworkMeshPoint();
        SimpleNeuralNetworkMeshPoint(float nX, float nY, float nZ);

        float getX() const;
        void setX(float nX);
        float getY() const;
        void setY(float nY);
        float getZ() const;
        void setZ(float nZ);

        bool isEqual(const SimpleNeuralNetworkMeshPoint &p) const;
        SimpleNeuralNetworkMeshPoint getMiddle(const SimpleNeuralNetworkMeshPoint &p) const;

    private:
        float m_nX = 0.0f;
        float m_nY = 0.0f;
        float m_nZ = 0.0f;
};

struct SimpleNeuralNetworkMeshTriangle {
    SimpleNeuralNetworkMeshPoint p0;
    SimpleNeuralNetworkMeshPoint p1;
    SimpleNeuralNetworkMeshPoint p2;

    void fillFromPointsByIndex(const std::vector<SimpleNeuralNetworkMeshPoint> &vPoints, int nStartIndex) {
        this->p0 = vPoints[nStartIndex];
        this->p1 = vPoints[nStartIndex + 1];
        this->p2 = vPoints[nStartIndex + 2];
    };

    void rotatePointsToMain(SimpleNeuralNetworkMeshPoint main_p) {
        if (main_p.isEqual(this->p0)) {
            return;
        }
        if (main_p.isEqual(this->p1)) {
            SimpleNeuralNetworkMeshPoint p = this->p1;
            this->p1 = this->p2;
            this->p2 = this->p0;
            this->p0 = p;
            return;
        }
        if (main_p.isEqual(this->p2)) {
            SimpleNeuralNetworkMeshPoint p = this->p2;
            this->p2 = this->p0;
            this->p0 = this->p1;
            this->p1 = p;
            return;
        }
    }
};

void squashOneRandomTriangle(std::vector<SimpleNeuralNetworkMeshTriangle> &vTriangles, const SimpleNeuralNetworkMeshPoint &main_p);
std::vector<SimpleNeuralNetworkMeshPoint> normalizeNumPoints(const std::vector<SimpleNeuralNetworkMeshPoint> &vPoints);
std::vector<float> toVector(const std::vector<SimpleNeuralNetworkMeshPoint> &vPoints);

// in 57 values, output 4 values
void calcTangentByPointsVIn(
    const std::vector<float> &vIn,
    float &out0, // x
    float &out1, // y
    float &out2, // z
    float &out3  // w
);

void calcTangentByPoints(
    const float &in0, // x
    const float &in1, // y
    const float &in2, // z
    const float &in3,
    const float &in4,
    const float &in5,
    const float &in6,
    const float &in7,
    const float &in8,
    const float &in9,
    const float &in10,
    const float &in11,
    const float &in12,
    const float &in13,
    const float &in14,
    const float &in15,
    const float &in16,
    const float &in17,
    const float &in18,
    const float &in19,
    const float &in20,
    const float &in21,
    const float &in22,
    const float &in23,
    const float &in24,
    const float &in25,
    const float &in26,
    const float &in27,
    const float &in28,
    const float &in29,
    const float &in30,
    const float &in31,
    const float &in32,
    const float &in33,
    const float &in34,
    const float &in35,
    const float &in36,
    const float &in37,
    const float &in38,
    const float &in39,
    const float &in40,
    const float &in41,
    const float &in42,
    const float &in43,
    const float &in44,
    const float &in45,
    const float &in46,
    const float &in47,
    const float &in48,
    const float &in49,
    const float &in50,
    const float &in51,
    const float &in52,
    const float &in53,
    const float &in54,
    const float &in55,
    const float &in56,
    float &out0, // x
    float &out1, // y
    float &out2, // z
    float &out3  // w
);