
// MFCGISView.h: CMFCGISView 类的接口
//

#pragma once

enum DRAW_STATUS { DRAW_NORMAL, DRAW_PAN_MOVE, DRAW_PAN_REDRAW };

class CMFCGISView : public CView
{
protected: // 仅从序列化创建
	CMFCGISView() noexcept;
	DECLARE_DYNCREATE(CMFCGISView)

// 特性
public:
	CMFCGISDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMFCGISView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()


public:
	void OnBtnPan();

private:
	bool m_bPan;//是否处于手状态下
	bool m_bMouseMovingAndPan;//是否手状态并且移动
	CPoint m_RefPoint, m_CurPoint;//起点和终点
	DRAW_STATUS m_howToDraw;//绘图方法
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // MFCGISView.cpp 中的调试版本
inline CMFCGISDoc* CMFCGISView::GetDocument() const
   { return reinterpret_cast<CMFCGISDoc*>(m_pDocument); }
#endif

