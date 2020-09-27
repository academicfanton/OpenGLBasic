#include <math.h>
#include <list>
#include "GL/freeglut.h"
#include "ModelArmPosition.h"
using namespace std;

std::list<sTwoArmPosition> ArmPositions;
unsigned int uBufferSizeArms = 16348;


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


    fX = (float)Pos.iPosX;
    fY = (float)Pos.iPosY;
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
    fX2 = fX1 + fArm1Length * cosf(fQ1Sol1);
    fY2 = fY1 + fArm1Length * sinf(fQ1Sol1);
    fX3 = fX2 + fArm2Length * cosf(fQ2Sol1 + fQ1Sol1);
    fY3 = fY2 + fArm2Length * sinf(fQ2Sol1 + fQ1Sol1);

    fX2p = fX1 + fArm1Length * cosf(fQ1Sol2);
    fY2p = fY1 + fArm1Length * sinf(fQ1Sol2);
    fX3p = fX2p + fArm2Length * cosf(fQ2Sol2 + fQ1Sol2);
    fY3p = fY2p + fArm2Length * sinf(fQ2Sol2 + fQ1Sol2);

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


void ClearArmPositionList()
{
    ArmPositions.clear();
}
void InsertArmPositionToList(const sTwoArmPosition NewArmPosition)
{
    if (ArmPositions.size() >= GetArmPositionListMaxSize())
        ArmPositions.pop_front();
    ArmPositions.push_back(NewArmPosition);
}

unsigned int GetArmPositionListMaxSize()
{
    return uBufferSizeArms;
}
void RenderArmPosition(const sTwoArmPosition ArmPos, float fRMax, float fGMax, float fBMax, unsigned int mwindW, unsigned int mwindH)
{
    glBegin(GL_LINES);
    glColor3f(fRMax*0.5f, fGMax * 0.5f, fBMax * 0.5f);
    glVertex2f(mwindW + ArmPos.fX1, mwindH + ArmPos.fY1);
    glVertex2f(mwindW + ArmPos.fX2, mwindH + ArmPos.fY2);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(fRMax , fGMax , fBMax );
    glVertex2f(mwindW + ArmPos.fX2, mwindH + ArmPos.fY2);
    glVertex2f(mwindW + ArmPos.fX3, mwindH + ArmPos.fY3);
    glEnd();

}
