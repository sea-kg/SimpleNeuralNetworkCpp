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

#include "car_simulator.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <thread>

long getCurrentTimeInMilliseconds() {
    long nTimeStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return nTimeStart;
}


// ---------------------------------------------------------------------
// YPos

YPos::YPos() {
    m_nX = 0;
    m_nY = 0;
}

YPos::YPos(int x, int y) {
    m_nX = x;
    m_nY = y;
}

int YPos::x() const {
    return m_nX;
}

int YPos::y() const {
    return m_nY;
}

void YPos::update(int x, int y) {
    m_nX = x;
    m_nY = y;
}

void YPos::update(const YPos& coord) {
    m_nX = coord.x();
    m_nY = coord.y();
}

void YPos::setX(int x) {
    m_nX = x;
}

void YPos::setY(int y) {
    m_nY = y;
}

YPos& YPos::operator+=(const YPos& other) {
    m_nX = m_nX + other.x();
    m_nY = m_nY + other.y();
    return *this;
}

YPos& YPos::operator-=(const YPos& other) {
    m_nX = m_nX - other.x();
    m_nY = m_nY - other.y();
    return *this;
}

YPos YPos::operator+(const YPos& other) const {
    YPos c(m_nX, m_nY);
    c += other;
    return c;
}

YPos YPos::operator-(const YPos& other) const {
    YPos c(m_nX, m_nY);
    c -= other;
    return c;
}

bool YPos::operator>(const YPos& other) const {
    return m_nX > other.x() || m_nY > other.y();
}

bool YPos::operator<(const YPos& other) const {
    return m_nX < other.x() || m_nY < other.y();
}

bool YPos::isInsideRect(const YPos& topLeft, const YPos& bottomRight) const {
    return
        m_nX >= topLeft.x()
        && m_nX <= bottomRight.x()
        && m_nY >= topLeft.y()
        && m_nY <= bottomRight.y()
    ;
}

// ---------------------------------------------------------------------
// YLine

YLine::YLine() {

}

YLine::YLine(YPos p0, YPos p1) {
    m_p0 = p0;
    m_p1 = p1;
}

YPos YLine::p0() const {
    return m_p0;
}

YPos YLine::p1() const {
    return m_p1;
}

void YLine::update(YPos p0, YPos p1) {
    m_p0 = p0;
    m_p1 = p1;
}

void YLine::set0(YPos p0) {
    m_p0 = p0;
}

void YLine::set1(YPos p1) {
    m_p1 = p1;
}

float YLine::getLength() const {
    float x21 = m_p1.x() - m_p0.x();
    float y21 = m_p0.y() - m_p1.y();
    return std::sqrt(x21*x21 + y21*y21);    
}

YPos YLine::findIntersectStraightLines(const YLine &line) const {
    // https://www.cyberforum.ru/cpp-beginners/thread2015192.html
    float a1 = m_p0.y() - m_p1.y();
    float b1 = m_p1.x() - m_p0.x();
    float c1 = m_p0.x() * m_p1.y() - m_p1.x() * m_p0.y();
    float a2 = line.p0().y() - line.p1().y();
    float b2 = line.p1().x() - line.p0().x();
    float c2 = line.p0().x() * line.p1().y() - line.p1().x() * line.p0().y();
    if ((a1 / a2) == (b1 / b2)) {
        return YPos(0, 0); // TODO lines is parallel
    }
    float det = a1 * b2 - a2 * b1;
    float x = (b1 * c2 - b2 * c1) / det;
    float y = (a2 * c1 - a1 * c2) / det;
    return YPos(x, y);
}

// ---------------------------------------------------------------------
// YKeyboard

YKeyboard::YKeyboard() {
    pollState();
}

void YKeyboard::pollState() {
    m_pKeyboardStateArray = SDL_GetKeyboardState(NULL);
}

bool YKeyboard::isUp() {
    bool bArrowUp =
        m_pKeyboardStateArray[SDL_SCANCODE_UP]
        && !m_pKeyboardStateArray[SDL_SCANCODE_LEFT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_RIGHT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_DOWN];
    bool bWasdUp = 
        m_pKeyboardStateArray[SDL_SCANCODE_W]
        && !m_pKeyboardStateArray[SDL_SCANCODE_A]
        && !m_pKeyboardStateArray[SDL_SCANCODE_D]
        && !m_pKeyboardStateArray[SDL_SCANCODE_S];
    return bArrowUp || bWasdUp;
}

bool YKeyboard::isUpLeft() {
    bool bArrowUp =
        m_pKeyboardStateArray[SDL_SCANCODE_UP]
        && m_pKeyboardStateArray[SDL_SCANCODE_LEFT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_RIGHT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_DOWN];
    bool bWasdUp = 
        m_pKeyboardStateArray[SDL_SCANCODE_W]
        && m_pKeyboardStateArray[SDL_SCANCODE_A]
        && !m_pKeyboardStateArray[SDL_SCANCODE_D]
        && !m_pKeyboardStateArray[SDL_SCANCODE_S];
    return bArrowUp || bWasdUp;
}

bool YKeyboard::isUpRight() {
    bool bArrowUp =
        m_pKeyboardStateArray[SDL_SCANCODE_UP]
        && !m_pKeyboardStateArray[SDL_SCANCODE_LEFT]
        && m_pKeyboardStateArray[SDL_SCANCODE_RIGHT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_DOWN];
    bool bWasdUp = 
        m_pKeyboardStateArray[SDL_SCANCODE_W]
        && !m_pKeyboardStateArray[SDL_SCANCODE_A]
        && m_pKeyboardStateArray[SDL_SCANCODE_D]
        && !m_pKeyboardStateArray[SDL_SCANCODE_S];
    return bArrowUp || bWasdUp;
}

bool YKeyboard::isDown() {
    bool bArrowUp =
        !m_pKeyboardStateArray[SDL_SCANCODE_UP]
        && !m_pKeyboardStateArray[SDL_SCANCODE_LEFT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_RIGHT]
        && m_pKeyboardStateArray[SDL_SCANCODE_DOWN];
    bool bWasdUp = 
        !m_pKeyboardStateArray[SDL_SCANCODE_W]
        && !m_pKeyboardStateArray[SDL_SCANCODE_A]
        && !m_pKeyboardStateArray[SDL_SCANCODE_D]
        && m_pKeyboardStateArray[SDL_SCANCODE_S];
    return bArrowUp || bWasdUp;
}

bool YKeyboard::isDownLeft() {
    bool bArrowUp =
        !m_pKeyboardStateArray[SDL_SCANCODE_UP]
        && m_pKeyboardStateArray[SDL_SCANCODE_LEFT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_RIGHT]
        && m_pKeyboardStateArray[SDL_SCANCODE_DOWN];
    bool bWasdUp = 
        !m_pKeyboardStateArray[SDL_SCANCODE_W]
        && m_pKeyboardStateArray[SDL_SCANCODE_A]
        && !m_pKeyboardStateArray[SDL_SCANCODE_D]
        && m_pKeyboardStateArray[SDL_SCANCODE_S];
    return bArrowUp || bWasdUp;
}

bool YKeyboard::isDownRight() {
    bool bArrowUp =
        !m_pKeyboardStateArray[SDL_SCANCODE_UP]
        && !m_pKeyboardStateArray[SDL_SCANCODE_LEFT]
        && m_pKeyboardStateArray[SDL_SCANCODE_RIGHT]
        && m_pKeyboardStateArray[SDL_SCANCODE_DOWN];
    bool bWasdUp = 
        !m_pKeyboardStateArray[SDL_SCANCODE_W]
        && !m_pKeyboardStateArray[SDL_SCANCODE_A]
        && m_pKeyboardStateArray[SDL_SCANCODE_D]
        && m_pKeyboardStateArray[SDL_SCANCODE_S];
    return bArrowUp || bWasdUp;
}

bool YKeyboard::isLeft() {
    bool bArrowUp =
        !m_pKeyboardStateArray[SDL_SCANCODE_UP]
        && m_pKeyboardStateArray[SDL_SCANCODE_LEFT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_RIGHT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_DOWN];
    bool bWasdUp = 
        !m_pKeyboardStateArray[SDL_SCANCODE_W]
        && m_pKeyboardStateArray[SDL_SCANCODE_A]
        && !m_pKeyboardStateArray[SDL_SCANCODE_D]
        && !m_pKeyboardStateArray[SDL_SCANCODE_S];
    return bArrowUp || bWasdUp;
}

bool YKeyboard::isRight() {
    bool bArrowUp =
        !m_pKeyboardStateArray[SDL_SCANCODE_UP]
        && !m_pKeyboardStateArray[SDL_SCANCODE_LEFT]
        && m_pKeyboardStateArray[SDL_SCANCODE_RIGHT]
        && !m_pKeyboardStateArray[SDL_SCANCODE_DOWN];
    bool bWasdUp = 
        !m_pKeyboardStateArray[SDL_SCANCODE_W]
        && !m_pKeyboardStateArray[SDL_SCANCODE_A]
        && m_pKeyboardStateArray[SDL_SCANCODE_D]
        && !m_pKeyboardStateArray[SDL_SCANCODE_S];
    return bArrowUp || bWasdUp;
}

bool YKeyboard::isF1() {
    return m_pKeyboardStateArray[SDL_SCANCODE_F1];
}

bool YKeyboard::isF4() {
    return m_pKeyboardStateArray[SDL_SCANCODE_F4];
}

bool YKeyboard::isF12() {
    return m_pKeyboardStateArray[SDL_SCANCODE_F12];
}

bool YKeyboard::isSpace() {
    return m_pKeyboardStateArray[SDL_SCANCODE_SPACE];
}

bool YKeyboard::isEscape() {
    return m_pKeyboardStateArray[SDL_SCANCODE_ESCAPE];
}

bool YKeyboard::isK() {
    return m_pKeyboardStateArray[SDL_SCANCODE_K];
}

bool YKeyboard::isL() {
    return m_pKeyboardStateArray[SDL_SCANCODE_L];
}

bool YKeyboard::isG() {
    return m_pKeyboardStateArray[SDL_SCANCODE_G];
}

bool YKeyboard::isH() {
    return m_pKeyboardStateArray[SDL_SCANCODE_H];
}

// ---------------------------------------------------------------------
// CarSimulatorState

CarSimulatorState::CarSimulatorState(int windowWidth, int windowHeight) {

}


// ---------------------------------------------------------------------
// RenderColor

RenderColor::RenderColor(int nR, int nG, int nB, int nA) {
    m_nR = nR;
    m_nG = nG;
    m_nB = nB;
    m_nA = nA;
}

void RenderColor::changeRenderColor(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, m_nR, m_nG, m_nB, m_nA);
}

// ---------------------------------------------------------------------
// RenderObject

RenderObject::RenderObject(int nPositionZ) {
    m_nPositionZ = nPositionZ;
    m_bDestroyed = false;
}

RenderObject::~RenderObject() {
    // nothing
}

int RenderObject::getPositionZ() {
    return m_nPositionZ;
}

bool RenderObject::isDestroyed() {
    return m_bDestroyed;
}

void RenderObject::destroy() {
    m_bDestroyed = true;
}

void RenderObject::modify(const CarSimulatorState& state) {
    // nothing modify in base object
}

int RenderObject::randomNoise() {
    int n = 7;
    int nRandom = std::rand() % n;
    return nRandom - (n / 2);
}


// ---------------------------------------------------------------------
// RenderLine

RenderLine::RenderLine(const YPos &p1, const YPos &p2, const RenderColor &color, int nPositionZ) 
: RenderObject(nPositionZ), m_color(color) {
    m_coord1 = p1;
    m_coord2 = p2;
    m_startCoord1 = p1;
    m_startCoord2 = p2;
}

void RenderLine::modify(const CarSimulatorState& state) {
    // m_coord1 = m_startCoord1 - state.getCoordLeftTop();
    // m_coord2 = m_startCoord2 - state.getCoordLeftTop() ;
}

bool RenderLine::canDraw(const CarSimulatorState& state) {
    return true;
    // return
    //     m_coord1.isInsideRect(state.getWindowRect())
    //     || m_coord2.isInsideRect(state.getWindowRect())
    // ;
}

void RenderLine::draw(SDL_Renderer* renderer) {
    m_color.changeRenderColor(renderer);
    SDL_RenderDrawLine(renderer, m_coord1.x(), m_coord1.y(), m_coord2.x(), m_coord2.y());
}

const YPos &RenderLine::getAbsoluteCoord1() {
    return m_startCoord1;
}

const YPos &RenderLine::getAbsoluteCoord2() {
    return m_startCoord2;
}

const YPos &RenderLine::getCoord1() {
    return m_coord1;
}

const YPos &RenderLine::getCoord2() {
    return m_coord2;
}

void RenderLine::updateAbsoluteCoords(const YPos &p1, const YPos &p2) {
    m_startCoord1 = p1;
    m_startCoord2 = p2;
}


// ---------------------------------------------------------------------
// RenderCar

RenderCar::RenderCar(const YPos &p0, const RenderColor &color, int nPositionZ) 
: RenderObject(nPositionZ), m_color(color) {
    m_nLinearVelocity = 0.0f;
    // m_coord1 = p0;
    m_top = p0;
    m_lineLeft = YLine(
        YPos(p0.x() - 5, p0.y() + 15),
        YPos(p0.x(), p0.y())
    );
    m_lineRight = YLine(
        YPos(p0.x() + 5, p0.y() + 15),
        YPos(p0.x(), p0.y())
    );
    m_lineBottom = YLine(
        YPos(p0.x() - 5, p0.y() + 15),
        YPos(p0.x() + 5, p0.y() + 15)
    );
}

void RenderCar::modify(const CarSimulatorState& state) {
    // m_coord1 = m_startCoord1 - state.getCoordLeftTop();
    // m_coord2 = m_startCoord2 - state.getCoordLeftTop() ;

    if (m_nLinearVelocity != 0) {
        if (m_nLinearVelocity > 0) {
            int step = 1 * m_nLinearVelocity;
            m_top.setY(m_top.y() - step);
            m_lineLeft.set1(m_top);
        }

        m_nLinearVelocity = m_nLinearVelocity * 0.99;
        if (m_nLinearVelocity < 0.01) {
            m_nLinearVelocity = 0.0f;
        }
        std::cout << m_nLinearVelocity << std::endl;
    }

}

bool RenderCar::canDraw(const CarSimulatorState& state) {
    return true;
    // return
    //     m_coord1.isInsideRect(state.getWindowRect())
    //     || m_coord2.isInsideRect(state.getWindowRect())
    // ;
}

void RenderCar::setLinearVelocity(float nLinearVelocity) {
    m_nLinearVelocity = nLinearVelocity;
}

void RenderCar::draw(SDL_Renderer* renderer) {
    m_color.changeRenderColor(renderer);
    SDL_RenderDrawLine(renderer, m_lineLeft.p0().x(), m_lineLeft.p0().y(), m_lineLeft.p1().x(), m_lineLeft.p1().y());
    SDL_RenderDrawLine(renderer, m_lineRight.p0().x(), m_lineRight.p0().y(), m_lineRight.p1().x(), m_lineRight.p1().y());
    SDL_RenderDrawLine(renderer, m_lineBottom.p0().x(), m_lineBottom.p0().y(), m_lineBottom.p1().x(), m_lineBottom.p1().y());
}

// ---------------------------------------------------------------------
// RenderWindow

RenderWindow::RenderWindow(const std::string &sTitle, int w, int h) {
    m_pWindow = NULL;
    m_pWindow = SDL_CreateWindow(
        sTitle.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        w, h,
        SDL_WINDOW_SHOWN // | SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (m_pWindow == NULL) {
        std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;
    }

    m_pRenderer = NULL;
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
}

RenderWindow::~RenderWindow() {
    if (m_pRenderer) {
        SDL_DestroyRenderer(m_pRenderer);
    }
}

void RenderWindow::cleanUp() {
    SDL_DestroyWindow(m_pWindow);
}

void RenderWindow::clear() {
    // background color
    SDL_SetRenderDrawColor(m_pRenderer, 22, 11, 22, 195);
    SDL_RenderClear(m_pRenderer);
}

void RenderWindow::modifyObjects(const CarSimulatorState& state) {
    for (int i = 0; i < m_vObjects.size(); i++) {
        m_vObjects[i]->modify(state);
    }
}

void RenderWindow::drawObjects(const CarSimulatorState& state) {
    for (int i = 0; i < m_vObjects.size(); i++) {
        m_vObjects[i]->draw(m_pRenderer);
    }

    // finish
    SDL_RenderPresent(m_pRenderer);
}


void RenderWindow::addObject(RenderObject *pObject) {
    m_vObjects.push_back(pObject);
}

void RenderWindow::removeObject(RenderObject *pObject) {
    auto it = std::find(m_vObjects.begin(), m_vObjects.end(), pObject);
    if (it != m_vObjects.end()) {
        m_vObjects.erase(it);
    }
}

// ---------------------------------------------------------------------
// CarSimulator

CarSimulator::CarSimulator() {
    m_sWindowName = "SimpleNeuralNetworkCpp: Car Learning Simulator SDL2";
    m_nWindowWidth = 1280;
    m_nWindowHeight = 720;
    m_pRenderWindow = nullptr;
    m_pRenderCar = nullptr;
}

bool CarSimulator::initSDL2() {
    if (SDL_Init(SDL_INIT_VIDEO) > 0) {
        std::cerr << "HEY.. SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cerr << "IMG_init has failed. Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // init
    m_pRenderWindow = new RenderWindow(
        m_sWindowName,
        m_nWindowWidth,
        m_nWindowHeight
    );

    m_pCarSimulatorState = new CarSimulatorState(
        m_nWindowWidth,
        m_nWindowHeight
    );
    return true;
}

int CarSimulator::start() {
    YKeyboard *pKeyboard = new YKeyboard();
    // YPos coordCenter = getCoordCenter();

    generateTrack();

    

    startFpsCounting();
    while (getMainState() != MainState::GAME_EXIT) {
        long nStartTime = getCurrentTimeInMilliseconds();

        clearWindow();
        modifyObjects();
        drawObjects();

        SDL_Event event;
        pKeyboard->pollState();

        // Get our controls and events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                setMainState(MainState::GAME_EXIT);
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                this->handleKeyboardCommand(pKeyboard);
            }
        }

        // normalize framerate to 60 fps
        long nFrameTime = 16 - (nStartTime - getCurrentTimeInMilliseconds());
        if (nFrameTime > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(nFrameTime));
        } else {
            std::cout << "Warning " << nFrameTime << std::endl;
        }
        updateFps();
    }

    m_pRenderWindow->cleanUp();
    // Mix_FreeMusic(music);
    SDL_Quit();
    return 0;
}

void CarSimulator::generateTrack() {

    m_vTrackPosints.push_back(YPos(100, 260));
    m_vTrackPosints.push_back(YPos(100, 160));
    m_vTrackPosints.push_back(YPos(500, 100));
    m_vTrackPosints.push_back(YPos(800, 150));
    m_vTrackPosints.push_back(YPos(750, 250));
    m_vTrackPosints.push_back(YPos(1000, 230));
    m_vTrackPosints.push_back(YPos(1100, 300));
    m_vTrackPosints.push_back(YPos(1100, 400));
    m_vTrackPosints.push_back(YPos(950, 600));
    m_vTrackPosints.push_back(YPos(550,360));
    m_vTrackPosints.push_back(YPos(250,600));


    // render car
    m_pRenderCar = new RenderCar(
        m_vTrackPosints[0],
        RenderColor(255,255,255,255)
    );
    m_pRenderWindow->addObject(m_pRenderCar);
    m_pRenderCar->setLinearVelocity(2.0);

    std::cout << m_vTrackPosints.size() << std::endl;
    for (int i = 0; i < m_vTrackPosints.size(); i++) {
        int n_next = (i + 1) % m_vTrackPosints.size();
        YPos p0 = m_vTrackPosints[i];
        YPos p1 = m_vTrackPosints[n_next];
        
        YLine lineTrack(p0, p1);

        float x21 = p1.x() - p0.x();
        float y21 = p1.y() - p0.y();
        float len = lineTrack.getLength();
        float distance = 5.0;
        
        float k = distance / len;

        float dx = x21 * k;
        float dy = y21 * k;

        float angle = -PI/2;
        float dx3 = distance * (dx * std::cos(angle) - dy * std::sin(angle));
        float dy3 = distance * (dx * std::sin(angle) + dy * std::cos(angle));
        YPos d3(dx3, dy3);

        angle = PI/2;
        float dx4 = distance * (dx * std::cos(angle) - dy * std::sin(angle));
        float dy4 = distance * (dx * std::sin(angle) + dy * std::cos(angle));
        YPos d4(dx4, dy4);
        
        std::cout
            << "len = " << len
            << std::endl;

        m_vTrackLeftLines.push_back(YLine(p0 + d3, p1 + d3));
        m_vTrackRightLines.push_back(YLine(p0 + d4, p1 + d4));

        // m_pRenderWindow->addObject(new RenderLine(
        //     YPos(p0.x(), p0.y()),
        //     YPos(p1.x(), p1.y()),
        //     RenderColor(255,255,255,255)
        // ));
    }

    for (int i = 0; i < m_vTrackLeftLines.size(); i++) {
        int n_next = (i + 1) % m_vTrackLeftLines.size();
        YLine line0 = m_vTrackLeftLines[i];
        YLine line1 = m_vTrackLeftLines[n_next];
        YPos p = line0.findIntersectStraightLines(line1);
        m_vTrackLeftLines[i].set1(p);
        m_vTrackLeftLines[n_next].set0(p);
    }

    for (int i = 0; i < m_vTrackRightLines.size(); i++) {
        int n_next = (i + 1) % m_vTrackRightLines.size();
        YLine line0 = m_vTrackRightLines[i];
        YLine line1 = m_vTrackRightLines[n_next];
        YPos p = line0.findIntersectStraightLines(line1);
        m_vTrackRightLines[i].set1(p);
        m_vTrackRightLines[n_next].set0(p);
    }
    
    for (int i = 0; i < m_vTrackLeftLines.size(); i++) {
        YLine lineLeft = m_vTrackLeftLines[i];
        m_pRenderWindow->addObject(new RenderLine(
            lineLeft.p0(),
            lineLeft.p1(),
            RenderColor(255,100,100,100)
        ));

        YLine lineRight = m_vTrackRightLines[i];
        m_pRenderWindow->addObject(new RenderLine(
            lineRight.p0(),
            lineRight.p1(),
            RenderColor(100,100,255,255)
        ));
    }

}

void CarSimulator::startFpsCounting() {
    m_nFpsNumberOfFrames = 0;
    m_nFpsStartTime = getCurrentTimeInMilliseconds();
    m_nFpsElapsed = 0;
}

void CarSimulator::updateFps() {
    m_nFpsNumberOfFrames++;
    m_nFpsElapsed = getCurrentTimeInMilliseconds() - m_nFpsStartTime;
    if (m_nFpsElapsed > 1000) {
        double nFPS = m_nFpsNumberOfFrames;
        nFPS /= m_nFpsElapsed;
        nFPS *= 1000;
        this->updateFpsValue(nFPS);
        m_nFpsStartTime = getCurrentTimeInMilliseconds();
        m_nFpsNumberOfFrames = 0;
    }
}

void CarSimulator::updateFpsValue(int nFps) {
    // m_pFpsText->setText(L"FPS: ~" + std::to_wstring(nFps));
    std::cout << "FPS: ~" << nFps << std::endl;
}

MainState CarSimulator::getMainState() {
    return m_nCurrentState;
}

void CarSimulator::setMainState(const MainState &newMainState) {
    m_nCurrentState = newMainState;
}

void CarSimulator::clearWindow() {
    m_pRenderWindow->clear();
}

void CarSimulator::modifyObjects() {
    m_pRenderWindow->modifyObjects(*m_pCarSimulatorState);
}

void CarSimulator::drawObjects() {
    m_pRenderWindow->drawObjects(*m_pCarSimulatorState);
}

void CarSimulator::handleKeyboardCommand(YKeyboard *pKeyboard) {
    if (pKeyboard->isEscape()) {
        setMainState(MainState::GAME_EXIT);
    }
}

YPos CarSimulator::getRandomTrackPoint() {
    const int nPadding = 50;
    int x = std::rand() % (m_nWindowWidth - nPadding*2) + nPadding;
    int y = std::rand() % (m_nWindowHeight - nPadding*2) + nPadding;
    return YPos(x,y);
}