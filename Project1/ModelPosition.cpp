#include <list>
#include "GL/freeglut.h"
#include "ModelPosition.h"

using namespace std;

// Buffer with calculated model positions
unsigned int uBufferSize = 16348;
std::list<sPosition> TotalPositions;

void ClearPositionList()
{
    TotalPositions.clear();
}
void InsertPositionToList(const sPosition NewPos)
{
    if (TotalPositions.size() >= uBufferSize)
        TotalPositions.pop_front();
    TotalPositions.push_back(NewPos);
}
unsigned int GetPositionListMaxSize()
{
    return uBufferSize;
}
void RenderPositionsPoints(float fRMax, float fGMax, float fBMax, unsigned int mwindW, unsigned int mwindH)
{
    int iCnt, iSize;
    // Dibujar un punto aleatorio
    float fRandomR, fRandomG, fRandomB;
    int iRandomX, iRandomY;

    std::list<sPosition>::iterator ListIterator;
    sPosition CurrentPos;

    iSize = TotalPositions.size();
    for (iCnt = 0, ListIterator = TotalPositions.begin(); ListIterator != TotalPositions.end(); ListIterator++, iCnt++)
    {
        CurrentPos = *ListIterator;

        fRandomR = 0.5f * (1.0f + fRMax * (float)iCnt / (float)iSize);
        fRandomG = 0.5f * (1.0f + fGMax * (float)iCnt / (float)iSize);
        fRandomB = 0.5f * (1.0f + fBMax * (float)iCnt / (float)iSize);

        iRandomX = mwindW + CurrentPos.iPosX;
        iRandomY = mwindH + CurrentPos.iPosY;

        glColor3f(fRandomR, fRandomG, fRandomB);
        glBegin(GL_POINTS);
        glVertex2f((float)iRandomX, (float)iRandomY);
        glEnd();
    }
}