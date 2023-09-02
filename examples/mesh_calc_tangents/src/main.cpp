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

#include <stdint.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <fstream>
#include <string>

#include "SimpleNeuralNetwork.h"


struct MeshPoint {
    int trinagle_id;
    float vertex_x;
    float vertex_y;
    float vertex_z;
    float tangent_x;
    float tangent_y;
    float tangent_z;
    float tangent_w;
    bool equal(const MeshPoint &p) {
        return
            p.vertex_x == this->vertex_x
            && p.vertex_y == this->vertex_y
            && p.vertex_z == this->vertex_z
        ;
    };
};

std::vector<MeshPoint> findAllTriangles(MeshPoint p, const std::vector<MeshPoint> &vPoints) {
    std::vector<MeshPoint> vRet;
    vRet.push_back(p);
    std::vector<int> vTriangles;
    for (int i = 0; i < vPoints.size(); i++) {
        if (p.equal(vPoints[i])) {
            if (std::find(vTriangles.begin(), vTriangles.end(), vPoints[i].trinagle_id) == vTriangles.end()) {
                // std::cout << "Found new t = " << vPoints[i].trinagle_id << std::endl;
                vTriangles.push_back(vPoints[i].trinagle_id);
            }
        }
    }
    for (int i = 0; i < vTriangles.size(); i++) {
        int trinagle_id = vTriangles[i];
        vRet.push_back(vPoints[trinagle_id*3 + 0]);
        vRet.push_back(vPoints[trinagle_id*3 + 1]);
        vRet.push_back(vPoints[trinagle_id*3 + 2]);
    }
    return vRet;
}

MeshPoint calcMiddlePoint(const MeshPoint &p1, const MeshPoint &p2) {
    MeshPoint ret;
    ret.vertex_x = (p1.vertex_x + p2.vertex_x) / 2.0;
    ret.vertex_y = (p1.vertex_y + p2.vertex_y) / 2.0;
    ret.vertex_z = (p1.vertex_z + p2.vertex_z) / 2.0;
    return ret;
}

struct MeshTriangle {
    MeshPoint p0;
    MeshPoint p1;
    MeshPoint p2;

    void fillFromLastAndRemove(std::vector<MeshPoint> &vPoints) {
        this->p0 = vPoints.back();
        vPoints.pop_back();
        this->p1 = vPoints.back();
        vPoints.pop_back();
        this->p2 = vPoints.back();
        vPoints.pop_back();
    };

    void fillFromPointsByIndex(const std::vector<MeshPoint> &vPoints, int nStartIndex) {
        this->p0 = vPoints[nStartIndex];
        this->p1 = vPoints[nStartIndex + 1];
        this->p2 = vPoints[nStartIndex + 2];
    };

    void rotatePointsToMain(MeshPoint main_p) {
        if (main_p.equal(this->p0)) {
            return;
        }
        if (main_p.equal(this->p1)) {
            MeshPoint p = this->p1;
            this->p1 = this->p2;
            this->p2 = this->p0;
            this->p0 = p;
            return;
        }
        if (main_p.equal(this->p2)) {
            MeshPoint p = this->p2;
            this->p2 = this->p0;
            this->p0 = this->p1;
            this->p1 = p;
            return;
        }
    }
};

void squashOneRandomTriangle(std::vector<MeshTriangle> &vTriangles, const MeshPoint &main_p) {
    int triangle_id0 = std::rand() % vTriangles.size();
    MeshTriangle t0 = vTriangles[triangle_id0];
    vTriangles.erase(std::next(vTriangles.begin(), triangle_id0));

    int triangle_id1 = -1;
    for (int i = 0; i < vTriangles.size(); i++) {
        if (vTriangles[i].p0.equal(main_p) && vTriangles[i].p1.equal(t0.p2)) {
            triangle_id1 = i;
            break;
        }
    }

    if (triangle_id1 != -1) {
        // std::cout << "squash (p1): " << triangle_id0 << " && " << triangle_id1 << std::endl;
        MeshTriangle t1 = vTriangles[triangle_id1];
        vTriangles.erase(std::next(vTriangles.begin(), triangle_id1));
        MeshTriangle t2;
        t2.p0 = main_p;
        t2.p1 = t0.p1;
        t2.p2 = t1.p2;
        vTriangles.push_back(t2);
        return;
    }

    for (int i = 0; i < vTriangles.size(); i++) {
        if (vTriangles[i].p0.equal(main_p) && vTriangles[i].p2.equal(t0.p1)) {
            triangle_id1 = i;
            break;
        }
    }

    if (triangle_id1 != -1) {
        // std::cout << "squash (p1): " << triangle_id0 << " && " << triangle_id1 << std::endl;
        MeshTriangle t1 = vTriangles[triangle_id1];
        vTriangles.erase(std::next(vTriangles.begin(), triangle_id1));
        MeshTriangle t2;
        t2.p0 = main_p;
        t2.p1 = t1.p1;
        t2.p2 = t0.p2;
        vTriangles.push_back(t2);
        return;
    }

    if (triangle_id1 == -1) {
        std::cout << "Could not squash" << std::endl;
    }
}

std::vector<MeshPoint> normalizeNumPoints(const std::vector<MeshPoint> &vPointsIn) {
    if (vPointsIn.size() == 19) {
        return vPointsIn;
    }
    MeshPoint main_p = vPointsIn[0];

    std::vector<MeshTriangle> vTriangles;
    for (int i = 1; i < vPointsIn.size(); i += 3) {
        MeshTriangle t;
        t.fillFromPointsByIndex(vPointsIn, i);
        t.rotatePointsToMain(main_p);
        vTriangles.push_back(t);
    }

    // more triangles
    while (vTriangles.size() < 6) {
        MeshTriangle t0 = vTriangles.back();
        vTriangles.pop_back();
        MeshPoint middle_p = calcMiddlePoint(t0.p1, t0.p2);

        MeshTriangle t1;
        t1.p0 = t0.p0;
        t1.p1 = t0.p1;
        t1.p2 = middle_p;
        vTriangles.push_back(t1);

        MeshTriangle t2;
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
            break;
        }
    }
    if (vTriangles.size() != 6) {
        std::cout << "Wrong nimber of triangles" << std::endl;
        exit(1);
    }

    std::vector<MeshPoint> vRet;
    vRet.push_back(main_p);
    for (int i = 0; i < vTriangles.size(); i++) {
        vRet.push_back(vTriangles[i].p0);
        vRet.push_back(vTriangles[i].p1);
        vRet.push_back(vTriangles[i].p2);
    }

    return vRet;
}

void initTrainingData(SimpleNeuralTrainingItemList &trainingData) {
    std::string sFilename = "examples/mesh_calc_tangents/data.txt";
    std::cout << "Read training data from " << sFilename << std::endl;
    std::ifstream data;
    std::vector<MeshPoint> vPoints;
    data.open(sFilename.c_str(), std::ios_base::in);  // open data
    int i = 0;
    for (std::string sLine; std::getline(data, sLine); ) {
        std::istringstream iLine(sLine);      //make a stream for the line itself
        MeshPoint p;
        iLine >> p.vertex_x;
        iLine >> p.vertex_y;
        iLine >> p.vertex_z;
        iLine >> p.tangent_x;
        iLine >> p.tangent_y;
        iLine >> p.tangent_z;
        iLine >> p.tangent_w;
        p.trinagle_id = i / 3;
        // std::cout << "p.trinagle_id = " << p.trinagle_id << std::endl;
        // trainingData.addItem(vIn, {out1, out2});
        vPoints.push_back(p);
        i++;
    }

    for (int i = 0; i < vPoints.size(); i++) {
        std::vector<MeshPoint> vPointsIn = findAllTriangles(vPoints[i], vPoints);
        std::vector<MeshPoint> vPointsNormIn = normalizeNumPoints(vPointsIn);
        if (vPointsNormIn.size() != 19) {
            std::cout << "point " << i << " -> found points " << vPointsIn.size() << " norm " << vPointsNormIn.size() << std::endl;
        }

        std::vector<float> vIn;
        std::vector<float> vOut;

        for (int i0 = 0; i0 < vPointsNormIn.size(); i0++) {
            vIn.push_back(vPointsNormIn[i0].vertex_x);
            vIn.push_back(vPointsNormIn[i0].vertex_y);
            vIn.push_back(vPointsNormIn[i0].vertex_z);
        }
        vOut.push_back(vPointsNormIn[0].tangent_x);
        vOut.push_back(vPointsNormIn[0].tangent_y);
        vOut.push_back(vPointsNormIn[0].tangent_z);
        vOut.push_back(vPointsNormIn[0].tangent_w);
        trainingData.addItem(vIn, vOut);
    }
    std::cout << "Data for training: " << trainingData.size() << std::endl;
}

int main(int argc, char *argv[]) {
	std::srand(std::time(nullptr));

    constexpr int nNumberOfIn = 57; // 19 points == 6 triangles + 1 main point
    constexpr int nNumberOfOut = 4;

    SimpleNeuralTrainingItemList trainingData(nNumberOfIn, nNumberOfOut);
    initTrainingData(trainingData);

    std::cout << "Init net... " << std::endl;
	SimpleNeuralNetwork *pNet = new SimpleNeuralNetwork({
        trainingData.getNumberOfIn(),
        128, 512, 64, // middle layers
        // 64, 10, 64, // middle layers
        trainingData.getNumberOfOut()
    });

    // init first generation
    constexpr int nBetterSpecimens = 30;
    constexpr int nMutateSpecimens = 40;
    constexpr int nMixSpecimens = 40;
    SimpleNeuralGenomList genoms(nBetterSpecimens, nMutateSpecimens, nMixSpecimens);
    genoms.fillRandom(pNet); // TODO fill can be randomly, no need net
    genoms.calculateRatingForAll(pNet, &trainingData);

    constexpr int nMaxGenerations = 5000;
    constexpr float nConditionRatingStop = 2.0f;
    int n = 0;
    std::cout << "Start learning... " << std::endl;
    while (genoms.getBetterRating() > nConditionRatingStop && n < nMaxGenerations) {
        ++n;

        auto start = std::chrono::steady_clock::now();
        genoms.sort(); // better generations will be on the top

        std::cout << " ------- Generation " << n << " ------- " << std::endl;
        genoms.printFirstRatings(3);
        genoms.mutateAndMix(pNet);

        // calc rating
        genoms.calculateRatingForMutatedAndMixed(pNet, &trainingData);

        auto end = std::chrono::steady_clock::now();
        std::cout
            << "Elapsed time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms" << std::endl
        ;
        std::cout << "calc avarage time: " << pNet->getCalcAvarageTimeInNanoseconds() << "ns" << std::endl;
    }

    const std::vector<float> &vBetterGenom = genoms.list()[0].getGenom();

    std::ofstream file;
    file.open("best_genom_128_512_64.txt", std::ofstream::out | std::ofstream::app);
    for (int i=0; i < vBetterGenom.size(); ++i) {
        file << vBetterGenom[i] << " ";
    }
    file << std::endl;
    file.close();

    std::cout << "calc avarage time: " << pNet->getCalcAvarageTimeInNanoseconds() << "ns" << std::endl;
	return 0;
}
