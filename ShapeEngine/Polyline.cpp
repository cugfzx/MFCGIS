#include "pch.h"
#include ".\polyline.h"

CPolyline::CPolyline(void)
{
	m_lpuchStatus = NULL;
	m_linePen.CreatePen(PS_SOLID, m_nPenSize, m_clrLine);	
}

CPolyline::CPolyline(SHPHandle hShape)
{
	CreateFromHandle(hShape);
	m_lpuchStatus = NULL;
	m_linePen.CreatePen(PS_SOLID, m_nPenSize, m_clrLine);
}

CPolyline::~CPolyline(void)
{
	if(m_lpuchStatus)
	{
		delete m_lpuchStatus;
		m_lpuchStatus = NULL;
	}
}

//void CPolyline::Render(CDC* pDC, CRect rcDest,double dfGeoOffsetX, double dfGeoOffsetY,bool bOptimize) 
//{
//	Draw(pDC,rcDest,dfGeoOffsetX,  dfGeoOffsetY,bOptimize);
//}

void CPolyline::Draw(CDC* pDC, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY,bool bOptimize)
{
	CPen *lpOldPen;
	lpOldPen = pDC->SelectObject(&m_linePen);
	
	//Base class draw
	CShape::Draw(pDC,rcDest, dfGeoOffsetX,  dfGeoOffsetY,bOptimize);

	//drawing
	if (!bOptimize)
	{	
		int nHitRec = m_hShape->nRecords;
		for(int i=0; i<nHitRec;i++)
		{	
			SHPObject * oSHP = SHPReadObject(m_hShape,i);//Get current object

		
			if(oSHP->nParts==1)//count of parts is 1
			{	
				for(int k=oSHP->panPartStart[0]; k<oSHP->nVertices-1; k++)
				{
					CPoint pt1,pt2;
					Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
					pDC->MoveTo(pt1);	

					Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);
				}			
			}
			else//count of parts is more than 1
			{
				for(int j=0; j<oSHP->nParts-1; j++)//多个part的record绘制
				{
					for(int k=oSHP->panPartStart[j]; k<oSHP->panPartStart[j+1]-1; k++)
					{
						CPoint pt1,pt2;
						Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);					
						pDC->MoveTo(pt1);
						Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);				
						pDC->LineTo(pt2);
					}

				}

				for(int k=oSHP->panPartStart[oSHP->nParts-1]; k<oSHP->nVertices-1; k++)
				{
					CPoint pt1,pt2;
					Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);				
					pDC->MoveTo(pt1);
					Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);
				}

			}

			SHPDestroyObject(oSHP);
		}
	}
	else
	{
		for(int i=0; i<m_nHitRec;i++)
		{	
			SHPObject * oSHP = SHPReadObject(m_hShape,m_panHits[i]);//Get current object

			if(oSHP->nParts==1)//count of parts is 1
			{	
				for(int k=oSHP->panPartStart[0]; k<oSHP->nVertices-1; k++)
				{
					CPoint pt1,pt2;
					Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
					pDC->MoveTo(pt1);	

					Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);
				}			
			}
			else//count of parts is more than 1
			{
				for(int j=0; j<oSHP->nParts-1; j++)//多个part的record绘制
				{
					for(int k=oSHP->panPartStart[j]; k<oSHP->panPartStart[j+1]-1-1; k++)
					{
						CPoint pt1,pt2;
						Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);					
						pDC->MoveTo(pt1);
						Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);				
						pDC->LineTo(pt2);
					}

				}

				for(int k=oSHP->panPartStart[oSHP->nParts-1]; k<oSHP->nVertices-1; k++)
				{
					CPoint pt1,pt2;
					Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);				
					pDC->MoveTo(pt1);
					Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);
				}

			}

			SHPDestroyObject(oSHP);
		}

		if(m_panHits)
		{	
			delete[] m_panHits;
			m_panHits = NULL;
			m_nHitRec = -1;
		}
	}
	

	pDC->SelectObject(lpOldPen);
}


int CPolyline::GetRoadColor(int nId)
{
	// Seed the random-number generator with current time so that
	// the numbers will be different every time we run.
	//
	//srand( (unsigned)time( NULL ) );
	//srand(100);

	// Usually, you will want to generate a number in a specific range,
	// such as 0 to 100, like this:
	if (m_lpuchStatus)
	{
		unsigned char speed =  *(unsigned char*)(m_lpuchStatus + 7 + nId * 2 + 1) ;
		speed = speed >>4;
		return (int)speed ;
	}
	else
	{
		int RANGE_MIN = 0;
		int RANGE_MAX = 100;
		int nSpeed = 0;
		nSpeed = (int)(rand()) % 100;

		if( nSpeed < 65 )
			nSpeed = 3;
		else if( nSpeed < 95 )
			nSpeed = 2;
		else
			nSpeed = 1;

		return nSpeed;

	}
}

void CPolyline::SetRTData(void *lpVoid)
{
	m_lpuchStatus = new unsigned char[m_hShape->nRecords * 2 + 7];
	memcpy(m_lpuchStatus, lpVoid , m_hShape->nRecords *  2 + 7);	
}
