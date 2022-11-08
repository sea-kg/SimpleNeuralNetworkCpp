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

#include "KiobBot.h"

/*
void initTrainingData(SimpleNeuralTrainingItemList &trainingData, int nTrainingData) {
    // init training data
    for (int i = 0; i < nTrainingData; ++i) {
        float in0 = static_cast<float>(std::rand() % 100);
        float in1 = static_cast<float>(std::rand() % 100);
        trainingData.addItem({in0, in1}, {in0 + in1});
    }
}
*/

int main(int argc, char *argv[]) {
	std::srand(std::time(nullptr));

    KiobBotNetwork net(10.0, 10);

    for (float nInput = -1.0f; nInput <= 1.0f; nInput += 0.1f) {
        float nOutput = net.calc({nInput});
        std::cout << "In: " << nInput << ", out: " << nOutput << std::endl;
    }

    // net.print();

    /*constexpr int nNumberOfIn = 2;
    constexpr int nNumberOfOut = 1;

    SimpleNeuralTrainingItemList trainingData(nNumberOfIn,nNumberOfOut);
    
    constexpr size_t nTrainingDataSize = 1000;
    initTrainingData(trainingData, nTrainingDataSize);

	SimpleNeuralNetwork *pNet = new SimpleNeuralNetwork({
        trainingData.getNumberOfIn(),
        64,64, // middle layers
        trainingData.getNumberOfOut()
    });

    // init first generation
    constexpr int nBetterSpecimens = 30;
    constexpr int nMutateSpecimens = 40;
    constexpr int nMixSpecimens = 40;
    SimpleNeuralGenomList genoms(nBetterSpecimens, nMutateSpecimens, nMixSpecimens);
    genoms.fillRandom(pNet); // TODO fill can be randomly, no need net
    genoms.calculateRatingForAll(pNet, &trainingData);

    constexpr int nMaxGenerations = 100;    
    constexpr float nConditionRatingStop = 0.1f;
    int n = 0;
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
        // 73007f060c20ea28d5d82db7ef2b0436d132d5b8: ~1608ms-1615ms
        // dee290c24e67b8bd692ebf91ffe851349d17b5d8: ~2305ms
    }

    pNet->setGenom(genoms.getBetterGenom().getGenom());
    for (int i = 0; i < 10; i++) {
        float x = (std::rand() % 200) - 100;
        float y = (std::rand() % 200) - 100;
        std::cout << x << " + " << y << " = " << pNet->calc({x,y})[0] << ", expected (" << int(x+y) << ") " << std::endl;
    }

    std::cout << "calc avarage time: " << pNet->getCalcAvarageTimeInNanoseconds() << "ns" << std::endl;
    // 73007f060c20ea28d5d82db7ef2b0436d132d5b8: ~22ms
    // dee290c24e67b8bd692ebf91ffe851349d17b5d8: ~22ms*/
	return 0;
}
