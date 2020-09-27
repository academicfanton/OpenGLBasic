#pragma once
#ifndef _EQUATIONS_MODEL_3_H_
#define _EQUATIONS_MODEL_3_H_

#include "ModelPosition.h"
#include <math.h>

#undef PI               
#define PI 3.141592657f

// -- Model variables -- 
// Drawing type 3: Lissajous curve using cartesian coordinates
//       X = sin(fLissajousP*t)
//       Y = sin (fLissajousQ*t+fLissajousDelta)
const float fLissajousA_Model3 = 1.0f;
const float fLissajousP_Model3 = 3.0f;
const float fLissajousR_Model3 = PI;  // /2
const float fLissajousQ_Model3 = 7.0f;
const float fLissajousDelta_Model3 = PI / 2;

sPosition GetPoint_Model3(const unsigned int uTime, const float fSpeedTheta, const float fRadius)
{
    sPosition NewPos;
    // Creamos nuevo punto - Lissajous en cartesianas -  x = sin(p*t) y = sin (q*t+d)
    NewPos.iPosX = (int)(fRadius * sinf(fLissajousP_Model3 * uTime * fSpeedTheta));
    NewPos.iPosY = (int)(fRadius * sinf(fLissajousQ_Model3 * uTime * fSpeedTheta + fLissajousDelta_Model3));
    NewPos.uTime = uTime;
    return NewPos;
}

#endif