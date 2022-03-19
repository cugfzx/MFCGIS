

#include <vector>
#include "../ShapeEngine/ESRIFactory.h"

using namespace std;

class CShapeArray
{
public:
	CShapeArray();
public:
	~CShapeArray(void);

	//是否为空
	bool IsEmpty(void) {return m_pShapeArray.empty();};
	int GetShapeCount(void) {return (int)(m_pShapeArray.size());};

	//初始化，调用所有函数前调用
	void Initialize(CDC *pDC); 
	bool IsInited(void) {return m_bInit;};

	//绘图区大小改变时调用
	void ResizeRcDest(CRect rcDest,CDC *pDC);

	//绘图函数、移动和缩放
	void Render(CDC* pDC);
	void RenderMove(CDC* pDC,int nOffsetX, int nOffsetY, bool bReDraw = false);

	void ReDraw(void) {m_bRedrawAll = true;};
	//bool Move(int nDirection);
	bool Zoom(int nInOrOut);
	void AutoZoom(void);//根据客户区计算能够展示所有图层全景的缩放系数和偏移量
	void AutoZoom(CString name);//根据客户区自动计算能够展示“name”图层全景的缩放系数和偏移量
	
	enum MOVE_DIRECTION {MOVE_LEFT = 0, MOVE_RIGHT, MOVE_UP, MOVE_DOWN};

	//添加、删除、获取矢量对象
	void AddShape(const char* path);
	void AddShape(CShape* pShape);
	CShape* GetShape(int nIndex);
	void RemoveShape(CString name);
	void RemoveAll(void);

	//获取绘图缓存
	inline CDC* GetMainDC(void) {return m_pDCMain;};

	void SetZoomFactor(double dfZoom);
	double  GetZoomFactor(void){return m_dfZoom;};

	void SetOffset(double dfOffsetX, double dfOffsetY) { m_dfGeoOffsetX = dfOffsetX; m_dfGeoOffsetY = dfOffsetY ; m_bRedrawAll = true;};
	double GetOffsetX(void) {return m_dfGeoOffsetX;};
	double GetOffsetY(void) {return m_dfGeoOffsetY;};
	
	//返回栅格化的矢量图
	const CBitmap* GetBitmap(void) {return &m_BmpMain;};

	//根据屏幕点返回地理坐标
	void TransScreenToGeo(CPoint point, double& dfGeoX, double& dfGeoY);

	int id;
	static int count;

protected:

	//刷新背景
	void FlushBkg(CDC* pDC);
	
	//根据指定参数绘制小块DC，并粘贴到主DC
	void PastePatch(CRect rcPatch, CDC* pDCPatch,CDC* pDstDC,double dfOffsetX, double dfOffsetY);
	
private:

	//主要shape数据
	vector<CShape*> m_pShapeArray;

	//显示设备绘图区中用于绘制的目标区
	CRect m_rcDest;

	//要绘制的地理坐标的左上角点坐标
	double m_dfGeoOffsetX;
	double m_dfGeoOffsetY;

	//后台DC
	CDC *m_pDCMain, *m_pDCMove,*m_pDCPatch;
	CBitmap m_BmpMain, m_BmpMove,m_BmpPatch;

	//是否全部重绘
	bool m_bRedrawAll;

	//background brush
	CBrush m_BkgBrush;

	//每次移动的屏幕偏移量
	int m_nScreenStepX;
	int m_nScreenStepY;

	//缩放系数
	double m_dfZoom;

	//是否已初始化
	bool m_bInit;


};

typedef CShapeArray ShapeManager;
