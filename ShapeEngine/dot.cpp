#include "pch.h"
#include ".\dot.h"


CDot::CDot(SHPHandle hShape)
{
	m_clrLine = RGB(255,0,255);
	m_clrFill = RGB(0,255,0);
	CreateFromHandle(hShape);
	
}

CDot::CDot()
{
	m_clrLine = RGB(255,0,255);
	m_clrFill = RGB(0,255,0);

}

CDot::~CDot(void)
{
}

//void CDot::Render(CDC* pDC, CRect rcDest,double dfLTGeoX, double dfLTGeoY,bool bOptimize) 
//{
//	Draw(pDC,rcDest,dfLTGeoX,dfLTGeoY, bOptimize);
//}

void CDot::Draw(CDC* pDC, CRect rcDest, double dfLTGeoX, double dfLTGeoY,bool bOptimize)
{
	//创建笔刷
	CPen linePen;
	linePen.CreatePen(PS_SOLID, 2, RGB(0,50,50));

	CPen* lpOldPen = pDC->SelectObject(&linePen);
	pDC->SelectStockObject(NULL_BRUSH);

	//调用基类draw
	CShape::Draw(pDC,rcDest,dfLTGeoX,dfLTGeoY, bOptimize);

	//开始画
	for(int i=0; i<m_nHitRec;i++)//对于所有击中的点
	{	
		SHPObject * oSHP = SHPReadObject(m_hShape,m_panHits[i]);//Get current object

		POINT pt;
		Geo2Screen(pt,oSHP,0,rcDest,dfLTGeoX,dfLTGeoY);	
	
		int nHalfSize = 2;
		
		pDC->Rectangle(pt.x - nHalfSize, pt.y - nHalfSize, pt.x + nHalfSize*2, pt.y + nHalfSize*2);//方框标注点位
	
		SHPDestroyObject(oSHP);
	}

	//清空击中的点
	if(m_panHits != NULL)
	{	
		delete[] m_panHits;
		m_panHits = NULL;
		m_nHitRec = -1;
	}

	pDC->SelectObject(lpOldPen);
}
