
// MFCGISView.cpp: CMFCGISView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCGIS.h"
#endif

#include "MFCGISDoc.h"
#include "MFCGISView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCGISView

IMPLEMENT_DYNCREATE(CMFCGISView, CView)

BEGIN_MESSAGE_MAP(CMFCGISView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCGISView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_BTN_PAN, &CMFCGISView::OnBtnPan)//拖移
	//ON_UPDATE_COMMAND_UI(ID_BTN_PAN, &CMFCGISView::OnUpdateBtnPan)//设置单选按钮选中状态。
	//ON_COMMAND(ID_BTN_ZOOM_AUTO, &CMFCGISView::OnBtnZoomAuto)//自动视野
	//ON_COMMAND(ID_BTN_ZOOM_IN, &CMFCGISView::OnBtnZoomIn)//放大
	//ON_COMMAND(ID_BTN_ZOOM_OUT, &CMFCGISView::OnBtnZoomOut)//缩小
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CMFCGISView 构造/析构

CMFCGISView::CMFCGISView() noexcept
{
	m_bPan = false;
	m_howToDraw = DRAW_NORMAL;
	m_bMouseMovingAndPan = false;
}

CMFCGISView::~CMFCGISView()
{
}

BOOL CMFCGISView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMFCGISView 绘图

void CMFCGISView::OnDraw(CDC* pDC)
{
	CMFCGISDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	switch (m_howToDraw)
	{
	case DRAW_NORMAL:
	{
		pDoc->GetShapeFilesManager()->Render(pDC);
		break;
	}
	case DRAW_PAN_MOVE:
	{
		int nOffsetX = -(m_CurPoint.x - m_RefPoint.x);// 屏幕偏移量
		int nOffsetY = -(m_CurPoint.y - m_RefPoint.y);
		pDoc->GetShapeFilesManager()->RenderMove(pDC, nOffsetX, nOffsetY);
		break;
	}
	case DRAW_PAN_REDRAW:
	{
		int nOffsetX = -(m_CurPoint.x - m_RefPoint.x);// 屏幕偏移量
		int nOffsetY = -(m_CurPoint.y - m_RefPoint.y);
		pDoc->GetShapeFilesManager()->RenderMove(pDC, nOffsetX, nOffsetY, true);
		m_bMouseMovingAndPan = false;
		m_howToDraw = DRAW_NORMAL;
	}
	default:
	{
		pDoc->GetShapeFilesManager()->Render(pDC);
		break;
	}
	}
}


// CMFCGISView 打印

BOOL CMFCGISView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCGISView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCGISView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMFCGISView 诊断

#ifdef _DEBUG
void CMFCGISView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCGISView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCGISDoc* CMFCGISView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCGISDoc)));
	return (CMFCGISDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCGISView 消息处理程序
void CMFCGISView::OnBtnPan()
{
	// TODO: Add your command handler code here
	m_bPan = !m_bPan;
}

void CMFCGISView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bPan)
	{
		m_RefPoint = point;
		m_howToDraw = DRAW_NORMAL;
	}
	CView::OnLButtonDown(nFlags, point);
}


void CMFCGISView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bPan && m_bMouseMovingAndPan)
	{
		m_CurPoint = point;
		m_bMouseMovingAndPan = false;
		m_howToDraw = DRAW_PAN_REDRAW;
		Invalidate(false);
	}
	CView::OnLButtonUp(nFlags, point);
}


void CMFCGISView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bPan && (nFlags == MK_LBUTTON))
	{
		m_CurPoint = point;
		m_bMouseMovingAndPan = true;
		m_howToDraw = DRAW_PAN_MOVE;
		Invalidate(false);
	}
	CView::OnMouseMove(nFlags, point);
}


void CMFCGISView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CMFCGISDoc* pDoc = (CMFCGISDoc*)GetDocument();

	if (pDoc->GetShapeFilesManager()->IsInited())
	{
		CRect rcDest;
		GetClientRect(&rcDest);
		pDoc->GetShapeFilesManager()->ResizeRcDest(rcDest, GetWindowDC());
		pDoc->GetShapeFilesManager()->ReDraw();

	}

	Invalidate(false);
}


BOOL CMFCGISView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CMFCGISDoc* pDoc = (CMFCGISDoc*)GetDocument();
	double f = pDoc->GetShapeFilesManager()->GetZoomFactor();
	if (zDelta > 0)//放大
	{
		pDoc->GetShapeFilesManager()->SetZoomFactor(f * 1.2);
		pDoc->GetShapeFilesManager()->ReDraw();
	}
	else if (zDelta < 0)
	{
		pDoc->GetShapeFilesManager()->SetZoomFactor(f / 1.2);
		pDoc->GetShapeFilesManager()->ReDraw();
	}
	Invalidate(false);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
