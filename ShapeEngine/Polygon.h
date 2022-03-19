#pragma once
#include "shape.h"


class  CPolygon :
	public CShape
{
public:
	
	CPolygon(SHPHandle hShape);
	CPolygon();
	virtual ~CPolygon(void);
	//virtual void Render(CDC* pDC, CRect rcDest,double dfLTGeoX=0, double dfLTGeoY=0,bool bOptimize=true);
	virtual void SetRTData(void * lpVoid) {};
	virtual void Draw(CDC* pDC, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY,bool bOptimize);
		
protected:
	
	COLORREF m_clrFill;
	CPen m_linePen;
};

