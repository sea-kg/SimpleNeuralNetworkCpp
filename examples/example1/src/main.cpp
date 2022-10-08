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

struct TrainingItem {
    explicit TrainingItem(float in0, float in1, float out0)
        : in0{std::move(in0)}
        , in1{std::move(in1)}
        , out0{std::move(out0)}
        {};
    const float in0;
    const float in1;
    const float out0;
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

int main(int argc, char *argv[]) {

	std::srand(std::time(nullptr));
    constexpr size_t nTrainingData = 1000;

    constexpr int nMaxGenerations = 100;

	SimpleNeuralNetwork net({2,64,64,1});

    // genetic algoritm

    // init training data
    const auto vTrainingData = [](const size_t size){
        std::vector<TrainingItem> result;
        result.reserve(size);

        static const auto get_random_int = []{
            return static_cast<float>(std::rand() % 100);
        };
        
        for (int i = 0; i < size; ++i) {
            float in0 = get_random_int();
            float in1 = get_random_int();
            float out = in0 + in1;
            result.emplace_back(in0, in1, out);
        }

        return result;
    }(nTrainingData);

    // init first generation
    SimpleNeuralGenomList genoms(30,35,35);
    genoms.fillRandom(&net);

    const auto calc_rating = [&]() {
        for (auto & g : genoms.list()) {
            net.setGenom(g.getGenom());
            g.setRating(calculate_rating(net, vTrainingData, g.getGenom()));
            // static int count = 0;
            // std::cout << "vGenoms[" << count++ << "].rating = " << g.rating << std::endl;
        }
    };
    calc_rating();
    
    int n = 0;
    while (genoms.getBetterRating() > 0.1f && n < nMaxGenerations) {
        ++n;

        auto start = std::chrono::steady_clock::now();
        // better generations will be on the top
        genoms.sort();

        

        std::cout << " ------- Gen " << n << " ------- " << std::endl;
        genoms.printFirstRatings(3);

        genoms.mutateAndMix(&net);

        // no need all calculate, enougth from nBetterGenoms in genoms, like code down
        // calc_rating();

        // calc rating
        for (auto it = genoms.beginRecalc(); it != genoms.end(); ++it) {
            net.setGenom(it->getGenom());
            it->setRating(calculate_rating(net, vTrainingData, it->getGenom()));
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