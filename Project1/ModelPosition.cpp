#include <list>
#include "GL/freeglut.h"
#include "ModelPosition.h"

using namespace std;

// Buffer with calculated model positions
unsigned int uBufferSize = 65536;
std::list<sPosition> TotalPositions;
std::list<sPosition> CurvePositions;

void ClearPositionList()
{
    TotalPositions.clear();
    CurvePositions.clear();
}
void InsertCurvePositionToList(const sPosition NewPos)
{
    if (CurvePositions.size() >= uBufferSize)
        CurvePositions.pop_front();
    CurvePositions.push_back(NewPos);
}
void InsertGranularPositionToList(const sPosition NewPos)
{
    if (TotalPositions.size() >= uBufferSize)
        TotalPositions.pop_front();
    TotalPositions.push_back(NewPos);
}
unsigned int GetPositionListMaxSize()
{
    return uBufferSize;
}
void RenderPositionsPoints(float fRMax, float fGMax, float fBMax, float fRCurveMax, float fGCurveMax, float fBCurveMax, unsigned int mwindW, unsigned int mwindH)
{
    int iCnt, iSize;
    float fRandomR, fRandomG, fRandomB;
    int iRandomX, iRandomY;
    std::list<sPosition>::iterator ListIterator;
    sPosition CurrentPos;

    // Draw granular positions
    iSize = TotalPositions.size();
    glPointSize(5);
    glBegin(GL_POINTS);
    for (iCnt = 0, ListIterator = TotalPositions.begin(); ListIterator != TotalPositions.end(); ListIterator++, iCnt++)
    {
        CurrentPos = *ListIterator;

        fRandomR = 0.5f * (1.0f + fRMax * (float)iCnt / (float)iSize);
        fRandomG = 0.5f * (1.0f + fGMax * (float)iCnt / (float)iSize);
        fRandomB = 0.5f * (1.0f + fBMax * (float)iCnt / (float)iSize);

        iRandomX = mwindW + CurrentPos.iPosX;
        iRandomY = mwindH + CurrentPos.iPosY;

        glColor3f(fRandomR, fRandomG, fRandomB);
        glVertex2f((float)iRandomX, (float)iRandomY);
    }
    glEnd();
    // Draw curve positions
    iSize = CurvePositions.size();
    glPointSize(1);
    glBegin(GL_POINTS);
    for (iCnt = 0, ListIterator = CurvePositions.begin(); ListIterator != CurvePositions.end(); ListIterator++, iCnt++)
    {
        CurrentPos = *ListIterator;

        fRandomR = 0.5f * (1.0f + fRCurveMax * (float)iCnt / (float)iSize);
        fRandomG = 0.5f * (1.0f + fGCurveMax * (float)iCnt / (float)iSize);
        fRandomB = 0.5f * (1.0f + fBCurveMax * (float)iCnt / (float)iSize);

        iRandomX = mwindW + CurrentPos.iPosX;
        iRandomY = mwindH + CurrentPos.iPosY;

        glColor3f(fRandomR, fRandomG, fRandomB);
        glVertex2f((float)iRandomX, (float)iRandomY);
    }
    glEnd();
}