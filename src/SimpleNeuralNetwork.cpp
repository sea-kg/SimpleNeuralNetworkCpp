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

#include "SimpleNeuralNetwork.h"

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <chrono>

SimpleNeuralNetwork::SimpleNeuralNetwork(std::vector<int> vLayers) 
    : m_vLayers{std::move(vLayers)}
{
    m_nCalcSumMs= 0;
    m_nCalcCounter = 0;
    m_nLayersSize = m_vLayers.size();
    m_nInputSize = m_vLayers[0];
    for (int i = 0; i < m_nInputSize; i++) {
        m_vWeights.push_back(1.0f);
    }

    for (int nL = 1; nL < m_nLayersSize; nL++) {
        int nPrevLayerSize = m_vLayers[nL - 1];
        int nLayerSize = m_vLayers[nL];
        for (int nN = 0; nN < nPrevLayerSize; nN++) {
            m_vBufferSignals.push_back(0.0f);
            for(int nP = 0; nP < nLayerSize; nP++) {
                float nWeight = this->randomWeight();
                // float nWeight = 1.0f;
                m_vWeights.push_back(nWeight);
            }
        }
    }

    m_nOutputSize = m_vLayers[m_nLayersSize-1];
    for (int i = 0; i < m_nOutputSize; i++) {
        m_vBufferOutput.push_back(0.0f);
        m_vBufferSignals.push_back(0.0f);
    }

    // std::cout
    //     << "m_vWeights.size() = " << m_vWeights.size() << std::endl
    //     << "m_vBufferSignals.size() = " << m_vBufferSignals.size() << std::endl
    // ;
}

const std::vector<float> &SimpleNeuralNetwork::calc(const std::vector<float> &vInput) {
    auto start = std::chrono::steady_clock::now();
    int nOffset = 0;
    float nSum = 0;
    if (m_nInputSize != vInput.size()) {
        throw std::runtime_error("Incorrect input size!");
    }

    int i = 0;
    while(i < m_nInputSize) {
        m_vBufferSignals[i] = vInput[i] * m_vWeights[i];
        ++i;
    }

    int nWeightOffset = m_nInputSize;
    int nSignalOffset = 0;
    int nCounter = 0;
    // std::cout << " ----- " << std::endl;
    int nP;
    int nN;
    int nBufferSignalN;
    int nPrevLayerSize;
    int nLayerSize;
    int nBufferNumberOffset;

    int nL = 1;
    while (nL < m_nLayersSize) {
        nPrevLayerSize = m_vLayers[nL - 1];
        nLayerSize = m_vLayers[nL];
        nBufferNumberOffset = nSignalOffset + nPrevLayerSize;
        nN = 0;
        while (nN < nLayerSize) {
            nSum = 0.0;
            nP = 0;
            while (nP < nPrevLayerSize) {
                nBufferSignalN = nSignalOffset + nP;
                nSum += m_vBufferSignals[nBufferSignalN] * m_vWeights[nWeightOffset];
                ++nWeightOffset;
                ++nP;
            }
            m_vBufferSignals[nBufferNumberOffset + nN] = nSum;
            ++nN;
        }
        nSignalOffset += nPrevLayerSize;
        ++nL;
    }

    
    // this shit very longer then 'for' + '=' +/- (100-300ns)
    // m_vBufferOutput = std::move(std::vector<float>(m_vBufferSignals.end() - m_nOutputSize, m_vBufferSignals.end()));

    // this fate
    int nOutputSize = m_nOutputSize - 1;
    int nSignalsLast = m_vBufferSignals.size() - 1;
    for (int i = 0; i <= nOutputSize; ++i) {
        int nOutputNumber = nSignalsLast - nOutputSize + i;
        // std::cout << "m_vBufferOutput[" << i << "] = m_vBufferSignals[" << nOutputNumber << "] " << m_vBufferSignals[nOutputNumber] << std::endl;
        m_vBufferOutput[i] = m_vBufferSignals[nOutputNumber];
    }

    auto end = std::chrono::steady_clock::now();
    m_nCalcSumMs = m_nCalcSumMs + std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    ++m_nCalcCounter;
    // 2022-09-08 00:07 - 22732ns
    return m_vBufferOutput;
}

long long SimpleNeuralNetwork::getCalcAvarageTimeInNanoseconds() {
    // std::cout
    //     << "m_nCalcSumMs = " << m_nCalcSumMs << std::endl
    //     << "m_nCalcCounter = " << m_nCalcCounter << std::endl
    // ;
    return m_nCalcSumMs / m_nCalcCounter;
}

const std::vector<float> &SimpleNeuralNetwork::getGenom() {
    return m_vWeights;
}

void SimpleNeuralNetwork::setGenom(const std::vector<float> &vWeights) {
    m_vWeights = vWeights;
}

void SimpleNeuralNetwork::mutateGenom() {
    int nCountOfMutations = std::rand() % m_vWeights.size();
    for (int i = 0; i < nCountOfMutations; i++) {
        int n = std::rand() % m_vWeights.size();
        m_vWeights[n] = this->randomWeight();
    }
}

void SimpleNeuralNetwork::mixGenom(const std::vector<float> &vWeights) {
    for (int i = 0; i < m_vWeights.size(); i++) {
        if (std::rand() % 2 == 0) {
            m_vWeights[i] = vWeights[i];
        }
    }
}

float SimpleNeuralNetwork::randomWeight() {
    return float((std::rand() % 200) - 100) / 100.0f;
}