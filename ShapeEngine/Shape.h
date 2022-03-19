//******************************************************//
//ESRI shape file I/O and render class for Windows Mobile
//Simon Zhang
//sheva2003@gmail.com
//Tongji University, Shanghai.
//*******************************************************//
#pragma once
#include "../shapeengine/shapelib/shapefil.h"
#include <vector>


	
class  CShape
{
public://interface

	//virtual void Render(CDC* pDC, CRect rcDest,double dfLTGeoX=0, double dfLTGeoY=0,bool bOptimize=true) = 0 ;
	virtual void Zoom(double dfZoom,CRect rcDest);
	virtual void Draw(CDC* pDC, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY,bool bOptimize=true);
	virtual void SetRTData(void * lpVoid) = 0;

public:
	
	virtual ~CShape(void);
	
	CString GetName(void){return m_lpszName;};
	void SetName(CString name) {m_lpszName = name;};

	//Load Data method
	const inline SHPHandle GetHandle() const {return m_hShape;} ;
	inline int GetShapeType() {return m_hShape->nShapeType;};
	
	//Render attribute method
	inline void SetLineColor(COLORREF clrLine) {m_clrLine = clrLine;};
	inline COLORREF GetLineColor() const {return m_clrLine;};
	inline void SetPenSize(COLORREF nPenSize) {m_nPenSize = nPenSize;};
	inline int GetPenSize() const {return m_nPenSize;};
	void SetTextSize(int nTextSize);

	//Geo information method
	const inline CRect GetRcSource() const {return m_rcGeoRange;};
	inline void SetRcSource(CRect rcSource) {m_rcGeoRange = rcSource;};
	
	//render zoom factor
	inline void SetZoomFactor(double dfZoom) {m_dfZoom = dfZoom;}; 
	inline double GetZoomFactor() const {return m_dfZoom;}; 
	
	//Render method
	void ComputeRangeHit(double pdfRangeHit[4], CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY);
	
	//count of the object
	inline int GetObjCount(void) {return m_hShape->nRecords;};

	// visible
	bool IsVisible(int nLvl) {return nLvl >= m_nLvlBottom && nLvl <= m_nLvlTop;};
	void SetVsblLvlRgn(int nBottom, int nTop) {m_nLvlBottom = nBottom; m_nLvlTop = nTop;};

	void LoadXBase(char* pchDbfPath);

protected:
	
	CShape(void);

	void CreateFromHandle(SHPHandle hShape);
	void Geo2Screen(POINT &point, SHPObject *oSHP, int nPtID, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY); 
	void Geo2Screen(POINT &point, double x, double y, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY);	
protected:	
	
	//Data member
	SHPHandle m_hShape;
	DBFHandle m_hXBase;
	SHPTree * m_pShpTree;

	//Geo information attributes
	CRect m_rcGeoRange;
	
	//zoom factor
	double m_dfZoom;

	//render attributes
	COLORREF m_clrLine;
	COLORREF m_clrText;
	int  m_nPenSize;
	int  m_nTextSize;
	CFont m_font;
	
	//lb zoom factor
	double m_dfLBZoom;

	//当前需要绘制的目标索引和目标数量
	int	*m_panHits, m_nHitRec;
	
	//当前图形层显示的级别范围
	int m_nLvlBottom,m_nLvlTop;

	bool m_bType;//true-LB, false-gauss

	CString m_lpszName;
};
