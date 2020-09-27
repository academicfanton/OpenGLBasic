#pragma once
#ifndef _MODELARMPOSITION_H_
#define _MODELARMPOSITION_H_
#include "ModelPosition.h"

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

void ClearArmPositionList();
void InsertArmPositionToList(const sTwoArmPosition NewArmPosition);
unsigned int GetArmPositionListMaxSize();
void RenderArmPosition(const sTwoArmPosition ArmPos,float fRMax, float fGMax, float fBMax, unsigned int mwindW, unsigned int mwindH);

#endif