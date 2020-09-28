#include <list>
#include "AngleMath.h"
#include "ModelArmPosition.h"
#include "ModelArmMoves.h"

using namespace std;

float fQ1Speed = 0.0f;
float fQ2Speed = 0.0f;
// Buffer with calculated model positions
std::list<sTwoArmPosition> ArmGranularPositions;

sArmsMove CalculateArmsMovement(const sTwoArmPosition StartPos, const sTwoArmPosition EndPos)
{
    sArmsMove Move;
    float fQ1Diff;
    float fQ2Diff;
    float fQ1Step;
    float fQ2Step;

    Move.fQ1StartPos = StartPos.fQ1;
    Move.fQ2StartPos = StartPos.fQ2;
    Move.fQ1EndPos = EndPos.fQ1;
    Move.fQ2EndPos = EndPos.fQ2;
    fQ1Diff = GetAngleDiff(StartPos.fQ1, EndPos.fQ1);
    fQ2Diff = GetAngleDiff(StartPos.fQ2, EndPos.fQ2);
    if (fabs(fQ1Diff) < fMinArmSpeed)
        fQ1Step = 0.0f;
    else if (fabs(fQ1Diff) < fMaxArmSpeed)
        fQ1Step = copysignf(fMinArmSpeed,fQ1Diff);
    else 
        fQ1Step = copysignf(fMaxArmSpeed, fQ1Diff);

    if (fabs(fQ2Diff) < fMinArmSpeed)
        fQ2Step = 0.0f;
    else if (fabs(fQ2Diff) < fMaxArmSpeed)
        fQ2Step = copysignf(fMinArmSpeed,fQ2Diff);
    else
        fQ2Step = copysignf(fMaxArmSpeed,fQ2Diff);

    Move.fQ1Step = fQ1Step;
    Move.fQ2Step = fQ2Step;
    //*+++++++
    return Move;
}

void ClearArmMovesList()
{
    ArmGranularPositions.clear();
}