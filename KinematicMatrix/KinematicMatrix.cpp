// KinematicMatrix.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <list>
#include <map>
using namespace std;

#undef PI               
#define PI 3.141592657f

struct sCoords
{
    int iX, iY;
};
typedef std::list<sCoords> tListCoords;

struct sAngles
{
    int iAlpha, iBeta;
};
typedef std::list<sAngles> tListAngles;

int iLongArm1 = 150;
int iLongArm2 = 150;
sCoords iCoordinates[360][360];

void FillCoordinates()
{
    float fAlpha, fBeta;
    float fAlphaDelta, fBetaDelta;
    int iCntAlpha, iCntBeta;
    sCoords Coords;

    for (fAlpha=0.0f, iCntAlpha = 0, fAlphaDelta=2*PI/360.0f; iCntAlpha < 360; iCntAlpha++, fAlpha+=fAlphaDelta)
    {
        for (fBeta = 0.0f, iCntBeta = 0, fBetaDelta = 2 * PI / 360.0f; iCntBeta < 360; iCntBeta++, fBeta += fBetaDelta)
        {
            float fX1, fY1;
            float fX2, fY2;
            float fPosX, fPosY;
            fX1 = iLongArm1 * cos(fAlpha);
            fY1 = iLongArm1 * sin(fAlpha);
            fX2 = iLongArm2 * cos(fBeta);
            fY2 = iLongArm2 * sin(fBeta);
            fPosX = fX1 + fX2;
            fPosY = fY1 + fY2;

            Coords.iX = (int)fPosX;
            Coords.iY = (int)fPosY;
            iCoordinates[iCntAlpha][iCntBeta] = Coords;
        }

    }
}

tListAngles FindSolution(const sCoords CheckCoords)
{
    tListAngles ListAngles;
    sAngles ValidAngles;
    int iCntAlpha, iCntBeta;
    
    for (iCntAlpha = 0; iCntAlpha < 360; iCntAlpha++)
    {
        for (iCntBeta = 0; iCntBeta < 360; iCntBeta++)
        {
            if ((iCoordinates[iCntAlpha][iCntBeta].iX == CheckCoords.iX) && (iCoordinates[iCntAlpha][iCntBeta].iY == CheckCoords.iY))
            {
                ValidAngles.iAlpha = iCntAlpha;
                ValidAngles.iBeta = iCntBeta;
                ListAngles.push_front(ValidAngles);
            }
        }
    }
    return ListAngles;
}
void ShowSolution(int iX, int iY)
{
    sCoords ResultXY;
    tListAngles Results;
    tListAngles::const_iterator ResultsIter;

    ResultXY.iX = iX; ResultXY.iY = iY; Results = FindSolution(ResultXY);
    for (ResultsIter = Results.begin(); ResultsIter != Results.end(); ResultsIter++)
    {
        sAngles Angles = *ResultsIter;
        cout << "X: " << ResultXY.iX << " Y: " << ResultXY.iY << " Alpha: " << Angles.iAlpha << " Beta: " << Angles.iBeta << endl;
    }
}

int main()
{
    cout << "Generating coordinates" << endl;
    FillCoordinates();
    ShowSolution(30, 15);
    ShowSolution(30, 30);
    ShowSolution(30, 45);
    ShowSolution(50, 15);
    ShowSolution(80, 30);
    cout << "Generated" << endl;
}

