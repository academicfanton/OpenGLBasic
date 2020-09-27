#pragma once
#ifndef _EQUATIONS_MODEL_1_H_
#define _EQUATIONS_MODEL_1_H_

#include "ModelPosition.h"
#include <math.h>

// Model 1
//  .. 8 leafs rose using a simple polar formula
//       Theta = t
//       R = sin(fRadiusSpeed*Theta)
const float fRadiusSpeed_Model1 = 4.0f;

sPosition GetPoint_Model1(const unsigned int uTime, const float fSpeedTheta, const float fRadius)
{
    sPosition NewPos;
    float fRad;
    float fTheta;
    // Creamos nuevo punto - rosa de 5 petalos - en polares - R=sin(4*t), O=t
    fTheta = uTime * fSpeedTheta;
    fRad = sinf(fRadiusSpeed_Model1 * fTheta);
    // Convertimos de polares a XY
    NewPos.iPosX = (int)(fRadius * fRad * cosf(fTheta));
    NewPos.iPosY = (int)(fRadius * fRad * sinf(fTheta));
    NewPos.uTime = uTime;
    return NewPos;
}


#endif