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

#include "SimpleNeuralNetwork.h"

float calculate_rating(
    SimpleNeuralNetwork &net,
    SimpleNeuralTrainingItemList &trainingData,
    const std::vector<float> &vGenom
) {
    float nSumDiffs = 0.0f;
    std::vector<SimpleNeuralTrainingItem>::iterator it;
    for (it = trainingData.begin(); it != trainingData.end(); ++it) {
        float ret = net.calc(it->getIn())[0];
        float nOut = it->getOut()[0];
        if (ret < nOut) {
            ret = nOut - ret;
        } else {
            ret = ret - nOut;
        }
        if (ret < 0) {
            ret *= -1;
        }
        nSumDiffs += ret;
    }
    return nSumDiffs / float(trainingData.size());
}

void initTrainingData(SimpleNeuralTrainingItemList &trainingData, int nTrainingData) {
    // init training data
    for (int i = 0; i < nTrainingData; ++i) {
        float in0 = static_cast<float>(std::rand() % 100);
        float in1 = static_cast<float>(std::rand() % 100);
        float out = in0 + in1;
        trainingData.addItem({in0, in1}, {out});
    }
}

int main(int argc, char *argv[]) {

	std::srand(std::time(nullptr));
    

    SimpleNeuralTrainingItemList trainingData(2,1);
    
    constexpr size_t nTrainingDataSize = 1000;
    initTrainingData(trainingData, nTrainingDataSize);

	SimpleNeuralNetwork net({
        trainingData.getNumberOfIn(),
        64,64, // middle layers
        trainingData.getNumberOfOut()
    });

    // init first generation
    SimpleNeuralGenomList genoms(30,35,35);
    genoms.fillRandom(&net);

    const auto calc_rating = [&]() {
        for (auto & g : genoms.list()) {
            net.setGenom(g.getGenom());
            g.setRating(calculate_rating(net, trainingData, g.getGenom()));
            // static int count = 0;
            // std::cout << "vGenoms[" << count++ << "].rating = " << g.rating << std::endl;
        }
    };
    calc_rating();

    constexpr int nMaxGenerations = 100;    
    constexpr float nConditionRatingStop = 0.1f;
    int n = 0;
    while (genoms.getBetterRating() > nConditionRatingStop && n < nMaxGenerations) {
        ++n;

        auto start = std::chrono::steady_clock::now();
        genoms.sort(); // better generations will be on the top

        std::cout << " ------- Generation " << n << " ------- " << std::endl;
        genoms.printFirstRatings(3);
        genoms.mutateAndMix(&net);

        // no need all calculate, enougth from nBetterGenoms in genoms, like code down
        // calc_rating();

        // calc rating
        for (auto it = genoms.beginRecalc(); it != genoms.end(); ++it) {
            net.setGenom(it->getGenom());
            it->setRating(calculate_rating(net, trainingData, it->getGenom()));
        }

        auto end = std::chrono::steady_clock::now();
        std::cout
            << "Elapsed time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms" << std::endl
        ;
        std::cout << "calc avarage time: " << net.getCalcAvarageTimeInNanoseconds() << "ns" << std::endl;
        // 73007f060c20ea28d5d82db7ef2b0436d132d5b8: ~1608ms-1615ms
        // dee290c24e67b8bd692ebf91ffe851349d17b5d8: ~2305ms
    }

    net.setGenom(genoms.getBetterGenom().getGenom());
    for (int i = 0; i < 10; i++) {
        float x = (std::rand() % 200) - 100;
        float y = (std::rand() % 200) - 100;
        std::cout << x << " + " << y << " = " << net.calc({x,y})[0] << ", expected (" << int(x+y) << ") " << std::endl;
    }

    std::cout << "calc avarage time: " << net.getCalcAvarageTimeInNanoseconds() << "ns" << std::endl;
    // 73007f060c20ea28d5d82db7ef2b0436d132d5b8: ~22ms
    // dee290c24e67b8bd692ebf91ffe851349d17b5d8: ~22ms
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