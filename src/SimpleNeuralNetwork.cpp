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
    if (m_nInputSize != vInput.size()) {
        throw std::runtime_error("Incorrect input size!");
    }
    for (int i = 0; i < m_nInputSize; ++i) {
        m_vBufferSignals[i] = vInput[i] * m_vWeights[i];
        // std::cout << "m_vBufferSignals[" << i << "] = " << m_vBufferSignals[i] << std::endl;
    }
    int nWeightOffset = m_nInputSize;
    int nSignalOffset = 0;
    // std::cout << " ----- " << std::endl;
    for (int nL = 1; nL < m_nLayersSize; ++nL) {
        // std::cout << "nL = " << nL << std::endl;
        int nPrevLayerSize = m_vLayers[nL - 1];
        int nLayerSize = m_vLayers[nL];
        for (int nN = 0; nN < nLayerSize; nN++) {
            float nSum = 0.0;
            for(int nP = 0; nP < nPrevLayerSize; ++nP) {
                int nBufferSignalN = nSignalOffset + nP;
                // std::cout
                //     << " += {m_vBufferSignals[" << nBufferSignalN << "] " << m_vBufferSignals[nBufferSignalN] << "} " 
                //     << " * {m_vWeights[" << nWeightOffset << "] = " << m_vWeights[nWeightOffset] << "}"
                //     << std::endl;
                
                nSum += m_vBufferSignals[nBufferSignalN] * m_vWeights[nWeightOffset];
                ++nWeightOffset;
            }
            int nBufferNumber = nSignalOffset + nPrevLayerSize + nN;
            m_vBufferSignals[nBufferNumber] = nSum;
            // std::cout << "m_vBufferSignals[" << nBufferNumber << "] = " << nSum << std::endl;
        }
        nSignalOffset += nPrevLayerSize;
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