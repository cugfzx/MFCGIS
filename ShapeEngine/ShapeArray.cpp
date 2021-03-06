#include "pch.h"
#include "ShapeArray.h"

int CShapeArray::count = 0;

CShapeArray::CShapeArray():
m_rcDest(CRect(0,0,0,0)),
m_dfGeoOffsetX(0),
m_dfGeoOffsetY(0),
m_bRedrawAll(true),
m_BkgBrush(RGB(255,255,255)),
m_nScreenStepX(20),
m_nScreenStepY(20),
m_bInit(false),
m_dfZoom(1.0),
id(count++)
{
	m_pShapeArray.clear();
}

CShapeArray::~CShapeArray(void)
{
	if(!m_pShapeArray.empty())
	{
		unsigned int i = 0;
		unsigned int n = m_pShapeArray.size();
		for(i=0; i< n; i++)
			delete m_pShapeArray[i];

		m_pShapeArray.clear();
	}

	if(IsInited())
	{
		m_pDCMain->DeleteDC();
		m_pDCPatch->DeleteDC();
		m_pDCMove->DeleteDC();

		if (m_pDCMain)
		{
			delete m_pDCMain;
			m_pDCMain = NULL;
		}
		if (m_pDCPatch)
		{
			delete m_pDCPatch;
			m_pDCPatch = NULL;
		}
		if (m_pDCMove)
		{
			delete m_pDCMove;
			m_pDCMove = NULL;
		}

	}
	
}

void CShapeArray::Initialize(CDC *pDC)
{
	//Main memory dc
	m_pDCMain = new CDC;
	m_pDCMain->CreateCompatibleDC(pDC);

	m_BmpMain.CreateCompatibleBitmap(pDC, m_rcDest.Width(),m_rcDest.Height());
	m_pDCMain->SelectObject(&m_BmpMain);

	//vertical patch memory dc
	m_pDCPatch = new CDC;
	m_pDCPatch->CreateCompatibleDC(pDC);

	m_BmpPatch.CreateCompatibleBitmap(pDC, m_rcDest.Width(), m_rcDest.Height());
	m_pDCPatch->SelectObject(&m_BmpPatch);

	//horizonal patch memory dc
	m_pDCMove = new CDC;
	m_pDCMove->CreateCompatibleDC(pDC);

	m_BmpMove.CreateCompatibleBitmap(pDC, m_rcDest.Width(),m_rcDest.Height());
	m_pDCMove->SelectObject(&m_BmpMove);

	m_bInit = true;
}

void CShapeArray::FlushBkg(CDC* pDC)
{
	pDC->FillRect(m_rcDest,&m_BkgBrush);
}

void CShapeArray::AddShape(const char* path)
{

	CShapeFactory* pFactory = new CESRIFactory;
	
	CShape* pShape = pFactory->CreateShape(path);

	if( pShape )
	{
		unsigned int i = 0;
		unsigned int n = m_pShapeArray.size();
		for (i = 0; i < n; i++)
		{
			if (m_pShapeArray[i]->GetName() == pShape->GetName())
			{
				delete pFactory;
				delete pShape;
				return;
			}
		}

		bool bAutoZoom = m_pShapeArray.empty();

		m_pShapeArray.push_back(pShape);
		
		pShape->SetZoomFactor(m_dfZoom);
		
		if (bAutoZoom)
		{
			AutoZoom(pShape->GetName());
		}
		
		//itr = m_pShapeArray.begin();
		//m_dfGeoOffsetX = (*itr)->GetRcSource().left + 2000 / m_dfZoom;
		//m_dfGeoOffsetY = (*itr)->GetRcSource().top - 100 / m_dfZoom;

		m_bRedrawAll = true;
	}
	delete pFactory;
}

void CShapeArray::AddShape(CShape* pShape)
{
	if( pShape )
	{	
		unsigned int i = 0;
		unsigned int n = m_pShapeArray.size();
		for (i = 0; i < n; i++)
		{
			if (m_pShapeArray[i]->GetName() == pShape->GetName())
			{
				delete pShape;
				return;
			}
		}

		m_pShapeArray.push_back(pShape);
		pShape->SetZoomFactor(m_dfZoom);
//		AutoZoom(pShape->GetName());
		m_bRedrawAll = true;
	}
}	

//**************************************************************************//
//Old mouse pos x > New mouse pos x: nOffsetX > 0 , move dirction   "<----"	//
//																			//
//Old mouse pos x < New mouse pos x: nOffsetX < 0 , move dirction   "---->"	//
//																			//
//Old mouse pos y > New mouse pos y: nOffsetY > 0 , move dirction   "/|\"	//
//																	  |		//
//																			//
//Old mouse pos y < New mouse pos y: nOffsetY < 0 , move dirction   " |"	//
//																	 \|/	//
//**************************************************************************//
void CShapeArray::RenderMove(CDC* pDC,int nOffsetX, int nOffsetY, bool bReDraw)
{
	int nStatus;//状态变量

	//******************************************************************************************************************//
	//绘图过程分为两个部分，移动后的目标DC区域分为新视图(阴影部分)，横向补丁和纵向补丁。								//
	//新视图为旧的DC拖动后遗留在当前目标DC上的部分，该部分不必重绘，先BitBlt到Move DC。									//
	//然后BitBlt到目标DC即可。横向补丁和纵向补丁为拖动后进入当前目标DC的新的区域，该区域								//
	//需要重绘。新视图和两个补丁的分布有四中情况，如下图所示。															//
	//																													//
	//	1: nOffsetX>0; nOffsetY>0.	2: nOffsetX<0; nOffsetY<0. 3: nOffsetX>0; nOffsetY<0.  4: nOffsetX<0; nOffsetY>0	//
	//	________________________	________________________	________________________	________________________	//
	//	|///////////////|		|	|		|横向补丁		|	| 横向补丁		|		|	|		|///////////////|	//
	//	|// 新视图//////|纵向	|	| 纵向	|_______________|	|_______________|纵向	|	| 纵向 	|///新视图//////|	//
	//	|///////////////|		|	|		|///////////////|	|///////////////|补丁	|	| 补丁	|///////////////|	//
	//	|///////////////|补丁	|	| 补丁	|///////////////|	|///////////////|		|	|		|///////////////|	//
	//	| 横向补丁		|		|	|		|/// 新视图/////|	|///新视图//////|		|	|		|	横向补丁	|	//
	//	|_______________|_______|	|_______|///////////////|	|///////////////|_______|	|_______|_______________|	//
	//																													//
	//******************************************************************************************************************//

	////////状态设定//////
	if (nOffsetX >=0 &&  nOffsetY >= 0)
		nStatus = 0;

	if (nOffsetX <=0 &&  nOffsetY <= 0)
		nStatus = 1;

	if (nOffsetX >=0 &&  nOffsetY <= 0)
		nStatus = 2;

	if (nOffsetX <=0 &&  nOffsetY >= 0)
		nStatus = 3;
	
	////////绘制目标DC/////////

	if (!bReDraw)
	{//鼠标左键未放开，只绘制新视图，不绘制补丁，Main DC不进行更新
		
		//绘制Move DC背景
		m_pDCMove->FillRect(m_rcDest, & m_BkgBrush);

		//从Main DC 截取相应内容并复制到Move DC的新视图区域
		m_pDCMove->BitBlt(0,0,m_rcDest.Width(),m_rcDest.Height(),m_pDCMain,nOffsetX,nOffsetY,SRCCOPY);
		
		//复制全部Move DC到目标DC
		pDC->BitBlt(0,0,m_rcDest.Width(),m_rcDest.Height(),m_pDCMove,0,0,SRCCOPY);
	}
	else
	{//鼠标左键松开，绘制补丁，Main DC进行更新
		
		//将Move DC 复制到Main DC
		m_pDCMain->BitBlt(0,0,m_rcDest.Width(),m_rcDest.Height(),m_pDCMove,0,0,SRCCOPY);

		//绘制补丁只需提供补丁地理偏移量（偏移量为补丁区域左下角地理坐标）和补丁尺寸。

		CPoint ptPatch;	//当前绘制补丁的左上角点
		CSize szPatch;	//当前绘制补丁的尺寸
		CRect rcPatch;	//当前绘制补丁的区域
		
		double dfPatchGeoOffsetX;	//当前绘制补丁对应的地理偏移量
		double dfPatchGeoOffsetY;

		//////////绘制纵向补丁//////////
		szPatch = CSize(abs(nOffsetX), m_rcDest.Height());

		switch(nStatus)
		{
		case 0:	
			ptPatch.x = m_rcDest.Width() - abs(nOffsetX);
			ptPatch.y = 0;
			dfPatchGeoOffsetX = m_dfGeoOffsetX + m_rcDest.Width() / m_dfZoom; 
			dfPatchGeoOffsetY = m_dfGeoOffsetY - abs(nOffsetY) / m_dfZoom; 
			break;
			
		case 1:
			dfPatchGeoOffsetX = m_dfGeoOffsetX - abs(nOffsetX) / m_dfZoom; 
			dfPatchGeoOffsetY = m_dfGeoOffsetY + abs(nOffsetY) / m_dfZoom; 
			ptPatch.x = 0;
			ptPatch.y = 0;
			break;
			
		case 2:	
			dfPatchGeoOffsetX = m_dfGeoOffsetX + m_rcDest.Width() / m_dfZoom; 
			dfPatchGeoOffsetY = m_dfGeoOffsetY + abs(nOffsetY) / m_dfZoom; 
			ptPatch.x = m_rcDest.Width() - abs(nOffsetX);
			ptPatch.y = 0;
			break;
	
		case 3:
			dfPatchGeoOffsetX = m_dfGeoOffsetX - abs(nOffsetX) / m_dfZoom; 
			dfPatchGeoOffsetY = m_dfGeoOffsetY - abs(nOffsetY) / m_dfZoom; 
			ptPatch.x = 0;
			ptPatch.y = 0;
			break;	
		}

		rcPatch = CRect(ptPatch,szPatch);
		
		PastePatch(rcPatch, m_pDCPatch, pDC, dfPatchGeoOffsetX, dfPatchGeoOffsetY);

		//////////绘制横向补丁//////////
		szPatch = CSize(m_rcDest.Width() - abs(nOffsetX), abs(nOffsetY));

		switch(nStatus)
		{
		case 0:	
			ptPatch.x = 0;
			ptPatch.y = m_rcDest.Height() - abs(nOffsetY);
			dfPatchGeoOffsetX = m_dfGeoOffsetX + abs(nOffsetX) / m_dfZoom; 
			dfPatchGeoOffsetY = m_dfGeoOffsetY - abs(nOffsetY) / m_dfZoom; 
			break;

		case 1:
			dfPatchGeoOffsetX = m_dfGeoOffsetX ; 
			dfPatchGeoOffsetY = m_dfGeoOffsetY + m_rcDest.Height() / m_dfZoom; 
			ptPatch.x = abs(nOffsetX);
			ptPatch.y = 0;

			break;

		case 2:	
			dfPatchGeoOffsetX = m_dfGeoOffsetX + abs(nOffsetX) / m_dfZoom; 
			dfPatchGeoOffsetY = m_dfGeoOffsetY + m_rcDest.Height() / m_dfZoom; 
			ptPatch.x = 0;
			ptPatch.y = 0;
			break;

		case 3:
			dfPatchGeoOffsetX = m_dfGeoOffsetX; 
			dfPatchGeoOffsetY = m_dfGeoOffsetY - abs(nOffsetY) / m_dfZoom; 
			ptPatch.x = abs(nOffsetX);
			ptPatch.y = m_rcDest.Height() - abs(nOffsetY);
			break;	
		}

		rcPatch = CRect(ptPatch,szPatch);

		PastePatch(rcPatch, m_pDCPatch, pDC, dfPatchGeoOffsetX, dfPatchGeoOffsetY);
		
		/////////////更新偏移量/////////////////
		switch(nStatus)
		{
		case 0:	
			m_dfGeoOffsetX += abs(nOffsetX) / m_dfZoom; 
			m_dfGeoOffsetY -= abs(nOffsetY) / m_dfZoom; 
			break;

		case 1:
			m_dfGeoOffsetX -= abs(nOffsetX) / m_dfZoom; 
			m_dfGeoOffsetY += abs(nOffsetY) / m_dfZoom; 
			break;

		case 2:	
			m_dfGeoOffsetX += abs(nOffsetX) / m_dfZoom; 
			m_dfGeoOffsetY += abs(nOffsetY) / m_dfZoom; 
			break;

		case 3:
			m_dfGeoOffsetX -= abs(nOffsetX) / m_dfZoom; 
			m_dfGeoOffsetY -= abs(nOffsetY) / m_dfZoom; 
			break;	
		}

		////////////更新目标DC///////////////////
		pDC->BitBlt(0, 0, m_rcDest.Width(), m_rcDest.Height(), m_pDCMain, 0,0,SRCCOPY);	
	}
}

void CShapeArray::PastePatch(CRect rcPatch, CDC* pDCPatch, CDC* pDstDC,double dfOffsetX, double dfOffsetY)
{
	m_BmpPatch.DeleteObject();
	m_BmpPatch.CreateCompatibleBitmap(pDstDC, rcPatch.Width(), rcPatch.Height());
	m_pDCPatch->SelectObject(&m_BmpPatch);

	m_pDCPatch->FillRect(CRect(CPoint(0,0),CSize(rcPatch.Width(),rcPatch.Height())),&m_BkgBrush);

	unsigned int i = 0;
	unsigned int n = m_pShapeArray.size();
	for (i = 0; i < n; i++)
	{
		m_pShapeArray[i]->Draw(m_pDCPatch, CRect(CPoint(0,0),CSize(rcPatch.Width(),rcPatch.Height())), dfOffsetX, dfOffsetY) ;
	}

	m_pDCMain->BitBlt(rcPatch.left, rcPatch.top, rcPatch.Width(), rcPatch.Height(),m_pDCPatch, 0, 0,SRCCOPY);
}

void CShapeArray::Render(CDC* pDC)
{

	if(!m_pShapeArray.empty())
	{
		if (m_bRedrawAll)
		{
			FlushBkg(m_pDCMain);
			unsigned int i = 0;
			unsigned int n = m_pShapeArray.size();
			for (i = 0; i < n; i++)
			{
				m_pShapeArray[i]->SetZoomFactor(m_dfZoom);
				m_pShapeArray[i]->Draw(m_pDCMain, m_rcDest,m_dfGeoOffsetX, m_dfGeoOffsetY);
			}
			m_bRedrawAll = false;
		}

		pDC->BitBlt(0,0,m_rcDest.Width(), m_rcDest.Height(),m_pDCMain, 0, 0,SRCCOPY);
	
	}
}
/*
bool CShapeArray::Move(int nDirection)
{
	m_bRedrawAll = false;

	switch(nDirection)
	{
	case MOVE_LEFT:
		{
			//获取要补画的块大小
			CRect rcPatch(0,0,m_nScreenStepX, m_rcDest.Height());

			//更新偏移量(地理)
			m_dfGeoOffsetX -= m_nScreenStepX / m_dfZoom;

			//利用原有DC直接填充部分新的画图区
			m_pDCMain->BitBlt(m_nScreenStepX, 0, m_rcDest.Width() - m_nScreenStepX,
				m_rcDest.Height(), m_pDCMain, 0,0,SRCCOPY);

			//画块，并填充到新的画图区
			FlushBkg(m_pDCHorPatch);
			std::vector<CShape*>::iterator itr;
			for(itr = m_pShapeArray.begin(); itr!=m_pShapeArray.end(); itr++)
			{
				(*itr)->Render(m_pDCHorPatch, rcPatch, m_dfGeoOffsetX, m_dfGeoOffsetY) ;	
			}
			m_pDCMain->BitBlt(0, 0, m_nScreenStepX, m_rcDest.Height(), m_pDCHorPatch, 0,0,SRCCOPY);

			break;
		}
	case MOVE_RIGHT:
		{
			CRect rcPatch(0,0,m_nScreenStepX, m_rcDest.Height());

			//画块，并填充到新的画图区
			FlushBkg(m_pDCHorPatch);
			std::vector<CShape*>::iterator itr;
			for(itr = m_pShapeArray.begin(); itr!=m_pShapeArray.end(); itr++)
			{
				(*itr)->Render(m_pDCHorPatch, rcPatch, 
						m_dfGeoOffsetX + m_rcDest.Width() / m_dfZoom,m_dfGeoOffsetY) ;
			}

			m_pDCMain->BitBlt(0, 0, m_rcDest.Width() - m_nScreenStepX,
				m_rcDest.Height(), m_pDCMain, m_nScreenStepX,0,SRCCOPY);

			m_pDCMain->BitBlt(m_rcDest.Width() - m_nScreenStepX, 0,
				m_nScreenStepX, m_rcDest.Height(), m_pDCHorPatch, 0,0,SRCCOPY);

			m_dfGeoOffsetX += m_nScreenStepX / m_dfZoom;

			break;
		}
	case MOVE_DOWN:
		{
			m_dfGeoOffsetY -= m_nScreenStepY / m_dfZoom;

			m_pDCMain->BitBlt(0, 0, m_rcDest.Width(),
				m_rcDest.Height()- m_nScreenStepY, m_pDCMain, 0,m_nScreenStepY,SRCCOPY);

			CRect rcPatch(0,0, m_rcDest.Width(), m_nScreenStepY);

			//画块，并填充到新的画图区
			FlushBkg(m_pDCVerPatch);
			std::vector<CShape*>::iterator itr;
			for(itr = m_pShapeArray.begin(); itr!=m_pShapeArray.end(); itr++)
			{
				(*itr)->Render(m_pDCVerPatch, rcPatch, m_dfGeoOffsetX, m_dfGeoOffsetY) ;
			}

			m_pDCMain->BitBlt(0, m_rcDest.Height() - m_nScreenStepY, 
				m_rcDest.Width(), m_nScreenStepY, m_pDCVerPatch, 0,0,SRCCOPY);

			break;
		}
	case MOVE_UP:
		{
			CRect rcPatch(0,0, m_rcDest.Width(),m_nScreenStepY);
			//画块，并填充到新的画图区
			FlushBkg(m_pDCVerPatch);
			std::vector<CShape*>::iterator itr;
			for(itr = m_pShapeArray.begin(); itr!=m_pShapeArray.end(); itr++)
			{
				(*itr)->Render(m_pDCVerPatch, rcPatch, m_dfGeoOffsetX, 
						m_dfGeoOffsetY + m_rcDest.Height() / m_dfZoom) ;
			}

			m_pDCMain->BitBlt(0, m_nScreenStepY, m_rcDest.Width(), m_rcDest.Height() - m_nScreenStepY,
				m_pDCMain,0, 0,SRCCOPY);

			m_pDCMain->BitBlt(0, 0, m_rcDest.Width(), m_nScreenStepY, m_pDCVerPatch, 0,0,SRCCOPY);

			m_dfGeoOffsetY += m_nScreenStepY / m_dfZoom;

			break;
		}
	}


	return true;
}
*/
bool CShapeArray::Zoom(int nInOrOut)
{
	if (m_pShapeArray.empty())
		return false;

	m_dfZoom *= 1.5;
	
	//设置重画标志为真
	m_bRedrawAll = true;

	unsigned int i = 0;
	unsigned int n = m_pShapeArray.size();
	for (i = 0; i < n; i++)
	{
		m_pShapeArray[i]->SetZoomFactor(m_dfZoom);
	}	
	return true;
}

void CShapeArray::TransScreenToGeo(CPoint point, double& dfGeoX, double& dfGeoY)
{
	//屏幕左下角对应地理偏移量(m_dfGeoOffsetX, m_dfGeoOffsetY)，屏幕坐标point原点在左上角
	int nPxlX = point.x;
	int nPxlY = m_rcDest.Height() - 1 - point.y;

	dfGeoX = m_dfGeoOffsetX + nPxlX / m_dfZoom;
	dfGeoY = m_dfGeoOffsetY + nPxlY / m_dfZoom;

}

void CShapeArray::AutoZoom(CString name)
{
	//这个不行
	//std::vector<CShape*>::iterator itr= m_pShapeArray.begin();
	//CString name1 = (*itr)->GetName();

	//而这个就可以
	//int i = 0;
	//CShape* pShape = m_pShapeArray[i];
	//CString name1 = pShape->GetName();

	unsigned int i = 0;
	unsigned int n = m_pShapeArray.size();
	for (i = 0; i < n; i++)
	{
		if (name == m_pShapeArray[i]->GetName())
		{
			double dfZoomV = (double)m_rcDest.Height() / m_pShapeArray[i]->GetRcSource().Height();
			double dfZoomH = (double)m_rcDest.Width() / m_pShapeArray[i]->GetRcSource().Width();

			m_dfZoom = dfZoomH < dfZoomV ? dfZoomH : dfZoomV;

			m_dfGeoOffsetX = m_pShapeArray[i]->GetRcSource().left - (m_rcDest.Width() - m_pShapeArray[i]->GetRcSource().Width() * m_dfZoom)/2 / m_dfZoom;
			m_dfGeoOffsetY = m_pShapeArray[i]->GetRcSource().top - (m_rcDest.Height() - m_pShapeArray[i]->GetRcSource().Height() * m_dfZoom)/2 / m_dfZoom;
			
			break;
		}
	}

	for (i = 0; i < n; i++)
	{
		m_pShapeArray[i]->SetZoomFactor(m_dfZoom);
	}

	
}

void CShapeArray::AutoZoom()
{
	if (m_pShapeArray.empty())
		return;

	unsigned int i = 0;
	unsigned int n = m_pShapeArray.size();

	CRect rcGeoSource;

	for (i = 0; i < n; i++)
	{
		if (i == 0)
			rcGeoSource = m_pShapeArray[i]->GetRcSource();
		else
		{
			if (rcGeoSource.left > m_pShapeArray[i]->GetRcSource().left)
				rcGeoSource.left = m_pShapeArray[i]->GetRcSource().left;
			if (rcGeoSource.right < m_pShapeArray[i]->GetRcSource().right)
				rcGeoSource.right = m_pShapeArray[i]->GetRcSource().right;
			if (rcGeoSource.top > m_pShapeArray[i]->GetRcSource().top)
				rcGeoSource.top = m_pShapeArray[i]->GetRcSource().top;
			if (rcGeoSource.bottom < m_pShapeArray[i]->GetRcSource().bottom)
				rcGeoSource.bottom = m_pShapeArray[i]->GetRcSource().bottom;
		}
	}

	double dfZoomV = (double)m_rcDest.Height() / rcGeoSource.Height();
	double dfZoomH = (double)m_rcDest.Width() / rcGeoSource.Width();

	m_dfZoom = dfZoomH < dfZoomV ? dfZoomH : dfZoomV;

	m_dfGeoOffsetX = rcGeoSource.left - (m_rcDest.Width() - rcGeoSource.Width() * m_dfZoom)/2 / m_dfZoom;
	m_dfGeoOffsetY = rcGeoSource.top - (m_rcDest.Height() - rcGeoSource.Height() * m_dfZoom)/2 / m_dfZoom;

	for (i = 0; i < n; i++)
	{
		m_pShapeArray[i]->SetZoomFactor(m_dfZoom);
	}
}

void CShapeArray::ResizeRcDest(CRect rcDest,CDC* pDC)
{
	m_rcDest = rcDest;
	m_bRedrawAll = true;

	m_BmpMain.DeleteObject();
	m_BmpMain.CreateCompatibleBitmap(pDC, m_rcDest.Width(),m_rcDest.Height());
	m_pDCMain->SelectObject(&m_BmpMain);

	m_BmpMove.DeleteObject();
	m_BmpMove.CreateCompatibleBitmap(pDC, m_rcDest.Width(),m_rcDest.Height());
	m_pDCMove->SelectObject(&m_BmpMove);

}

CShape* CShapeArray::GetShape(int nIndex)
{
	unsigned int i = 0;
	unsigned int n = m_pShapeArray.size();
	for (i = 0; i < n; i++)
	{
		if (i == nIndex)
		{
			return m_pShapeArray[i];
		}
	}

	return NULL;
}

void CShapeArray::RemoveAll(void)
{
	m_pShapeArray.clear();

}

void CShapeArray::RemoveShape(CString name)
{
	unsigned int i = 0;
	unsigned int n = m_pShapeArray.size();
	for (i = 0; i < n; i++)
	{
		if (name == m_pShapeArray[i]->GetName())
		{
			m_pShapeArray.erase(m_pShapeArray.begin()+i);
			break;
		}
	}
}

void CShapeArray::SetZoomFactor(double dfZoom)
{
	m_dfZoom = dfZoom;
	unsigned int i = 0;
	unsigned int n = m_pShapeArray.size();
	for (i = 0; i < n; i++)
	{
		m_pShapeArray[i]->SetZoomFactor(m_dfZoom);
	}
}