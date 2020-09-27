#pragma once
#ifndef _MODELPOSITION_H_
#define _MODELPOSITION_H_

// Position for all models: X,Y, time 
struct sPosition
{
    int iPosX;
    int iPosY;
    unsigned int uTime;
};

void ClearPositionList();
void InsertPositionToList(const sPosition NewPos);
unsigned int GetPositionListMaxSize();
void RenderPositionsPoints(float fRMax, float fGMax, float fBMax, unsigned int mwindW, unsigned int mwindH);

#endif