#pragma once
#include "shape.h"


//Polyline3, PolylineM 23
class  CPolyline :
	public CShape
{
public:
	CPolyline();
	CPolyline(SHPHandle hShape);
	
	virtual ~CPolyline(void);

	//virtual void Render(CDC* pDC, CRect rcDest,double dfGeoOffsetX, double dfGeoOffsetY,bool bOptimize=true);
	virtual void SetRTData(void * lpVoid) ;
	virtual void Draw(CDC* pDC, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY,bool bOptimize);

protected:
	
	void DrawName(CDC* pDC, CRect rcDest, double dfLTGeoX, double dfLTGeoY,bool bOptimize);
	inline void SortVerticesF(double *pnVerticesRef,double *pnVertices,int nLen,bool bX);
	inline double GetLen(double* pdfVertices,int nCount);
	inline float GetTextPos(float fPosGiven,SHPObject* oSHP,bool bX);
	int GetRoadColor(int nId);

	unsigned char* m_lpuchStatus;

	CPen m_linePen;
};