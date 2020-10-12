#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <list>
#include "GL/freeglut.h"
#include "AngleMath.h"
#include "ModelPosition.h"
#include "ModelArmPosition.h"
#include "ModelArmMoves.h"
#include "EquationsModels.h"

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
int iDrawingType = 1;

void ResetTimeAndZeroPos();
void RenderString(const float fX, const float fY, const float fRed, const float fGreen, const float fBlue, void* font, const char* string);

// Arms variables
sArmsMove ArmsPositionMove;


void MainDrawFunction()
{
    char sTexto[255];
    glClear(GL_COLOR_BUFFER_BIT);
    RenderPositionsPoints(fRMax, fGMax, fBMax, 1.0f, 0.0f, 0.0f, mwindW, mwindH);
    RenderArmPosition(ArmsPositionMove.CurrentPosition,0.6f, 0.6f, 0.8f, mwindW, mwindH);
    // Mostrar en texto la variable tiempo
    snprintf(sTexto, 255, "%i speed: %lu t:%lu", iDrawingType, uTimeSpeed, uTime);
    RenderString(10.0f, 10.0f, 1.0f, 1.0f, 1.0f, GLUT_BITMAP_TIMES_ROMAN_10, sTexto);
}

void InsertNextPoint()
{
    sPosition NewPos;
    sTwoArmSolutions Sols;
    sTwoArmPosition NewArmPosition;
    // Get new point
    NewPos = Models[iDrawingType](uTime, fSpeedTheta, fRadio);
    // Insert new point to the positions list
    InsertCurvePositionToList(NewPos);
    // Calculate arms position and add to the list
    Sols = GetTwoArmSolutionsFromPosition(NewPos, (float)iLonArm1, (float)iLonArm2);
    NewArmPosition = GetTwoArmNextBestSolution(Sols, ArmsPositionMove.CurrentPosition);
    // Insert new arm point to the positions list
    InsertArmPositionToList(NewArmPosition);
    // Calculate the arms movement
    ArmsPositionMove = CalculateArmsMovement(ArmsPositionMove.CurrentPosition, NewArmPosition, (float)iLonArm1, (float)iLonArm2);
    // Move time forward
    uTime = uTime + uTimeSpeed;
}

void InsertCurrentArmsPositionAsNextPoint()
{
    sPosition NewPos;
    NewPos.iPosX = ArmsPositionMove.CurrentPosition.fX3;
    NewPos.iPosY = ArmsPositionMove.CurrentPosition.fY3;
    NewPos.uTime = uTime;
    InsertGranularPositionToList(NewPos);
}

void MovingPointNextStep()
{
    // If movement finished, get next point and move ahead
    bMovingToPoint = AdvanceArmsMovement(&ArmsPositionMove);
}

void MainIdleFunction()
{
    if (!bMovingToPoint)
    {
        // Calculate the next point
        InsertNextPoint();
        bMovingToPoint = true;
    }
    else
    {
        MovingPointNextStep();
    }
    InsertCurrentArmsPositionAsNextPoint();
}

// Keyboard handler:
//  . and , to change movement pattern
//  + and - to change movement speed (time - applies to points in the pattern - iTimeSpeed)
//  Space to assign new random colour to draw the pattern

void MainKeyFunction(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ',':
        if (iDrawingType == 0) iDrawingType = ModelsCount-1; else iDrawingType--;
        ResetTimeAndZeroPos();
        break;
    case '.':
        if (iDrawingType == ModelsCount-1) iDrawingType = 0; else iDrawingType++;
        ResetTimeAndZeroPos();
        break;
    case '+':
        uTimeSpeed++;
        break;
    case '-':
        if (uTimeSpeed>1) uTimeSpeed--;
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
    ClearPositionList();
    ClearArmPositionList();
    ClearArmMovesList();
    uTime = 0;
    bMovingToPoint = false;
    // Arms start in 0,0 position. If iLonArm1<>iLonArm2 - this would need to be adapted
    SetStartPosition(& ArmsPositionMove.CurrentPosition, (float)iLonArm1, (float)iLonArm2);
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
