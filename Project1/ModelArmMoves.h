#pragma once
#ifndef _MODELARMMOVES_H_
#define _MODELARMMOVES_H_

#undef PI               
#define PI 3.141592657f

struct sArmsMove
{
	float fQ1StartPos;
	float fQ2StartPos;
	float fQ1EndPos;
	float fQ2EndPos;
	float fQ1Step;
	float fQ2Step;
};

const float fMaxArmSpeed = PI / 1024.0f;
const float fMinArmSpeed = PI / 4096.0f;
sArmsMove CalculateArmsMovement(const sTwoArmPosition StartPos, const sTwoArmPosition EndPos);

void ClearArmMovesList();

#endif 