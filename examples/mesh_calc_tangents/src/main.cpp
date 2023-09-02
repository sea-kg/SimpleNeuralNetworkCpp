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
#include "CalcTangentSimpleNeuralNetwork.h"

std::vector<SimpleNeuralNetworkMeshPoint> findAllTriangles(SimpleNeuralNetworkMeshPoint p, const std::vector<SimpleNeuralNetworkMeshPoint> &vPoints) {
    std::vector<SimpleNeuralNetworkMeshPoint> vRet;
    vRet.push_back(p);
    std::vector<int> vTriangles;
    for (int i = 0; i < vPoints.size(); i++) {
        if (p.isEqual(vPoints[i])) {
            int trinagle_id = i / 3;
            if (std::find(vTriangles.begin(), vTriangles.end(), trinagle_id) == vTriangles.end()) {
                // std::cout << "Found new t = " << vPoints[i].trinagle_id << std::endl;
                vTriangles.push_back(trinagle_id);
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

struct MeshPointTangents {
    float tangent_x;
    float tangent_y;
    float tangent_z;
    float tangent_w;
};

void initTrainingData(SimpleNeuralTrainingItemList &trainingData) {
    std::string sFilename = "examples/mesh_calc_tangents/data.txt";
    std::cout << "Read training data from " << sFilename << std::endl;
    std::ifstream data;
    std::vector<SimpleNeuralNetworkMeshPoint> vPoints;
    std::vector<MeshPointTangents> vTangents;
    data.open(sFilename.c_str(), std::ios_base::in);  // open data
    for (std::string sLine; std::getline(data, sLine); ) {
        std::istringstream iLine(sLine);      //make a stream for the line itself
        float nX, nY, nZ;
        iLine >> nX >> nY >> nZ;
        vPoints.push_back(SimpleNeuralNetworkMeshPoint(nX, nY, nZ));

        MeshPointTangents t;
        iLine >> t.tangent_x;
        iLine >> t.tangent_y;
        iLine >> t.tangent_z;
        iLine >> t.tangent_w;

        vTangents.push_back(t);
    }

    for (int i = 0; i < vPoints.size(); i++) {
        std::vector<SimpleNeuralNetworkMeshPoint> vPointsIn = findAllTriangles(vPoints[i], vPoints);
        std::vector<SimpleNeuralNetworkMeshPoint> vPointsNormIn = normalizeNumPoints(vPointsIn);
        if (vPointsNormIn.size() != 19) {
            std::cout << "point " << i << " -> found points " << vPointsIn.size() << " norm " << vPointsNormIn.size() << std::endl;
        }

        std::vector<float> vIn = toVector(vPointsNormIn);

        std::vector<float> vOut;
        vOut.push_back(vTangents[i].tangent_x);
        vOut.push_back(vTangents[i].tangent_y);
        vOut.push_back(vTangents[i].tangent_z);
        vOut.push_back(vTangents[i].tangent_w);
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
        30, 71, 8, // middle layers
        // 64, 10, 64, // middle layers
        trainingData.getNumberOfOut()
    });

    // init first generation
    constexpr int nBetterSpecimens = 30;
    constexpr int nMutateSpecimens = 40;
    constexpr int nMixSpecimens = 40;
    SimpleNeuralGenomList genoms(nBetterSpecimens, nMutateSpecimens, nMixSpecimens);
    genoms.fillRandom(pNet); // TODO fill can be randomly, no need net
    std::cout << "First calc... " << std::endl;
    genoms.calculateRatingForAll(pNet, &trainingData);

    constexpr int nMaxGenerations = 5000;
    constexpr float nConditionRatingStop = 0.1f;
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

        pNet->setGenom(genoms.getBetterGenom().getGenom());
        pNet->exportToCppFunction(
            "CalcTangentSimpleNeuralNetwork.cpp",
            "calcTangentByPoints",
            "// " + std::to_string(n) + " generatoins, rate " + std::to_string(genoms.getBetterGenom().getRating()) + ", training for sphere"
        );
    }

    const std::vector<float> &vBetterGenom = genoms.list()[0].getGenom();

    std::cout << "calc avarage time: " << pNet->getCalcAvarageTimeInNanoseconds() << "ns" << std::endl;
	return 0;
}
