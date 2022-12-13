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

#ifndef __CAR_SIMULATOR_H__
#define __CAR_SIMULATOR_H__

#include <string>
#include "SDL.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <cmath>
#define PI 3.14159265

long getCurrentTimeInMilliseconds();

class YPos {
    public:
        YPos();
        YPos(int x, int y);
        int x() const;
        int y() const;
        void update(int x, int y);
        void update(const YPos& coord);
        void setX(int x);
        void setY(int y);
        YPos& operator+=(const YPos& other);
        YPos& operator-=(const YPos& other);
        YPos operator+(const YPos& other) const;
        YPos operator-(const YPos& other) const;
        bool operator>(const YPos& other) const;
        bool operator<(const YPos& other) const;

        bool isInsideRect(const YPos& topLeft, const YPos& bottomRight) const;

    private:
        int m_nX, m_nY;
};

class YLine {
    public:
        YLine();
        YLine(YPos p0, YPos p1);
        YPos p0() const;
        YPos p1() const;
        void update(YPos p0, YPos p1);
        void set0(YPos p0);
        void set1(YPos p1);
        YPos findIntersectStraightLines(const YLine &line) const;
        float getLength() const;

    private:
        YPos m_p0;
        YPos m_p1;
};

class YKeyboard {

    public:
        YKeyboard();
        void pollState();
        bool isUp();
        bool isUpLeft();
        bool isUpRight();
        bool isDown();
        bool isDownLeft();
        bool isDownRight();
        bool isLeft();
        bool isRight();
        bool isF1();
        bool isF4();
        bool isF12();
        bool isSpace();
        bool isEscape();
        bool isK();
        bool isL();
        bool isG();
        bool isH();
    
    private:
        const Uint8 *m_pKeyboardStateArray;
};


class CarSimulatorState {

    public:
        CarSimulatorState(int windowWidth, int windowHeight);
};

class RenderColor {
    public:
        RenderColor(int nR, int nG, int nB, int nA);
        void changeRenderColor(SDL_Renderer* renderer);

    private:
        int m_nR;
        int m_nG;
        int m_nB;
        int m_nA;
};

class RenderObject;


class RenderObject {

    public:
        RenderObject(int nPositionZ);
        virtual ~RenderObject();
        int getPositionZ();
        bool isDestroyed();
        void destroy();
        virtual void modify(const CarSimulatorState& state);
        virtual bool canDraw(const CarSimulatorState& state) = 0;
        virtual void draw(SDL_Renderer* pRenderer) = 0;
        int randomNoise();

    protected:
        bool m_bDestroyed;
        int m_nPositionZ;
       
};

class RenderLine : public RenderObject {

    public:
        RenderLine(
            const YPos &p1,
            const YPos &p2,
            const RenderColor &color,
            int nPositionZ = 0
        );
        virtual void modify(const CarSimulatorState& state) override;
        virtual bool canDraw(const CarSimulatorState& state) override;
        virtual void draw(SDL_Renderer* renderer) override;

        const YPos &getAbsoluteCoord1();
        const YPos &getAbsoluteCoord2();
        const YPos &getCoord1();
        const YPos &getCoord2();
        void updateAbsoluteCoords(const YPos &p1, const YPos &p2);

    private:
        YPos m_startCoord1;
        YPos m_startCoord2;
        YPos m_coord1;
        YPos m_coord2;
        RenderColor m_color;
};

class RenderWindow {
    public:
        RenderWindow(const std::string &sTitle, int p_w, int p_h);
        ~RenderWindow();
        void cleanUp();
        void clear();
        void modifyObjects(const CarSimulatorState& state);
        void drawObjects(const CarSimulatorState& state);

        void addObject(RenderObject *pObject);
        void removeObject(RenderObject *pObject);

    private:
        SDL_Window* m_pWindow;
        SDL_Renderer* m_pRenderer;
        SDL_DisplayMode m_displayMode;
        SDL_Texture* m_pTextureBioplast;

        std::vector<RenderObject *> m_vObjects;
};

enum class MainState {
    LOADING,
    WAITING_SPACE,
    GAME_ACTION,
    GAME_EXIT
};

class CarSimulator {
    public:
        CarSimulator();
        bool initSDL2();
        
        int start();

        void generateTrack();

        void startFpsCounting();
        void updateFps();
        void updateFpsValue(int nFps);

        MainState getMainState();
        void setMainState(const MainState &newMainState);
        void clearWindow();
        void modifyObjects();
        void drawObjects();
        void handleKeyboardCommand(YKeyboard *pKeyboard);

    private:

        YPos getRandomTrackPoint();

        RenderWindow *m_pRenderWindow;
        std::string m_sWindowName;
        CarSimulatorState *m_pCarSimulatorState;

        int m_nWindowWidth;
        int m_nWindowHeight;

        long m_nFpsNumberOfFrames;
        long m_nFpsStartTime;
        long m_nFpsElapsed;

        MainState m_nCurrentState;


        std::vector<YPos> m_vTrackPosints;
        std::vector<YLine> m_vTrackLeftLines;
        std::vector<YLine> m_vTrackRightLines;
        

};

#endif // __CAR_SIMULATOR_H__