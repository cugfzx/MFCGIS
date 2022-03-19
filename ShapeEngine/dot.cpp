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
	//������ˢ
	CPen linePen;
	linePen.CreatePen(PS_SOLID, 2, RGB(0,50,50));

	CPen* lpOldPen = pDC->SelectObject(&linePen);
	pDC->SelectStockObject(NULL_BRUSH);

	//���û���draw
	CShape::Draw(pDC,rcDest,dfLTGeoX,dfLTGeoY, bOptimize);

	//��ʼ��
	for(int i=0; i<m_nHitRec;i++)//�������л��еĵ�
	{	
		SHPObject * oSHP = SHPReadObject(m_hShape,m_panHits[i]);//Get current object

		POINT pt;
		Geo2Screen(pt,oSHP,0,rcDest,dfLTGeoX,dfLTGeoY);	
	
		int nHalfSize = 2;
		
		pDC->Rectangle(pt.x - nHalfSize, pt.y - nHalfSize, pt.x + nHalfSize*2, pt.y + nHalfSize*2);//�����ע��λ
	
		SHPDestroyObject(oSHP);
	}

	//��ջ��еĵ�
	if(m_panHits != NULL)
	{	
		delete[] m_panHits;
		m_panHits = NULL;
		m_nHitRec = -1;
	}

	pDC->SelectObject(lpOldPen);
}
