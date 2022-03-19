#include "pch.h"
#include ".\polygon.h"



CPolygon::CPolygon(SHPHandle hShape)
{
	CreateFromHandle(hShape);
	m_linePen.CreatePen(PS_SOLID, m_nPenSize, m_clrLine);
}

CPolygon::CPolygon(void)
{
	m_clrFill = RGB(0,200,200);
}

CPolygon::~CPolygon(void)
{ 
	
}

void CPolygon::Draw(CDC* pDC, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY,bool bOptimize)
{
	CPen *lpOldPen;
	lpOldPen = pDC->SelectObject(&m_linePen);

	//Base class draw
	CShape::Draw(pDC,rcDest, dfGeoOffsetX,  dfGeoOffsetY,bOptimize);

	//drawing
	CPoint pt1,pt2;//pt1是线段起点，pt2是线段终点

	if (!bOptimize)//未优化版本
	{	
		int nHitRec = m_hShape->nRecords;
		for(int i=0; i<nHitRec;i++)
		{	
			SHPObject * oSHP = SHPReadObject(m_hShape,i);//Get current object

			if(oSHP->nParts==1)//如果只有一个多边形
			{	
				for(int k=oSHP->panPartStart[0]; k<oSHP->nVertices-1; k++)
				{
					
					Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
					pDC->MoveTo(pt1);	

					Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);
				}

				//连接首尾
				Geo2Screen(pt1,oSHP,oSHP->nVertices-1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
				pDC->MoveTo(pt1);	

				Geo2Screen(pt2,oSHP,oSHP->panPartStart[0],rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
				pDC->LineTo(pt2);

			}
			else//count of parts is more than 1
			{
				for(int j=0; j<oSHP->nParts-1; j++)//多个part的record绘制
				{
					for(int k=oSHP->panPartStart[j]; k<oSHP->panPartStart[j+1]-1; k++)
					{
				
						Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);					
						pDC->MoveTo(pt1);
						Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);				
						pDC->LineTo(pt2);
					}

		
					Geo2Screen(pt1,oSHP,oSHP->panPartStart[j+1]-1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
					pDC->MoveTo(pt1);	

					Geo2Screen(pt2,oSHP,oSHP->panPartStart[j],rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);

				}

				//画最后一个
				for(int k=oSHP->panPartStart[oSHP->nParts-1]; k<oSHP->nVertices-1; k++)
				{
	
					Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);				
					pDC->MoveTo(pt1);
					Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);
				}


				Geo2Screen(pt1,oSHP,oSHP->nVertices-1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
				pDC->MoveTo(pt1);	

				Geo2Screen(pt2,oSHP,oSHP->panPartStart[oSHP->nParts-1],rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
				pDC->LineTo(pt2);

			}

			SHPDestroyObject(oSHP);
		}
	}
	else//优化版本
	{
		for(int i=0; i<m_nHitRec;i++)
		{	
			SHPObject * oSHP = SHPReadObject(m_hShape,m_panHits[i]);//Get current object

			if(oSHP->nParts==1)//count of parts is 1
			{	
				for(int k=oSHP->panPartStart[0]; k<oSHP->nVertices-1; k++)
				{
			
					Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
					pDC->MoveTo(pt1);	

					Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);
				}		
	
				Geo2Screen(pt1,oSHP,oSHP->nVertices-1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
				pDC->MoveTo(pt1);	

				Geo2Screen(pt2,oSHP,oSHP->panPartStart[0],rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
				pDC->LineTo(pt2);
			}
			else//count of parts is more than 1
			{
				for(int j=0; j<oSHP->nParts-1; j++)//多个part的record绘制
				{
					for(int k=oSHP->panPartStart[j]; k<oSHP->panPartStart[j+1]-1-1; k++)
					{
						
						Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);					
						pDC->MoveTo(pt1);
						Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);				
						pDC->LineTo(pt2);
					}
					Geo2Screen(pt1,oSHP,oSHP->panPartStart[j+1]-1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
					pDC->MoveTo(pt1);	

					Geo2Screen(pt2,oSHP,oSHP->panPartStart[j],rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);

				}

				for(int k=oSHP->panPartStart[oSHP->nParts-1]; k<oSHP->nVertices-1; k++)
				{
	
					Geo2Screen(pt1,oSHP,k,rcDest,dfGeoOffsetX,  dfGeoOffsetY);				
					pDC->MoveTo(pt1);
					Geo2Screen(pt2,oSHP,k+1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
					pDC->LineTo(pt2);
				}
		
				Geo2Screen(pt1,oSHP,oSHP->nVertices-1,rcDest,dfGeoOffsetX,  dfGeoOffsetY);
				pDC->MoveTo(pt1);	

				Geo2Screen(pt2,oSHP,oSHP->panPartStart[oSHP->nParts-1],rcDest,dfGeoOffsetX,  dfGeoOffsetY);			
				pDC->LineTo(pt2);

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


//void CPolygon::Render(CDC* pDC, CRect rcDest,double dfLTGeoX, double dfLTGeoY,bool bOptimize) 
//{	
//	Draw(pDC,rcDest,dfLTGeoX,  dfLTGeoY,bOptimize);
//}




