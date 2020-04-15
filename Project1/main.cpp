#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <list>
#include "GL/freeglut.h"

using namespace std;

#undef PI               
#define PI 3.141592657f

void RenderString(float fX, float fY, float fRed, float fGreen, float fBlue, void* font, const char* string);

GLint windW = 300, windH = 300;
GLint mwindW, mwindH;
long int iTime = 0;
long int iTimeSpeed = 2;
float fSpeed = 2000.0f;
float fSpeedTheta = 2.0f * PI / fSpeed;
float fRadio = 200.0f;
int iBufferSize = 1024;
float fRMax = 1.0f;
float fGMax = 1.0f;
float fBMax = 1.0f;
int iDrawingType = 2;
int iMaxDrawingType = 3;

float fLissajousA = 1.0f;
float fLissajousP = 3.0f;
float fLissajousR = PI / 2;
float fLissajousQ = 7.0f;
float fLissajousDelta = PI / 2;

struct sPosition
{
    int iPosX;
    int iPosY;
    int iTime;
};

std::list<sPosition> TotalPositions;


void MainDrawFunction()
{
    int iCnt, iSize;
    // Dibujar un punto aleatorio
    float fRandomR, fRandomG, fRandomB;
    int iRandomX, iRandomY;
    char sTexto[255];
    std::list<sPosition>::iterator ListIterator;

    glClear(GL_COLOR_BUFFER_BIT);
    iSize = TotalPositions.size();
    for (iCnt=0, ListIterator = TotalPositions.begin(); ListIterator != TotalPositions.end(); ListIterator++, iCnt++)
    {
        sPosition CurrentPos = *ListIterator;

        fRandomR = fRMax * (float) iCnt / (float) iSize;
        fRandomG = fGMax * (float) iCnt / (float) iSize;
        fRandomB = fBMax * (float) iCnt / (float) iSize;

        iRandomX = (float) mwindW + CurrentPos.iPosX;
        iRandomY = (float) mwindH + CurrentPos.iPosY;

        glColor3f(fRandomR , fRandomG , fRandomB );
        //glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
        glVertex2f((float)iRandomX, (float)iRandomY);
        glEnd();
    }
    // Mostrar en texto la variable tiempo
    snprintf(sTexto, 255, "%i speed: %i t:%i (%i)", iDrawingType, iTimeSpeed, iTime, iBufferSize);
    RenderString(10.0f, 10.0f, 1.0f, 1.0f, 1.0f, GLUT_BITMAP_TIMES_ROMAN_10, sTexto);
}

void MainIdleFunction()
{
    sPosition NewPos;
    float fRadius;
    float fTheta;
    switch (iDrawingType)
    {
    case 1:
        // Creamos nuevo punto - rosa de 5 petalos - en polares - R=sin(4*t), O=t
        fTheta = iTime * fSpeedTheta;
        fRadius = sin(4.0f * fTheta);
        // Convertimos de polares a XY
        NewPos.iPosX = fRadio * fRadius * cos(fTheta);
        NewPos.iPosY = fRadio * fRadius * sin(fTheta);
        NewPos.iTime = iTime;
        break;
    case 2:
        // Creamos nuevo punto - Lissajous en polares -  R = A*sin(p*t), O = R*sin (q*t+d)
        fRadius = fLissajousA * sin(fLissajousP * iTime * fSpeedTheta);
        fTheta = fLissajousR * sin(fLissajousQ * iTime * fSpeedTheta + fLissajousDelta);
        // Convertimos de polares a XY
        NewPos.iPosX = fRadio * fRadius * cos(fTheta);
        NewPos.iPosY = fRadio * fRadius * sin(fTheta);
        NewPos.iTime = iTime;
        break;
    case 3:
        // Creamos nuevo punto - Lissajous en cartesianas -  x = sin(p*t) y = sin (q*t+d)
        NewPos.iPosX = fRadio * sin(fLissajousP * iTime * fSpeedTheta);
        NewPos.iPosY = fRadio * sin(fLissajousQ * iTime * fSpeedTheta + fLissajousDelta);
        NewPos.iTime = iTime;
        break;
    }

    // Insertamos nuevo punto en la lista, eliminado si hay mas de 255
    if (TotalPositions.size() >= iBufferSize)
        TotalPositions.pop_front();
    TotalPositions.push_back(NewPos);
    // Avanzamos
    iTime = iTime + iTimeSpeed;
}

void MainKeyFunction(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ',':
        if (iDrawingType == 1) iDrawingType = iMaxDrawingType; else iDrawingType--;
        TotalPositions.clear();
        break;
    case '.':
        if (iDrawingType == iMaxDrawingType) iDrawingType = 1; else iDrawingType++;
        TotalPositions.clear();
        break;
    case '+':
        iTimeSpeed++;
        break;
    case '-':
        if (iTimeSpeed>1) iTimeSpeed--;
        break;
    case 'a':
        iBufferSize++;
        break;
    case 'z':
        if (iBufferSize > 1)
        {
            iBufferSize--;
            if (TotalPositions.size() > iBufferSize)
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


// ------------------------------------------------------------------------------------------------------------------------------------------
//   .... No changes ....
// ------------------------------------------------------------------------------------------------------------------------------------------
static void Init(void)
{
    TotalPositions.clear();
    iTime = 0;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glDisable(GL_DITHER);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderString(float fX, float fY, float fRed, float fGreen, float fBlue, void* font, const char* string)
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
