#include "pch.h"
#include ".\shape.h"


CShape::CShape(void)
:m_rcGeoRange(CRect(0,0,0,0)),
m_clrText(RGB(200,200,200)),
m_nPenSize(1),
m_dfZoom(0.5),
m_hShape(NULL),
m_hXBase(NULL),
m_dfLBZoom(1.0),
m_panHits(NULL),
m_nHitRec(-1),
m_nTextSize(15),
m_nLvlBottom(-1),
m_nLvlTop(-1),
m_lpszName(L"未命名")
{
	VERIFY(m_font.CreateFont(
		m_nTextSize,               // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"微软雅黑"));                // lpszFacename

	srand( (unsigned)time( NULL ) );

	int RANGE_MIN = 0;
	int RANGE_MAX = 200;
	int color[3];
	color[0] = (int)((double) rand() / (double) RAND_MAX * RANGE_MAX + RANGE_MIN);
	color[1] = (int)((double) rand() / (double) RAND_MAX * RANGE_MAX + RANGE_MIN);
	color[2] = (int)((double) rand() / (double) RAND_MAX * RANGE_MAX + RANGE_MIN);
	
	/*int max,min;
	max = min = 0;
	for (int i=1; i<3; i++)
	{
		if (color[max] < color[i])
		{
			max = i;
		}
		if (color[min] > color[i])
		{
			min = i;
		}

	}

	color[max] += 100;
	if (color[max] > 255)
	{
		color[max] = 255;
	}
	color[min] -= 100;
	if (color[min] < 0)
	{
		color[min] = 0;
	}*/

	m_clrLine = RGB(color[0],color[1],color[2]);
}

CShape::~CShape(void)
{
	if(m_pShpTree)
		SHPDestroyTree(m_pShpTree);
	
	if(m_hShape)
	{
		SHPClose(m_hShape);
		m_hShape = NULL;
	}

	if (m_hXBase)
	{
		DBFClose(m_hXBase);
		m_hXBase = NULL;
	}

	if(m_panHits != NULL)
	{	
		delete[] m_panHits;
		m_panHits = NULL;
		m_nHitRec = -1;
	}

	m_font.DeleteObject(); 
}

//==========================================================================//
//计算当前需要绘制的地理范围区域
//pdfRangeHit	返回值，得到的需要绘制的地理范围
//rcDest		当前绘图的目标区
//==========================================================================//
void CShape::ComputeRangeHit( double pdfRangeHit[4], CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY)
{
	//left
	pdfRangeHit[0] = dfGeoOffsetX;
	//top
	pdfRangeHit[1] = dfGeoOffsetY;
	//right
	pdfRangeHit[2] = pdfRangeHit[0] + rcDest.Width() / m_dfZoom;
	//bottom
	pdfRangeHit[3] = pdfRangeHit[1] + rcDest.Height() / m_dfZoom;

	for (int i=0; i<4; i++)
	{
		pdfRangeHit[i] /= m_dfLBZoom;
	}
}

void CShape::Geo2Screen(POINT &point,SHPObject* oSHP,int nPtID, CRect rcDest,double dfGeoOffsetX, double dfGeoOffsetY)
{
	point.x = (long)((oSHP->padfX[nPtID] * m_dfLBZoom - dfGeoOffsetX) * m_dfZoom);
	point.y = (long)((oSHP->padfY[nPtID] * m_dfLBZoom - dfGeoOffsetY) * m_dfZoom);

	point.x += rcDest.left;
	point.y = rcDest.bottom - point.y ;
}

void CShape::Geo2Screen(POINT &point, double x, double y, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY)
{
	point.x = (long)((x * m_dfLBZoom - dfGeoOffsetX) * m_dfZoom);
	point.y = (long)((y * m_dfLBZoom - dfGeoOffsetY) * m_dfZoom);

	point.x += rcDest.left;
	point.y = rcDest.bottom - point.y;
}

void CShape::Zoom(double dfZoom,CRect rcDest)
{
	m_dfZoom = dfZoom;
}

//Draw() 不进行实际的绘图工作，只是获取击中的目标索引值，在不同的子类中决定绘制的具体行为
void CShape::Draw(CDC* pDC, CRect rcDest, double dfGeoOffsetX, double dfGeoOffsetY,bool bOptimize)
{
	if(!m_hShape ) 	
		return;

	if (!bOptimize)
	{
		return;
	}
	else
	{
		//Compute geo range Hit
		double pdfRangeHit[4];
		ComputeRangeHit(pdfRangeHit, rcDest, dfGeoOffsetX, dfGeoOffsetY);

		//Select the record in the range of rcDevice 
		double padfBoundsMin[4];
		double padfBoundsMax[4];

		padfBoundsMin[0] = pdfRangeHit[0];//left
		padfBoundsMin[1] = pdfRangeHit[1];//top
		padfBoundsMax[0] = pdfRangeHit[2];//right
		padfBoundsMax[1] = pdfRangeHit[3];//bottom

		m_panHits = SHPTreeFindLikelyShapes(m_pShpTree, padfBoundsMin, padfBoundsMax,&m_nHitRec );
	}
}

void CShape::CreateFromHandle(SHPHandle hShape)
{
	m_hShape = hShape;
	if(!m_hShape)
		return ;

	long width = (long)(m_hShape->adBoundsMax[0] - m_hShape->adBoundsMin[0]);
	if (width < 10)
		m_dfLBZoom = 1000;
	
	m_rcGeoRange.left = (long)(m_hShape->adBoundsMin[0] * m_dfLBZoom);
	m_rcGeoRange.top = (long)(m_hShape->adBoundsMin[1] * m_dfLBZoom);
	m_rcGeoRange.right = (long)(m_hShape->adBoundsMax[0] * m_dfLBZoom);
	m_rcGeoRange.bottom = (long)(m_hShape->adBoundsMax[1] * m_dfLBZoom);

	m_pShpTree = SHPCreateTree( m_hShape, 2,10, NULL, NULL );
	SHPTreeTrimExtraNodes( m_pShpTree );
}

void CShape::LoadXBase(char* pchDbfPath)
{
	m_hXBase = DBFOpen(pchDbfPath,"rb");
}

void CShape::SetTextSize(int nTextSize)
{
	m_nTextSize = nTextSize;
	m_font.DeleteObject();

	VERIFY(m_font.CreateFont(
		m_nTextSize,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"黑体"));                // lpszFacename
}


