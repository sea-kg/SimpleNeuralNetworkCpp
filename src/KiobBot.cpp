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

#include "KiobBot.h"

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <math.h>


// ---------------------------------------------------------------------
// KiobBotParticle

KiobBotParticle::KiobBotParticle() : KiobBotParticle(0.0f, 0.0f, 1.0f) {
    
}

KiobBotParticle::KiobBotParticle(float nX, float nY, float nSignalGain) {
    m_nX = nX;
    m_nY = nY;
    m_nSignal = 0.0f;
    m_nSignalGain = nSignalGain;
}

float KiobBotParticle::getX() {
    return m_nX;
}

float KiobBotParticle::getY() {
    return m_nY;
}

void KiobBotParticle::setSignal(float nSignal) {
    m_nSignal = nSignal;
}

void KiobBotParticle::appendEchoSignal(float nEchoSignal) {
    m_nSignal += nEchoSignal;
}

float KiobBotParticle::getSignal() {
    return m_nSignal * m_nSignalGain;
}

float KiobBotParticle::getSignalGain() {
    return m_nSignalGain;
}

float KiobBotParticle::distance(KiobBotParticle &part) {
    float dX = part.getX() - m_nX;
    float dY = part.getY() - m_nY;
    return std::sqrt(dX*dX + dY*dY);
}

float KiobBotParticle::calcK(KiobBotParticle &part) {
    float nX0 = part.getX();
    float nY0 = part.getY();
    float nX1 = m_nX;
    float nY1 = m_nY;
    float nK = nX0 * nX1 *  + nY0 * nY1;

    float nDistance = this->distance(part);
    return nK; /// * (1.0f / (nDistance + 1.0f));
}

// float KiobBotParticle::calcSignal(KiobBotParticle &part) {
// 
// }

// ---------------------------------------------------------------------
// KiobBotNetwork

KiobBotNetwork::KiobBotNetwork(float nSize, int nBeginCount) {
    m_nSize = nSize;
    for (int i = 0; i < nBeginCount; i++) {
        float nX = m_nSize * (float(std::rand() % 1000)/1000);
        float nY = m_nSize * (float(std::rand() % 1000)/1000);
        float nSignalGain = static_cast<float>(std::rand() % 4000 - 2000) / 1000.0f;
        m_vParticles.push_back(KiobBotParticle(nX, nY, nSignalGain));
    }

    m_in = KiobBotParticle(0.0f, 0.0f, 1.0f);
    m_out = KiobBotParticle(nSize, nSize, 1.0f);
}

float KiobBotNetwork::calc(std::vector<float> vInput) {
    float nXCenter = 0.0f;
    float nYCenter = 0.0f;
    int nParticlesSize = m_vParticles.size();

    std::vector<int> vSendedPoints;
    for (int i = 0; i < vInput.size(); i++) {
        m_vParticles[i].setSignal(vInput[i]);
        vSendedPoints.push_back(i);
    }

    // Sending a Primary Signal
    int nCicles = 2;
    for (int c = 0; c < nCicles; c++) {
        
        std::vector<int> vSendedPoints2;
        for (int i = 0; i < vSendedPoints.size(); i++) {
            int n = vSendedPoints[i];
            std::vector<int> vPoints2 = KiobBotNetwork::sendSignal(m_vParticles[n]);
            for (int t = 0; t < vPoints2.size(); t++) {
                vSendedPoints2.push_back(vPoints2[t]);
            }
        }
        vSendedPoints.clear();
        for (int t = 0; t < vSendedPoints2.size(); t++) {
            vSendedPoints.push_back(vSendedPoints2[t]);
        }
    }

    // for (int i = 0; i < nParticlesSize; i++) {
    //     float nDistance = center.distance(m_vParticles[i]);
    //     if (nDistance < ds) {
    //         float nK = (1.0f / (nDistance + 1.0f));
    //         m_vParticles[i].setSignal(nK * center.getSignal());
    //         vEchoSignals.push_back(i);
    //     }
    // }

    // for (int n = 0; n < 1; n++) {
    //     for (int x = 0; x < nParticlesSize; x++) {
    //         for (int y = 0; y < nParticlesSize; y++) {
    //             float nK = m_vParticles[x].calcK(m_vParticles[y]);
    //             m_vEchoSignals[y] += nK * m_vParticles[x].getSignal();
    //         }
    //     }
    //     for (int i = 0; i < nParticlesSize; i++) {
    //         m_vParticles[i].appendEchoSignal(m_vEchoSignals[i]);
    //         m_vEchoSignals[i] = 0.0f;
    //     }
    // }

    // Sending a Primary Signal
    // float nOutput = 0.0f;
    // for (int i = 0; i < nParticlesSize; i++) {
    //     float nK = center.calcK(m_vParticles[i]);
    //     nOutput += nK * m_vParticles[i].getSignal();
    // }
    return m_vParticles[0].getSignal();
}

std::vector<int> KiobBotNetwork::sendSignal(KiobBotParticle point) {
    std::vector<int> vSended;
    for (int i = 0; i < m_vParticles.size(); i++) {
        float nDistance = point.distance(m_vParticles[i]);
        if (nDistance < m_nSize / 3) {
            float nK = (1.0f / (nDistance + 1.0f));
            m_vParticles[i].setSignal(nK * point.getSignal());
            vSended.push_back(i);
        }
        if (vSended.size() > 5) {
            break;
        }
    }
    return vSended;
}


void KiobBotNetwork::print() {
    for (int i = 0; i < m_vParticles.size(); i++) {
        std::cout
            << i << " ("
            << m_vParticles[i].getX() << ","
            << m_vParticles[i].getY()
            << ") "
            << std::endl
        ;
    }
}

/*
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
    // 2022-09-08 01:52 - 2780ns (?)
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
    int nTypeOfMutation = std::rand() % 2;
    if (nTypeOfMutation == 0) {
        // light mutation
        int nCountOfMutations = std::rand() % m_vWeights.size();
        while (nCountOfMutations < 5) {
            // no mutation - it's not a option
            nCountOfMutations = std::rand() % m_vWeights.size();
        }
        for (int i = 0; i < nCountOfMutations; i++) {
            int n = std::rand() % m_vWeights.size();
            float nSign = float((std::rand() % 2) - 1);
            // change only of 1% of genom
            m_vWeights[n] = m_vWeights[n] + nSign * m_vWeights[n] * 0.01f;
        }
    } else {
        // hard mutation
        int nCountOfMutations = std::rand() % m_vWeights.size();
        while (nCountOfMutations < 5) {
            // no mutation - it's not a option
            nCountOfMutations = std::rand() % m_vWeights.size();
        }
        for (int i = 0; i < nCountOfMutations; i++) {
            int n = std::rand() % m_vWeights.size();
            m_vWeights[n] = this->randomWeight();
        }
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

// ---------------------------------------------------------------------
// SimpleNeuralTrainingItem

SimpleNeuralTrainingItem::SimpleNeuralTrainingItem(const std::vector<float> vIn, const std::vector<float> vOut) 
    : m_vIn{std::move(vIn)}
    , m_vOut{std::move(vOut)}

{
    // nothing
}

const std::vector<float> &SimpleNeuralTrainingItem::getIn() const {
    return m_vIn;
}

const std::vector<float> &SimpleNeuralTrainingItem::getOut() const {
    return m_vOut;
}

// ---------------------------------------------------------------------
// SimpleNeuralTrainingItemList

SimpleNeuralTrainingItemList::SimpleNeuralTrainingItemList(int nNumberOfIn, int nNumberOfOut) 
    : m_nNumberOfIn(nNumberOfIn)
    , m_nNumberOfOut(nNumberOfOut)
{

}

int SimpleNeuralTrainingItemList::getNumberOfIn() const {
    return m_nNumberOfIn;
}

int SimpleNeuralTrainingItemList::getNumberOfOut() const {
    return m_nNumberOfOut;
}

void SimpleNeuralTrainingItemList::addItem(std::vector<float> vIn, std::vector<float> vOut) {
    if (vIn.size() != m_nNumberOfIn) {
        std::cerr << "[SimpleNeuralTrainingItemList::addItem] Wrong number of in elements" << std::endl;
        return;
    }
    if (vOut.size() != m_nNumberOfOut) {
        std::cerr << "[SimpleNeuralTrainingItemList::addItem] Wrong number of out elements" << std::endl;
        return;
    }
    m_vData.emplace_back(vIn, vOut);
}

unsigned int SimpleNeuralTrainingItemList::size() const {
    return m_vData.size();
}

std::vector<SimpleNeuralTrainingItem>::iterator SimpleNeuralTrainingItemList::begin() {
    return m_vData.begin();
}

std::vector<SimpleNeuralTrainingItem>::iterator SimpleNeuralTrainingItemList::end() {
    return m_vData.end();
}


// ---------------------------------------------------------------------
// SimpleNeuralGenom


SimpleNeuralGenom::SimpleNeuralGenom(std::vector<float> vGenom, float nRating)
    : m_vGenom{std::move(vGenom)}
    , m_nRating{std::move(nRating)}
{

}

const std::vector<float> &SimpleNeuralGenom::getGenom() const {
    return m_vGenom;
}

void SimpleNeuralGenom::setGenom(const std::vector<float> &vGenom) {
    m_vGenom = vGenom;
}

float SimpleNeuralGenom::getRating() const {
    return m_nRating;
}


void SimpleNeuralGenom::addRating(float nDiff) {
    m_nRating += nDiff;
}

void SimpleNeuralGenom::setRating(float nRating) {
    m_nRating = nRating;
}

void SimpleNeuralGenom::calculateRating(SimpleNeuralNetwork *pNet, SimpleNeuralTrainingItemList *pTrainingData) {
    float nSumDiffs = 0.0f;
    pNet->setGenom(m_vGenom);
    std::vector<SimpleNeuralTrainingItem>::iterator it;
    for (it = pTrainingData->begin(); it != pTrainingData->end(); ++it) {
        std::vector<float> vOutNet = pNet->calc(it->getIn());
        std::vector<float> vOutExpected = it->getOut();
        float ret = 0;
        for (int i = 0; i < vOutNet.size(); i++) {
            float x1 = vOutNet[i];
            float x2 = vOutExpected[i];
            ret += (x2 - x1)*(x2 - x1);
        }
        ret = std::sqrt(ret);
        nSumDiffs += ret;
    }
    m_nRating = nSumDiffs / float(pTrainingData->size());
}

// ---------------------------------------------------------------------
// SimpleNeuralGenomList

SimpleNeuralGenomList::SimpleNeuralGenomList(int nBetterGenoms, int nMutateGenoms, int nMixGenoms) 
    : m_nBetterGenoms(nBetterGenoms)
    , m_nMutateGenoms(nMutateGenoms)
    , m_nMixGenoms(nMixGenoms)
    , m_nAllGenoms(m_nBetterGenoms + m_nMutateGenoms + m_nMixGenoms)
{
    m_vGenoms.reserve(m_nAllGenoms);
}

void SimpleNeuralGenomList::fillRandom(SimpleNeuralNetwork *pNet) {
    for (int i = 0; i < m_nAllGenoms; ++i) {
        pNet->mutateGenom();
        m_vGenoms.emplace_back(pNet->getGenom(), 100000.0f);
    }
}

const std::vector<SimpleNeuralGenom> &SimpleNeuralGenomList::list() const {
    return m_vGenoms;
}

float SimpleNeuralGenomList::getBetterRating() {
    return m_vGenoms[0].getRating();
}

void SimpleNeuralGenomList::sort() {
    std::sort(
        m_vGenoms.begin(), 
        m_vGenoms.end(), 
        [](const SimpleNeuralGenom &a, const SimpleNeuralGenom &b) { 
            return a.getRating() < b.getRating(); 
        });
}

void SimpleNeuralGenomList::printFirstRatings(int nNumber) {
    for (int nG = 0; nG < nNumber; nG++) {
        std::cout << "genom[" << nG << "].rating = " << m_vGenoms[nG].getRating() << std::endl;
    }
}

void SimpleNeuralGenomList::mutateAndMix(SimpleNeuralNetwork *pNet) {
    // assert(nBetterGenoms + nMutateGenoms <= nGenoms);
    int nOffset = m_nBetterGenoms;
    // mutate
    for (int i = 0; i < m_nMutateGenoms; i++) {
        int n0 = std::rand() % m_nBetterGenoms;
        pNet->setGenom(m_vGenoms[n0].getGenom());
        pNet->mutateGenom();
        m_vGenoms[nOffset + i].setGenom(pNet->getGenom());
    }

    // assert(nBetterGenoms + 2 * nMutateGenoms <= nGenoms);
    nOffset += m_nMutateGenoms;
    for (int i = 0; i < m_nMixGenoms; i++) {
        int n0 = std::rand() % m_nBetterGenoms;
        int n1 = std::rand() % m_nBetterGenoms;
        pNet->setGenom(m_vGenoms[n0].getGenom());
        pNet->mixGenom(m_vGenoms[n1].getGenom());
        m_vGenoms[nOffset + i].setGenom(pNet->getGenom());
    }
}

const SimpleNeuralGenom &SimpleNeuralGenomList::getBetterGenom() {
    this->sort();
    return m_vGenoms[0];
}

void SimpleNeuralGenomList::calculateRatingForAll(
    SimpleNeuralNetwork *pNet,
    SimpleNeuralTrainingItemList *pTrainingData
) {
    for (auto it = m_vGenoms.begin(); it != m_vGenoms.end(); ++it) {
        it->calculateRating(pNet, pTrainingData);
    }
}

void SimpleNeuralGenomList::calculateRatingForMutatedAndMixed(
    SimpleNeuralNetwork *pNet,
    SimpleNeuralTrainingItemList *pTrainingData
) {
    for (auto it = m_vGenoms.begin() + m_nBetterGenoms; it != m_vGenoms.end(); ++it) {
        it->calculateRating(pNet, pTrainingData);
    }
}*/