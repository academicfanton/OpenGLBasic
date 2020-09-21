#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <list>
#include "GL/freeglut.h"

using namespace std;

// Constants to manage the figure speed and aspect
#undef PI               
#define PI 3.141592657f
const float fSpeed = 2000.0f;
const float fRadio = 200.0f;
const float fSpeedTheta = 2.0f * PI / fSpeed;
const int iLonArm1 = 140;
const int iLonArm2 = 140;

// Image related global variables
// Changed on startup and when resized. Current width and height, and half of it (m).
GLint windW = 300, windH = 300;
GLint mwindW, mwindH;
// Colour to render points (fades down to black). Changed when pressing space key to a random colour.
float fRMax = 1.0f;
float fGMax = 1.0f;
float fBMax = 1.0f;

// Time global related variables
long unsigned int uTime = 0;
long unsigned int uTimeSpeed = 1;
bool bMovingToPoint = false;

// Global variable: what are we rendering?
int iDrawingType = 2;
int iMaxDrawingType = 3;

void ResetTimeAndZeroPos();
void RenderString(const float fX, const float fY, const float fRed, const float fGreen, const float fBlue, void* font, const char* string);


// Position for all models: X,Y, time 
struct sPosition
{
    int iPosX;
    int iPosY;
    unsigned int uTime;
};

// Buffer with calculated model positions
unsigned int uBufferSize = 16348;
std::list<sPosition> TotalPositions;


// Model 1
//  .. 8 leafs rose using a simple polar formula
//       Theta = t
//       R = sin(fRadiusSpeed*Theta)
const float fRadiusSpeed_Model1 = 4.0f;
sPosition GetPoint_Model1(const unsigned int uTime, const float fRadius);
// -- Model variables -- 
// Drawing type 2: Lissajous curve using polar coordinates
//       R = fLissajousA*sin(fLissajousP*t), 
//       Theta = fLissajousR*sin(fLissajousQ*t+fLissajousDelta)
const float fLissajousA = 1.0f;
const float fLissajousP = 3.0f;
const float fLissajousR = PI;  // /2
const float fLissajousQ = 7.0f;
const float fLissajousDelta = PI / 2;
sPosition GetPoint_Model2(const unsigned int uTime, const float fRadius);
// Drawing type 3: Lissajous curve using cartesian coordinates
//       X = sin(fLissajousP*t)
//       Y = sin (fLissajousQ*t+fLissajousDelta)
sPosition GetPoint_Model3(const unsigned int uTime, const float fRadius);

// Valid solution for 2 arm robot positioning
// Q1: Angle of first arm
// Q2: Angle of second arm
// X1,Y1 to X2,Y2: first arm coordinates
// X2,Y2 to X3,Y3: second arm coordinates
struct sTwoArmPosition
{
    float fQ1;
    float fQ2;
    float fX1;
    float fY1;
    float fX2;
    float fY2;
    float fX3;
    float fY3;
};
// All points have 2 valid solutions. We will calculate both and then choose one.
struct sTwoArmSolutions
{
    sTwoArmPosition Sol1;
    sTwoArmPosition Sol2;
};
// Calculate both solutions for arms to position Pos
sTwoArmSolutions GetTwoArmSolutionsFromPosition(const sPosition Pos, const float fArm1Length, const float fArm2Length);
// Given current positioning (CurrentPos), calculate best solution from solutions pair
sTwoArmPosition GetTwoArmNextBestSolution(const sTwoArmSolutions Sols, const sTwoArmPosition CurrentPos);
// Arms variables
sTwoArmPosition LastArmPosition;
sTwoArmPosition NextArmPosition;
sTwoArmPosition CurrentArmPosition;
const float fMaxArmSpeed = PI / 1024.0f;
float fQ1Speed=0.0f;
float fQ2Speed = 0.0f;
std::list<sTwoArmPosition> ArmPositions;
// Buffer with calculated model positions
std::list<sTwoArmPosition> ArmGranularPositions;

float GetAngleDiff(const float fAngle1, const float fAngle2)
{
    // a = targetA - sourceA
    // a -= 360 if a > 180
    // a += 360 if a < -180
    float fDiff;
    fDiff = fAngle1 - fAngle2;
    if (fDiff > PI)
        fDiff -= 2.0f * PI;
    if (fDiff < -PI)
        fDiff += 2.0f * PI;
    return fDiff;
}

sPosition GetPoint_Model1(const unsigned int uTime, const float fRadius)
{
    sPosition NewPos;
    float fRad;
    float fTheta;
    // Creamos nuevo punto - rosa de 5 petalos - en polares - R=sin(4*t), O=t
    fTheta = uTime * fSpeedTheta;
    fRad = sinf(fRadiusSpeed_Model1 * fTheta);
    // Convertimos de polares a XY
    NewPos.iPosX = (int)(fRadius * fRad * cosf(fTheta));
    NewPos.iPosY = (int)(fRadius * fRad * sinf(fTheta));
    NewPos.uTime = uTime;
    return NewPos;
}
sPosition GetPoint_Model2(const unsigned int uTime, const float fRadius)
{
    sPosition NewPos;
    float fRad;
    float fTheta;
    // Creamos nuevo punto - Lissajous en polares -  R = A*sin(p*t), O = R*sin (q*t+d)
    fRad = fLissajousA * sinf(fLissajousP * uTime * fSpeedTheta);
    fTheta = fLissajousR * sinf(fLissajousQ * uTime * fSpeedTheta + fLissajousDelta);
    // Convertimos de polares a XY
    NewPos.iPosX = (int)(fRadio * fRad * cosf(fTheta));
    NewPos.iPosY = (int)(fRadio * fRad * sinf(fTheta));
    NewPos.uTime = uTime;
    return NewPos;

}
sPosition GetPoint_Model3(const unsigned int uTime, const float fRadius)
{
    sPosition NewPos;
    // Creamos nuevo punto - Lissajous en cartesianas -  x = sin(p*t) y = sin (q*t+d)
    NewPos.iPosX = (int)(fRadio * sinf(fLissajousP * uTime * fSpeedTheta));
    NewPos.iPosY = (int)(fRadio * sinf(fLissajousQ * uTime * fSpeedTheta + fLissajousDelta));
    NewPos.uTime = uTime;
    return NewPos;
}

sTwoArmSolutions GetTwoArmSolutionsFromPosition(const sPosition Pos, const float fArm1Length, const float fArm2Length)
{
    sTwoArmSolutions Sols;
    float fX, fY;
    float fR, fR2;

    float fLArm2;
    float fQ1Sol1, fQ2Sol1;
    float fQ1Sol2, fQ2Sol2;
    float fX1, fY1;
    float fX2, fY2;
    float fX3, fY3;
    float fX2p, fY2p;
    float fX3p, fY3p;


    fX = (float) Pos.iPosX;
    fY = (float) Pos.iPosY;
    fR2 = fX * fX + fY * fY;
    fR = sqrtf(fR2);
    fLArm2 = fArm1Length * fArm1Length + fArm2Length * fArm2Length;

    fQ2Sol1 = acosf((fR2 - fLArm2) / (2.0f * fArm1Length * fArm2Length));
    fQ1Sol1 = atan2f(fY, fX) - atan2f(fArm2Length * sinf(fQ2Sol1), fArm1Length + fArm2Length * cosf(fQ2Sol1));

    fQ2Sol2 = -acosf((fR2 - fLArm2) / (2.0f * fArm1Length * fArm2Length));
    fQ1Sol2 = atan2f(fY, fX) + atan2f(fArm2Length * sinf(-fQ2Sol2), fArm1Length + fArm2Length * cosf(-fQ2Sol2));

    //fX1 = (float)mwindW;
    //fY1 = (float)mwindH;
    fX1 = 0.0f;
    fY1 = 0.0f;
    fX2 = fX1 + iLonArm1 * cosf(fQ1Sol1);
    fY2 = fY1 + iLonArm1 * sinf(fQ1Sol1);
    fX3 = fX2 + iLonArm2 * cosf(fQ2Sol1 + fQ1Sol1);
    fY3 = fY2 + iLonArm2 * sinf(fQ2Sol1 + fQ1Sol1);

    fX2p = fX1 + iLonArm1 * cosf(fQ1Sol2);
    fY2p = fY1 + iLonArm1 * sinf(fQ1Sol2);
    fX3p = fX2p + iLonArm2 * cosf(fQ2Sol2 + fQ1Sol2);
    fY3p = fY2p + iLonArm2 * sinf(fQ2Sol2 + fQ1Sol2);

    Sols.Sol1.fQ1 = fQ1Sol1;
    Sols.Sol1.fQ1 = fQ2Sol1;
    Sols.Sol1.fX1 = fX1;
    Sols.Sol1.fY1 = fY1;
    Sols.Sol1.fX2 = fX2;
    Sols.Sol1.fY2 = fY2;
    Sols.Sol1.fX3 = fX3;
    Sols.Sol1.fY3 = fY3;

    Sols.Sol2.fQ1 = fQ1Sol2;
    Sols.Sol2.fQ1 = fQ2Sol2;
    Sols.Sol2.fX1 = fX1;
    Sols.Sol2.fY1 = fY1;
    Sols.Sol2.fX2 = fX2p;
    Sols.Sol2.fY2 = fY2p;
    Sols.Sol2.fX3 = fX3p;
    Sols.Sol2.fY3 = fY3p;

    return Sols;
}

sTwoArmPosition GetTwoArmNextBestSolution(const sTwoArmSolutions Sols, const sTwoArmPosition CurrentPos)
{
    sTwoArmPosition NewPos;
    float fR2;
    float fD1, fD2, fD;
    float fD1p, fD2p, fDp;
    
    fR2 = Sols.Sol1.fX3 * Sols.Sol1.fX3 + Sols.Sol1.fY3 * Sols.Sol1.fY3;
    if (fR2 < 2.0f)
    {
        NewPos = CurrentPos;
    }
    else
    {
        //fD1 = fabs(GetAngleDiff(Sols.Sol1.fQ1,CurrentPos.fQ1) * (float)iLonArm1 * (float)iLonArm1);
        //fD2 = fabs(GetAngleDiff(Sols.Sol1.fQ2,CurrentPos.fQ2) * (float)iLonArm2 * (float)iLonArm2);
        //fD1p = fabs(GetAngleDiff(Sols.Sol2.fQ1,CurrentPos.fQ1) * (float)iLonArm1 * (float)iLonArm1);
        //fD2p = fabs(GetAngleDiff(Sols.Sol2.fQ2,CurrentPos.fQ2) * (float)iLonArm2 * (float)iLonArm2);
        fD1 = (Sols.Sol1.fX2 - CurrentPos.fX2) * (Sols.Sol1.fX2 - CurrentPos.fX2) + (Sols.Sol1.fY2 - CurrentPos.fY2) * (Sols.Sol1.fY2 - CurrentPos.fY2);
        fD2 = (Sols.Sol1.fX3 - CurrentPos.fX3) * (Sols.Sol1.fX3 - CurrentPos.fX3) + (Sols.Sol1.fY3 - CurrentPos.fY3) * (Sols.Sol1.fY3 - CurrentPos.fY3);
        fD1p = (Sols.Sol2.fX2 - CurrentPos.fX2) * (Sols.Sol2.fX2 - CurrentPos.fX2) + (Sols.Sol2.fY2 - CurrentPos.fY2) * (Sols.Sol2.fY2 - CurrentPos.fY2);
        fD2p = (Sols.Sol2.fX3 - CurrentPos.fX3) * (Sols.Sol2.fX3 - CurrentPos.fX3) + (Sols.Sol2.fY3 - CurrentPos.fY3) * (Sols.Sol2.fY3 - CurrentPos.fY3);
        fD = fD1 + fD2;
        fDp = fD1p + fD2p;
        if (fD < fDp)
            NewPos = Sols.Sol1;
        else 
            NewPos = Sols.Sol2;
    }

    return NewPos;
}


void MainDrawFunction()
{
    int iCnt, iSize;
    // Dibujar un punto aleatorio
    float fRandomR, fRandomG, fRandomB;
    int iRandomX, iRandomY;
    char sTexto[255];
    std::list<sPosition>::iterator ListIterator;
    sPosition CurrentPos;

    glClear(GL_COLOR_BUFFER_BIT);
    iSize = TotalPositions.size();
    for (iCnt=0, ListIterator = TotalPositions.begin(); ListIterator != TotalPositions.end(); ListIterator++, iCnt++)
    {
        CurrentPos = *ListIterator;

        fRandomR = fRMax * (float) iCnt / (float) iSize;
        fRandomG = fGMax * (float) iCnt / (float) iSize;
        fRandomB = fBMax * (float) iCnt / (float) iSize;

        iRandomX = mwindW + CurrentPos.iPosX;
        iRandomY = mwindH + CurrentPos.iPosY;

        glColor3f(fRandomR , fRandomG , fRandomB );
        //glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
        glVertex2f((float)iRandomX, (float)iRandomY);
        glEnd();
    }
    if (iCnt != 0)
    {
        sTwoArmPosition ArmPos;
        ArmPos = LastArmPosition;

        glBegin(GL_LINES);
        glColor3f(0.4f, 0.4f, 0.8f);
        glVertex2f(mwindW + ArmPos.fX1, mwindH + ArmPos.fY1);
        glVertex2f(mwindW + ArmPos.fX2, mwindH + ArmPos.fY2);
        glEnd();
        glBegin(GL_LINES);
        glColor3f(0.6f, 0.6f, 0.8f);
        glVertex2f(mwindW + ArmPos.fX2, mwindH + ArmPos.fY2);
        glVertex2f(mwindW + ArmPos.fX3, mwindH + ArmPos.fY3);
        glEnd();

    }
    // Mostrar en texto la variable tiempo
    snprintf(sTexto, 255, "%i speed: %lu t:%lu (%u)", iDrawingType, uTimeSpeed, uTime, uBufferSize);
    RenderString(10.0f, 10.0f, 1.0f, 1.0f, 1.0f, GLUT_BITMAP_TIMES_ROMAN_10, sTexto);
}

void InsertNextPoint()
{
    sPosition NewPos;
    // Get new point
    switch (iDrawingType)
    {
    case 1:
        NewPos = GetPoint_Model1(uTime, fRadio);
        break;
    case 2:
        NewPos = GetPoint_Model2(uTime, fRadio);
        break;
    case 3:
        NewPos = GetPoint_Model3(uTime, fRadio);
        break;
    }
    // Insert new point to the positions list, up to uBufferSize
    if (TotalPositions.size() >= uBufferSize)
        TotalPositions.pop_front();
    TotalPositions.push_back(NewPos);
    // Calculate arms position and add to the list
    sTwoArmSolutions Sols;
    sTwoArmPosition NewArmPosition;
    Sols = GetTwoArmSolutionsFromPosition(NewPos, (float)iLonArm1, (float)iLonArm2);
    NewArmPosition = GetTwoArmNextBestSolution(Sols, LastArmPosition);
    // Insert new point to the positions list, up to uBufferSize
    if (ArmPositions.size() >= uBufferSize)
        ArmPositions.pop_front();
    ArmPositions.push_back(NewArmPosition);
    NextArmPosition = NewArmPosition;
    // Move time forward
    uTime = uTime + uTimeSpeed;
}

void MovingPointNextStep()
{
    // If movement finished, get next point and move ahead
    bMovingToPoint = false;
    LastArmPosition = NextArmPosition;
}

void CalculateArmsMovement()
{
    float fQ1Diff;
    float fQ2Diff;

    CurrentArmPosition = LastArmPosition;
    fQ1Diff = GetAngleDiff(CurrentArmPosition.fQ1,NextArmPosition.fQ1);
    fQ2Diff = GetAngleDiff(CurrentArmPosition.fQ2, NextArmPosition.fQ2);
    //*+++++++
    const float fMaxArmSpeed = PI / 1024.0f;
    float fQ1Speed = 0.0f;
    float fQ2Speed = 0.0f;
    std::list<sTwoArmPosition> ArmPositions;
    // Buffer with calculated model positions
    std::list<sTwoArmPosition> ArmGranularPositions;

}
void MainIdleFunction()
{
    if (!bMovingToPoint)
    {
        InsertNextPoint();
        CalculateArmsMovement();
        bMovingToPoint = true;
    }
    else
    {
        MovingPointNextStep();
    }
}

// Keyboard handler:
//  . and , to change movement pattern
//  + and - to change movement speed (time - applies to points in the pattern - iTimeSpeed)
//  a and z to change number of points to render
//  Space to assign new random colour to draw the pattern

void MainKeyFunction(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ',':
        if (iDrawingType == 1) iDrawingType = iMaxDrawingType; else iDrawingType--;
        ResetTimeAndZeroPos();
        break;
    case '.':
        if (iDrawingType == iMaxDrawingType) iDrawingType = 1; else iDrawingType++;
        ResetTimeAndZeroPos();
        break;
    case '+':
        uTimeSpeed++;
        break;
    case '-':
        if (uTimeSpeed>1) uTimeSpeed--;
        break;
    case 'a':
        uBufferSize++;
        break;
    case 'z':
        if (uBufferSize > 1)
        {
            uBufferSize--;
            if (TotalPositions.size() > uBufferSize)
                TotalPositions.pop_front();
        }
        break;
    case ' ':
        fRMax = 0.5f + ((rand() & 0xFF) / 512.0f);
        fGMax = 0.5f + ((rand() & 0xFF) / 512.0f);
        fBMax = 0.5f + ((rand() & 0xFF) / 512.0f);
        break;
    }
}

void ResetTimeAndZeroPos()
{
    TotalPositions.clear();
    ArmPositions.clear();
    ArmGranularPositions.clear();
    uTime = 0;
    // Arms start in 0,0 position. If iLonArm1<>iLonArm2 - this would need to be adapted
    LastArmPosition.fQ1 = 0;
    LastArmPosition.fQ2 = PI;
    LastArmPosition.fX1 = 0.0f;
    LastArmPosition.fY1 = 0.0f;
    LastArmPosition.fX2 = iLonArm1;
    LastArmPosition.fY2 = 0.0f;
    LastArmPosition.fX3 = 0.0f;
    LastArmPosition.fY3 = 0.0f;
}


// ------------------------------------------------------------------------------------------------------------------------------------------
//   .... No changes ....
// ------------------------------------------------------------------------------------------------------------------------------------------
static void Init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glDisable(GL_DITHER);
    glClear(GL_COLOR_BUFFER_BIT); 
    ResetTimeAndZeroPos();
}

void RenderString(const float fX, const float fY, const float fRed, const float fGreen, const float fBlue, void* font, const char* string)
{
    glColor3f(fRed, fGreen, fBlue);
    glRasterPos2f(fX, fY);
    glutBitmapString(font, (const unsigned char *)string);
}

void Reshape(int width, int height)
{
    windW = width;
    windH = height;
    mwindW = windW >> 1;
    mwindH = windH >> 1;
    glViewport(0, 0, windW, windH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-0.5, windW + 0.5, -0.5, windH + 0.5);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void Key(unsigned char key, int x, int y)
{
    MainKeyFunction(key, x, y);
    switch (key)
    {
    case 27:
        exit(0);
    }
}

void Display(void)
{
    MainDrawFunction();
    glutSwapBuffers();
    //glFlush();
}

void Idle(void)
{
    MainIdleFunction();
    glutPostRedisplay();
}

void Visible(int state)
{
    if (state == GLUT_VISIBLE)
    {
        glutIdleFunc(Idle);
    }
    else
    {
        glutIdleFunc(NULL);
    }
}


int main(int argc, char** argv)
{
    GLenum type;


    windW = GetSystemMetrics(SM_CXSCREEN);
    windH = GetSystemMetrics(SM_CYSCREEN);

    if (windW > windH)
    {
        windW = windW >> 2;
        windH = windW;
    }
    else
    {
        windH = windH >> 2;
        windW = windH;
    }
    mwindW = windW >> 1;
    mwindH = windH >> 1;

    glutInitWindowSize(windW, windH);
    glutInit(&argc, argv);

    type = GLUT_RGB | GLUT_DOUBLE ;
    glutInitDisplayMode(type);
    glutCreateWindow("GLWINDOW");
    Init();
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutVisibilityFunc(Visible);
    glutDisplayFunc(Display);
    glutMainLoop();
    return 0;
}
