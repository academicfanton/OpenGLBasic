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
unsigned int uBufferSize = 2048;
float fRMax = 1.0f;
float fGMax = 1.0f;
float fBMax = 1.0f;
int iDrawingType = 2;
int iMaxDrawingType = 3;

int iAxisDrawingType = 2;


float fLissajousA = 1.0f;
float fLissajousP = 3.0f;
float fLissajousR = PI / 2;
float fLissajousQ = 7.0f;
float fLissajousDelta = PI / 2;

// el BRAZO!
int iLonArm1=110, iLonArm2=110;
int iValidSolution = -1;
float fBX3 = 0, fBY3 = 0;
float fBX2 = 0, fBY2 = 0;

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
        float fR,fR2;
        float fLArm2;
        float fX, fY;
        float fQ1Sol1, fQ2Sol1;
        float fQ1Sol2, fQ2Sol2;
        float fX1, fY1;
        float fX2, fY2;
        float fX3, fY3;
        float fX2p, fY2p;
        float fX3p, fY3p;
        float fD1, fD2, fD;
        float fD1p, fD2p, fDp;


        fX = (float)iRandomX - mwindW;
        fY = (float)iRandomY - mwindH;
        fR2 = fX * fX + fY * fY;
        fR = sqrtf(fR2);
        fLArm2=(float)iLonArm1*iLonArm1+(float)iLonArm2*iLonArm2;

        fQ2Sol1 = acosf((fR2 - fLArm2) / (2.0f * iLonArm1 * iLonArm2));
        fQ1Sol1 = atan2f(fY, fX) - atan2f(iLonArm2*sinf(fQ2Sol1),(float)iLonArm1+(float)iLonArm2*cosf(fQ2Sol1));

        fQ2Sol2 = -acosf((fR2 - fLArm2) / (2.0f * iLonArm1 * iLonArm2));
        fQ1Sol2 = atan2f(fY, fX) + atan2f(iLonArm2 * sinf(-fQ2Sol2), (float)iLonArm1 + (float)iLonArm2 * cosf(-fQ2Sol2));

        fX1 = (float) mwindW;
        fY1 = (float) mwindH;
        fX2 = fX1   + iLonArm1 * cosf(fQ1Sol1);
        fY2 = fY1   + iLonArm1 * sinf(fQ1Sol1);
        fX3 = fX2   + iLonArm2 * cosf(fQ2Sol1 + fQ1Sol1);
        fY3 = fY2   + iLonArm2 * sinf(fQ2Sol1 + fQ1Sol1);

        fX2p = fX1  + iLonArm1 * cosf(fQ1Sol2);
        fY2p = fY1  + iLonArm1 * sinf(fQ1Sol2);
        fX3p = fX2p + iLonArm2 * cosf(fQ2Sol2 + fQ1Sol2);
        fY3p = fY2p + iLonArm2 * sinf(fQ2Sol2 + fQ1Sol2);

        
        if (iValidSolution == -1)
        {
            iValidSolution = 1;
            fD1 = fD2 = fD = 0.0f;
            fD1p = fD2p = fDp = 1.0f;
        }
        else
        {
            if (fR2 < 2.0f)
            {
                fX2p = fX2 = fBX2 ;
                fY2p = fY2 = fBY2 ;
                fX3p = fX3 = fX1;
                fY3p = fY3 = fY1;
            }
            else
            {
                fD1 = (fX2 - fBX2) * (fX2 - fBX2) + (fY2 - fBY2) * (fY2 - fBY2);
                fD2 = (fX3 - fBX3) * (fX3 - fBX3) + (fY3 - fBY3) * (fY3 - fBY3);
                fD = fD1 + fD2;

                fD1p = (fX2p - fBX2) * (fX2p - fBX2) + (fY2p - fBY2) * (fY2p - fBY2);
                fD2p = (fX3p - fBX3) * (fX3p - fBX3) + (fY3p - fBY3) * (fY3p - fBY3);
                fDp = fD1p + fD2p;

                if (fD < fDp)
                    iValidSolution = 1;
                else if (fDp < fD)
                    iValidSolution = 0;
            }
        }
        if (iValidSolution == 1)
        {
            fBX2 = fX2;
            fBY2 = fY2;
            fBX3 = fX3;
            fBY3 = fY3;
        }
        else
        {
            fBX2 = fX2p;
            fBY2 = fY2p;
            fBX3 = fX3p;
            fBY3 = fY3p;
        }

        glBegin(GL_LINES);
        glColor3f(0.4f, 0.4f, 0.8f);
        glVertex2f(fX1, fY1);
        glVertex2f(fBX2, fBY2);
        glEnd();
        glBegin(GL_LINES);
        glColor3f(0.6f, 0.6f, 0.8f);
        glVertex2f(fBX2, fBY2);
        glVertex2f(fBX3, fBY3);
        glEnd();


        //glBegin(GL_LINES);
        //glColor3f(0.8, 0.4, 0.4);
        //glVertex2f(fX1, fY1);
        //glVertex2f(fX2, fY2);
        //glEnd();
        //glBegin(GL_LINES);
        //glColor3f(0.8, 0.6, 0.6);
        //glVertex2f(fX2, fY2);
        //glVertex2f(fX3, fY3);
        //glEnd();

        //glBegin(GL_LINES);
        //glColor3f(0.8, 0.4, 0.4);
        //glVertex2f(fX1, fY1);
        //glVertex2f(fX2p, fY2p);
        //glEnd();
        //glBegin(GL_LINES);
        //glColor3f(0.8, 0.6, 0.6);
        //glVertex2f(fX2p, fY2p);
        //glVertex2f(fX3p, fY3p);
        //glEnd();

        // BRAZO
        //float h, a, b, g;
        //float x, y;
        //float ab, aa; 
        //ab es el angulo del brazo y aa en del antebrazo... el segundo no lo uso.. pero puede hacer falta
        //x = (float)iRandomX - mwindW;
        //y = (float)iRandomY - mwindH;
        //h = sqrt((x * x) + (y * y));
        //a = atan2(y, x);
        //b = acos(((float)(iLonArm1 * iLonArm1) - (float)(iLonArm2 * iLonArm2) + (h * h)) / (2 * (float)iLonArm1 * h));
        //ab = a + b;
        //g = acos(((float)(iLonArm1 * iLonArm1) + (float)(iLonArm2 * iLonArm2) - (h * h)) / (2 * (float)(iLonArm1 * iLonArm2)));
        //aa = g - PI; // g-180º 
        //
        //x = mwindW + iLonArm1 * cos(ab);
        //y = mwindH + iLonArm1 * sin(ab);
        //glBegin(GL_LINES);
        //glColor3f(0.4, 0.4, 0.8);
        //glVertex2f((float)mwindW, (float)mwindH);
        //glVertex2f(x, y);
        //glEnd();
        //glBegin(GL_LINES);
        //glColor3f(0.6, 0.6, 0.8);
        //glVertex2f(x, y);
        //glVertex2f((float)iRandomX, (float)iRandomY);
        //glEnd();

    }
    // Mostrar en texto la variable tiempo
    snprintf(sTexto, 255, "%i speed: %i t:%i (%u)", iDrawingType, iTimeSpeed, iTime, uBufferSize);
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
        fRadius = sinf(4.0f * fTheta);
        // Convertimos de polares a XY
        NewPos.iPosX = (int)(fRadio * fRadius * cosf(fTheta));
        NewPos.iPosY = (int)(fRadio * fRadius * sinf(fTheta));
        NewPos.iTime = iTime;
        break;
    case 2:
        // Creamos nuevo punto - Lissajous en polares -  R = A*sin(p*t), O = R*sin (q*t+d)
        fRadius = fLissajousA * sinf(fLissajousP * iTime * fSpeedTheta);
        fTheta = fLissajousR * sinf(fLissajousQ * iTime * fSpeedTheta + fLissajousDelta);
        // Convertimos de polares a XY
        NewPos.iPosX = (int)(fRadio * fRadius * cosf(fTheta));
        NewPos.iPosY = (int)(fRadio * fRadius * sinf(fTheta));
        NewPos.iTime = iTime;
        break;
    case 3:
        // Creamos nuevo punto - Lissajous en cartesianas -  x = sin(p*t) y = sin (q*t+d)
        NewPos.iPosX = (int)(fRadio * sinf(fLissajousP * iTime * fSpeedTheta));
        NewPos.iPosY = (int)(fRadio * sinf(fLissajousQ * iTime * fSpeedTheta + fLissajousDelta));
        NewPos.iTime = iTime;
        break;
    }

    // Insertamos nuevo punto en la lista, eliminado si hay mas de 255
    if (TotalPositions.size() >= uBufferSize)
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
        if (iDrawingType == 3)  iLonArm1 = iLonArm2 = 140; else iLonArm1 = iLonArm2 = 110;
        TotalPositions.clear();
        break;
    case '.':
        if (iDrawingType == iMaxDrawingType) iDrawingType = 1; else iDrawingType++;
        if (iDrawingType == 3)  iLonArm1 = iLonArm2 = 140; else iLonArm1 = iLonArm2 = 110;
        TotalPositions.clear();
        break;
    case '+':
        iTimeSpeed++;
        break;
    case '-':
        if (iTimeSpeed>1) iTimeSpeed--;
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
