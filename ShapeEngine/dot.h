#pragma once
#include "shape.h"


class  CDot :
	public CShape
{
public:
	CDot(SHPHandle hShape);
	CDot();
	virtual ~CDot(void);
	
	//virtual void Render(CDC* pDC, CRect rcDest,double dfLTGeoX=0, double dfLTGeoY=0,bool bOptimize=true);
	virtual void SetRTData(void * lpVoid) {};
	virtual void Draw(CDC* pDC, CRect rcDest, double dfLTGeoX, double dfLTGeoY,bool bOptimize);
protected:

	
	
	COLORREF m_clrFill;
	int m_nStyle;
};
