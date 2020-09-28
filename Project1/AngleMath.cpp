#include <math.h>
#include "AngleMath.h"

float GetAngleDiff(const float fAngle1, const float fAngle2)
{
    // a = targetA - sourceA
    // a -= 360 if a > 180
    // a += 360 if a < -180
    float fDiff;
    fDiff = fAngle2 - fAngle1;
    if (fDiff > PI)
        fDiff -= 2.0f * PI;
    if (fDiff < -PI)
        fDiff += 2.0f * PI;
    return fDiff;
    // Examples: Source:   10, Target:   30 - Diff=  20       , Sign +
    // Examples: Source:   10, Target:  170 - Diff= 160       , Sign +
    // Examples: Source:   10, Target: -170 - Diff=-180       , Sign +
    // Examples: Source:   10, Target: -171 - Diff=-181 -> 179, Sign +
    // Examples: Source: -170, Target: -150 - Diff=  20       , Sign +
    // Examples: Source: -170, Target:  -10 - Diff= 160       , Sign +
    // Examples: Source: -170, Target:   10 - Diff= 180       , Sign +
    // Examples: Source: -170, Target:   11 - Diff= 181 ->-179, Sign -
}

float GetAngleNormalized(const float fAngle)
{
    int iRounds = (int)floor(fAngle / (2.0f * PI));
    float fVal;
    fVal = fAngle - iRounds * 2.0f * PI;
    if (fVal > PI)
        fVal -= 2.0f * PI;
    if (fVal < -PI)
        fVal += 2.0f * PI;
    return fVal;
}
