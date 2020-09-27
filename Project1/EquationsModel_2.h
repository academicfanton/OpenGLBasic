#pragma once
#ifndef _EQUATIONS_MODEL_2_H_
#define _EQUATIONS_MODEL_2_H_

#include "ModelPosition.h"
#include <math.h>

#undef PI               
#define PI 3.141592657f

// -- Model variables -- 
// Drawing type 2: Lissajous curve using polar coordinates
//       R = fLissajousA*sin(fLissajousP*t), 
//       Theta = fLissajousR*sin(fLissajousQ*t+fLissajousDelta)
const float fLissajousA_Model2 = 1.0f;
const float fLissajousP_Model2 = 3.0f;
const float fLissajousR_Model2 = PI;  // /2
const float fLissajousQ_Model2 = 7.0f;
const float fLissajousDelta_Model2 = PI / 2;

sPosition GetPoint_Model2(const unsigned int uTime, const float fSpeedTheta, const float fRadius)
{
    sPosition NewPos;
    float fRad;
    float fTheta;
    // Creamos nuevo punto - Lissajous en polares -  R = A*sin(p*t), O = R*sin (q*t+d)
    fRad = fLissajousA_Model2 * sinf(fLissajousP_Model2 * uTime * fSpeedTheta);
    fTheta = fLissajousR_Model2 * sinf(fLissajousQ_Model2 * uTime * fSpeedTheta + fLissajousDelta_Model2);
    // Convertimos de polares a XY
    NewPos.iPosX = (int)(fRadius * fRad * cosf(fTheta));
    NewPos.iPosY = (int)(fRadius * fRad * sinf(fTheta));
    NewPos.uTime = uTime;
    return NewPos;

}

#endif