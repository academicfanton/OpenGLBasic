#include <list>
#include "AngleMath.h"
#include "ModelArmPosition.h"
#include "ModelArmMoves.h"

using namespace std;

float fQ1Speed = 0.0f;
float fQ2Speed = 0.0f;
// Buffer with calculated model positions
std::list<sTwoArmPosition> ArmGranularPositions;

sArmsMove CalculateArmsMovement(const sTwoArmPosition StartPos, const sTwoArmPosition EndPos, float fLenArm1, float fLenArm2)
{
    sArmsMove Move;
    float fQ1Diff;
    float fQ2Diff;

    Move.fQ1StartPos = StartPos.fQ1;
    Move.fQ2StartPos = StartPos.fQ2;
    Move.fQ1EndPos = EndPos.fQ1;
    Move.fQ2EndPos = EndPos.fQ2;
    fQ1Diff = GetAngleDiff(StartPos.fQ1, EndPos.fQ1);
    fQ2Diff = GetAngleDiff(StartPos.fQ2, EndPos.fQ2);
    Move.fQ1SmallStep = copysignf(fMinArmSpeed, fQ1Diff);
    Move.fQ1LargeStep = copysignf(fMaxArmSpeed, fQ1Diff);
    Move.fQ2SmallStep = copysignf(fMinArmSpeed, fQ2Diff);
    Move.fQ2LargeStep = copysignf(fMaxArmSpeed, fQ2Diff);
    Move.bMoveCompleted = false;
    Move.bMoveCompleted1 = false;
    Move.bMoveCompleted2 = false;
    Move.fLenArm1 = fLenArm1;
    Move.fLenArm2 = fLenArm2;
    Move.CurrentPosition = StartPos;
    return Move;
}

void ClearArmMovesList()
{
    ArmGranularPositions.clear();
}

bool AdvanceArmsMovement(sArmsMove* pArmsMove)
{
    float fCurrentDistance, fSmallStepDistance, fLargeStepDistance;
    float fQSmall, fQLarge;
    bool bMoved = false;
    if (!pArmsMove->bMoveCompleted1)
    {
        fQLarge = pArmsMove->CurrentPosition.fQ1 + pArmsMove->fQ1LargeStep;
        fQSmall = pArmsMove->CurrentPosition.fQ1 + pArmsMove->fQ1SmallStep;
        fCurrentDistance = fabsf(GetAngleDiff(pArmsMove->CurrentPosition.fQ1,pArmsMove->fQ1EndPos));
        fSmallStepDistance = fabsf(GetAngleDiff(fQSmall,pArmsMove->fQ1EndPos));
        fLargeStepDistance = fabsf(GetAngleDiff(fQLarge,pArmsMove->fQ1EndPos));
        if (fCurrentDistance <= fSmallStepDistance && fCurrentDistance <= fLargeStepDistance)
        {
            pArmsMove->bMoveCompleted1 = true;
        }
        else
        {
            if (fSmallStepDistance <= fLargeStepDistance)
            {
                pArmsMove->CurrentPosition.fQ1 += pArmsMove->fQ1SmallStep;
            }
            else
            {
                pArmsMove->CurrentPosition.fQ1 += pArmsMove->fQ1LargeStep;
            }

        }
    }
    if (!pArmsMove->bMoveCompleted2)
    {
        fQLarge = pArmsMove->CurrentPosition.fQ2 + pArmsMove->fQ2LargeStep;
        fQSmall = pArmsMove->CurrentPosition.fQ2 + pArmsMove->fQ2SmallStep;
        fCurrentDistance = fabsf(GetAngleDiff(pArmsMove->CurrentPosition.fQ2, pArmsMove->fQ2EndPos));
        fSmallStepDistance = fabsf(GetAngleDiff(fQSmall, pArmsMove->fQ2EndPos));
        fLargeStepDistance = fabsf(GetAngleDiff(fQLarge, pArmsMove->fQ2EndPos));
        if (fCurrentDistance <= fSmallStepDistance && fCurrentDistance <= fLargeStepDistance)
        {
            pArmsMove->bMoveCompleted2 = true;
        }
        else
        {
            if (fSmallStepDistance <= fLargeStepDistance)
            {
                pArmsMove->CurrentPosition.fQ2 += pArmsMove->fQ2SmallStep;
            }
            else
            {
                pArmsMove->CurrentPosition.fQ2 += pArmsMove->fQ2LargeStep;
            }
        }
    }
    if (pArmsMove->bMoveCompleted1 && pArmsMove->bMoveCompleted2)
        pArmsMove->bMoveCompleted = true;
    UpdateTwoArmPositionFromQs(&pArmsMove->CurrentPosition,pArmsMove->fLenArm1,pArmsMove->fLenArm2);
    return !pArmsMove->bMoveCompleted;
}

