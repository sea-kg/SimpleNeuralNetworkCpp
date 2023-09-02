#include "CalcTangentSimpleNeuralNetwork.h"

#include <iostream>

// SimpleNeuralNetworkMeshPoint

SimpleNeuralNetworkMeshPoint::SimpleNeuralNetworkMeshPoint(float nX, float nY, float nZ) {
    m_nX = nX;
    m_nY = nY;
    m_nZ = nZ;
}

SimpleNeuralNetworkMeshPoint::SimpleNeuralNetworkMeshPoint() : SimpleNeuralNetworkMeshPoint(0,0,0) {

}

float SimpleNeuralNetworkMeshPoint::getX() const {
    return m_nX;
}

void SimpleNeuralNetworkMeshPoint::setX(float nX) {
    m_nX = nX;
}

float SimpleNeuralNetworkMeshPoint::getY() const {
    return m_nY;
}

void SimpleNeuralNetworkMeshPoint::setY(float nY) {
    m_nY = nY;
}

float SimpleNeuralNetworkMeshPoint::getZ() const {
    return m_nZ;
}

void SimpleNeuralNetworkMeshPoint::setZ(float nZ) {
    m_nZ = nZ;
}

bool SimpleNeuralNetworkMeshPoint::isEqual(const SimpleNeuralNetworkMeshPoint &p) const {
    return
        p.getX() == m_nX
        && p.getY() == m_nY
        && p.getZ() == m_nZ
    ;
};

SimpleNeuralNetworkMeshPoint SimpleNeuralNetworkMeshPoint::getMiddle(const SimpleNeuralNetworkMeshPoint &p) const {
    return SimpleNeuralNetworkMeshPoint(
        (this->getX() + p.getX()) / 2.0,
        (this->getY() + p.getY()) / 2.0,
        (this->getZ() + p.getZ()) / 2.0
    );
}

void squashOneRandomTriangle(std::vector<SimpleNeuralNetworkMeshTriangle> &vTriangles, const SimpleNeuralNetworkMeshPoint &main_p) {
    int triangle_id0 = std::rand() % vTriangles.size();
    SimpleNeuralNetworkMeshTriangle t0 = vTriangles[triangle_id0];
    vTriangles.erase(std::next(vTriangles.begin(), triangle_id0));

    int triangle_id1 = -1;
    for (int i = 0; i < vTriangles.size(); i++) {
        if (vTriangles[i].p0.isEqual(main_p) && vTriangles[i].p1.isEqual(t0.p2)) {
            triangle_id1 = i;
            break;
        }
    }

    if (triangle_id1 != -1) {
        // std::cout << "squash (p1): " << triangle_id0 << " && " << triangle_id1 << std::endl;
        SimpleNeuralNetworkMeshTriangle t1 = vTriangles[triangle_id1];
        vTriangles.erase(std::next(vTriangles.begin(), triangle_id1));
        SimpleNeuralNetworkMeshTriangle t2;
        t2.p0 = main_p;
        t2.p1 = t0.p1;
        t2.p2 = t1.p2;
        vTriangles.push_back(t2);
        return;
    }

    for (int i = 0; i < vTriangles.size(); i++) {
        if (vTriangles[i].p0.isEqual(main_p) && vTriangles[i].p2.isEqual(t0.p1)) {
            triangle_id1 = i;
            break;
        }
    }

    if (triangle_id1 != -1) {
        // std::cout << "squash (p1): " << triangle_id0 << " && " << triangle_id1 << std::endl;
        SimpleNeuralNetworkMeshTriangle t1 = vTriangles[triangle_id1];
        vTriangles.erase(std::next(vTriangles.begin(), triangle_id1));
        SimpleNeuralNetworkMeshTriangle t2;
        t2.p0 = main_p;
        t2.p1 = t1.p1;
        t2.p2 = t0.p2;
        vTriangles.push_back(t2);
        return;
    }

    for (int i = 0; i < vTriangles.size(); i++) {
        if (vTriangles[i].p0.isEqual(main_p) && vTriangles[i].p1.isEqual(t0.p1)) {
            triangle_id1 = i;
            break;
        }
    }

    if (triangle_id1 != -1) {
        // std::cout << "squash (p1): " << triangle_id0 << " && " << triangle_id1 << std::endl;
        SimpleNeuralNetworkMeshTriangle t1 = vTriangles[triangle_id1];
        vTriangles.erase(std::next(vTriangles.begin(), triangle_id1));
        SimpleNeuralNetworkMeshTriangle t2;
        t2.p0 = main_p;
        t2.p1 = t1.p2;
        t2.p2 = t0.p2;
        vTriangles.push_back(t2);
        return;
    }

    for (int i = 0; i < vTriangles.size(); i++) {
        if (vTriangles[i].p0.isEqual(main_p) && vTriangles[i].p2.isEqual(t0.p2)) {
            triangle_id1 = i;
            break;
        }
    }

    if (triangle_id1 != -1) {
        // std::cout << "squash (p1): " << triangle_id0 << " && " << triangle_id1 << std::endl;
        SimpleNeuralNetworkMeshTriangle t1 = vTriangles[triangle_id1];
        vTriangles.erase(std::next(vTriangles.begin(), triangle_id1));
        SimpleNeuralNetworkMeshTriangle t2;
        t2.p0 = main_p;
        t2.p1 = t1.p1;
        t2.p2 = t0.p1;
        vTriangles.push_back(t2);
        return;
    }

    if (triangle_id1 == -1) {
        vTriangles.push_back(t0);
        std::cout << "Could not squash " << vTriangles.size() << std::endl;
    }
}

std::vector<SimpleNeuralNetworkMeshPoint> normalizeNumPoints(const std::vector<SimpleNeuralNetworkMeshPoint> &vPointsIn) {
    if (vPointsIn.size() == 19) {
        return vPointsIn;
    }
    SimpleNeuralNetworkMeshPoint main_p = vPointsIn[0];

    std::vector<SimpleNeuralNetworkMeshTriangle> vTriangles;
    for (int i = 1; i < vPointsIn.size(); i += 3) {
        SimpleNeuralNetworkMeshTriangle t;
        t.fillFromPointsByIndex(vPointsIn, i);
        t.rotatePointsToMain(main_p);
        vTriangles.push_back(t);
    }

    // more triangles
    while (vTriangles.size() < 6) {
        SimpleNeuralNetworkMeshTriangle t0 = vTriangles.back();
        vTriangles.pop_back();
        SimpleNeuralNetworkMeshPoint middle_p = t0.p1.getMiddle(t0.p2);

        SimpleNeuralNetworkMeshTriangle t1;
        t1.p0 = t0.p0;
        t1.p1 = t0.p1;
        t1.p2 = middle_p;
        vTriangles.push_back(t1);

        SimpleNeuralNetworkMeshTriangle t2;
        t2.p0 = t0.p0;
        t2.p1 = middle_p;
        t2.p2 = t0.p2;
        vTriangles.push_back(t2);

    }

    // TODO safe cicles
    int nSafeCicles = 0;
    while (vTriangles.size() > 6) {
        squashOneRandomTriangle(vTriangles, main_p);
        nSafeCicles++;
        if (nSafeCicles > 100) {
            std::cout << "nSafeCicles break;" << std::endl;
            break;
        }
    }

    if (vTriangles.size() != 6) {
        std::cout << "Wrong number of triangles" << std::endl;
    }
    while (vTriangles.size() < 6) {
        vTriangles.push_back(vTriangles.back());
    }

    std::vector<SimpleNeuralNetworkMeshPoint> vRet;
    vRet.push_back(main_p);
    for (int i = 0; i < vTriangles.size(); i++) {
        vRet.push_back(vTriangles[i].p0);
        vRet.push_back(vTriangles[i].p1);
        vRet.push_back(vTriangles[i].p2);
    }

    return vRet;
}

std::vector<float> toVector(const std::vector<SimpleNeuralNetworkMeshPoint> &vPoints) {
    std::vector<float> vRet;
    for (int i = 0; i < vPoints.size(); i++) {
        vRet.push_back(vPoints[i].getX());
        vRet.push_back(vPoints[i].getY());
        vRet.push_back(vPoints[i].getZ());
    }
    return vRet;
}

// in 57 values, output 4 values

void calcTangentByPointsVIn(
    const std::vector<float> &vIn,
    float &out0,
    float &out1,
    float &out2,
    float &out3
) {
    if (vIn.size() != 57) {
        std::cout << "Wrong size for vIn" << std::endl;
        return;
    }
    calcTangentByPoints(
        vIn[0],
        vIn[1],
        vIn[2],
        vIn[3],
        vIn[4],
        vIn[5],
        vIn[6],
        vIn[7],
        vIn[8],
        vIn[9],
        vIn[10],
        vIn[11],
        vIn[12],
        vIn[13],
        vIn[14],
        vIn[15],
        vIn[16],
        vIn[17],
        vIn[18],
        vIn[19],
        vIn[20],
        vIn[21],
        vIn[22],
        vIn[23],
        vIn[24],
        vIn[25],
        vIn[26],
        vIn[27],
        vIn[28],
        vIn[29],
        vIn[30],
        vIn[31],
        vIn[32],
        vIn[33],
        vIn[34],
        vIn[35],
        vIn[36],
        vIn[37],
        vIn[38],
        vIn[39],
        vIn[40],
        vIn[41],
        vIn[42],
        vIn[43],
        vIn[44],
        vIn[45],
        vIn[46],
        vIn[47],
        vIn[48],
        vIn[49],
        vIn[50],
        vIn[51],
        vIn[52],
        vIn[53],
        vIn[54],
        vIn[55],
        vIn[56],
        out0,
        out1,
        out2,
        out3
   );
}

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
) {
    // nothing now
};