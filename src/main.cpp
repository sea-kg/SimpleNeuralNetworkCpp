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

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>

#include "SimpleNeuralNetwork.h"

struct Genom {
    std::vector<float> genom;
    float rating;
};

struct TrainingItem {
    float in0;
    float in1;
    float out0;
};


float calculate_rating(
    SimpleNeuralNetwork &net,
    const std::vector<TrainingItem> &vTrainingData,
    const std::vector<float> &vGenom
) {
    float nSumDiffs = 0.0f;
    for (int i = 0; i < vTrainingData.size(); i++) {
        TrainingItem item = vTrainingData[i];
        float ret = net.calc({item.in0,item.in1})[0];
        if (ret < item.out0) {
            ret = item.out0 - ret;
        } else {
            ret = ret - item.out0;
        }
        if (ret < 0) {
            ret *= -1;
        }
        nSumDiffs += ret;
    }
    return nSumDiffs / vTrainingData.size();
}

void sort_genoms(std::vector<Genom> &vGenoms) {
    struct {
        bool operator()(Genom a, Genom b) const { return a.rating < b.rating; }
    } customLess;
    std::sort(vGenoms.begin(), vGenoms.end(), customLess);
}

int main(int argc, char *argv[]) {
	std::srand(std::time(nullptr));
    int nTrainingData = 1000;
    int nGenoms = 100;
    int nBetterGenoms = 30;
    int nMutateGenoms = 35;
    int nMixGenoms = 35;
    int nMaxGenerations = 100;

	SimpleNeuralNetwork net({2,64,64,1});

    // genetic algoritm

    // init training data
    std::vector<TrainingItem> vTrainingData;
    for (int i = 0; i < nTrainingData; i++) {
        TrainingItem item;
        item.in0 = std::rand() % 100;
        item.in1 = std::rand() % 100;
        item.out0 = item.in0 + item.in1;
        vTrainingData.push_back(item);
    }

    // init first generation
    std::vector<Genom> vGenoms;
    for (int i = 0; i < nGenoms; i++) {
        net.mutateGenom();
        Genom gen;
        gen.genom = net.getGenom();
        gen.rating = 100000.0f;
        vGenoms.push_back(gen);
    }

    // calc rating
    for (int nG = 0; nG < vGenoms.size(); nG++) {
        net.setGenom(vGenoms[nG].genom);
        vGenoms[nG].rating = calculate_rating(net, vTrainingData, vGenoms[nG].genom);
        // std::cout << "vGenoms[" << nG << "].rating = " << vGenoms[nG].rating << std::endl;
    }
    
    int n = 0;
    while(vGenoms[0].rating > 1.0f && n < nMaxGenerations) {
        n += 1;
        // better generations will be on the top
        sort_genoms(vGenoms);
        std::cout << " ------- Gen " << n << " ------- " << std::endl;
        for (int nG = 0; nG < 3; nG++) {
            std::cout << "vGenoms[" << nG << "].rating = " << vGenoms[nG].rating << std::endl;
        }

        int nOffset = nBetterGenoms;
        // mutate
        for (int i = 0; i < nMutateGenoms; i++) {
            int n0 = std::rand() % nBetterGenoms;
            net.setGenom(vGenoms[n0].genom);
            net.mutateGenom();
            vGenoms[nOffset + i].genom = net.getGenom();
        }

        nOffset += nMutateGenoms;
        for (int i = 0; i < nMixGenoms; i++) {
            int n0 = std::rand() % nBetterGenoms;
            int n1 = std::rand() % nBetterGenoms;
            net.setGenom(vGenoms[n0].genom);
            net.mixGenom(vGenoms[n1].genom);
            vGenoms[nOffset + i].genom = net.getGenom();
        }

        // calc rating
        for (int nG = nBetterGenoms; nG < vGenoms.size(); nG++) {
            net.setGenom(vGenoms[nG].genom);
            vGenoms[nG].rating = calculate_rating(net, vTrainingData, vGenoms[nG].genom);
            // std::cout << "vGenoms[" << nG << "].rating = " << vGenoms[nG].rating << std::endl;
        }
    }

    net.setGenom(vGenoms[0].genom);
    for (int i = 0; i < 10; i++) {
        float x = std::rand() % 100;
        float y = std::rand() % 100;
        std::cout << x << " + " << y << " = " << net.calc({x,y})[0] << ", expected (" << int(x+y) << ") " << std::endl;
    }
	return 0;
}

/*
// first run
42 + 22 = 64.0628, expected (64) 
18 + 37 = 57.3337, expected (55) 
62 + 70 = 135.232, expected (132) 
70 + 25 = 94.1286, expected (95) 
80 + 98 = 182.813, expected (178) 
51 + 69 = 123.614, expected (120) 
53 + 68 = 124.446, expected (121) 
79 + 21 = 98.413, expected (100) 
65 + 92 = 161.946, expected (157) 
14 + 46 = 63.256, expected (60)
*/