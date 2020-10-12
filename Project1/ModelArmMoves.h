#pragma once
#ifndef _MODELARMMOVES_H_
#define _MODELARMMOVES_H_
#include "ModelArmPosition.h"

#undef PI               
#define PI 3.141592657f

struct sArmsMove
{
	float fQ1StartPos;
	float fQ2StartPos;
	float fQ1EndPos;
	float fQ2EndPos;
	float fQ1SmallStep;
	float fQ2SmallStep;
	float fQ1LargeStep;
	float fQ2LargeStep;
	float fLenArm1;
	float fLenArm2;
	sTwoArmPosition CurrentPosition;
	bool bMoveCompleted;
	bool bMoveCompleted1;
	bool bMoveCompleted2;
};

const float fMaxArmSpeed = PI / 256.0f;
const float fMinArmSpeed = PI / 4096.0f;
sArmsMove CalculateArmsMovement(const sTwoArmPosition StartPos, const sTwoArmPosition EndPos, float fLenArm1, float fLenArm2);
bool AdvanceArmsMovement(sArmsMove* pArmsMove);

void ClearArmMovesList();

#endif 