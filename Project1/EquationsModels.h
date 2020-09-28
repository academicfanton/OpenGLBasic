#pragma once
#ifndef _EQUATIONSMODELS_H_
#define _EQUATIONSMODELS_H_

#include "ModelPosition.h"
#include "EquationsModel_1.h"
#include "EquationsModel_2.h"
#include "EquationsModel_3.h"

// Function pointer type for position calculator
typedef sPosition(*pGetPoint_Model_Function)(const unsigned int uTime, const float fSpeedTheta, const float fRadius);


// Models factory - old C style constant array based.
// It could be better done with a class and a singleton for the factory as well as for the models,
// so they could autoregister. Preferred to write in C so it can go into the Arduino.
const pGetPoint_Model_Function Models[] =
{
	GetPoint_Model1,
	GetPoint_Model2,
	GetPoint_Model3
};
const unsigned int ModelsCount = sizeof(Models) / sizeof(Models[0]);


#endif