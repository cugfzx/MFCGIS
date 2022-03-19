

#include <vector>
#include "../ShapeEngine/ESRIFactory.h"

using namespace std;

class CShapeArray
{
public:
	CShapeArray();
public:
	~CShapeArray(void);

	//�Ƿ�Ϊ��
	bool IsEmpty(void) {return m_pShapeArray.empty();};
	int GetShapeCount(void) {return (int)(m_pShapeArray.size());};

	//��ʼ�����������к���ǰ����
	void Initialize(CDC *pDC); 
	bool IsInited(void) {return m_bInit;};

	//��ͼ����С�ı�ʱ����
	void ResizeRcDest(CRect rcDest,CDC *pDC);

	//��ͼ�������ƶ�������
	void Render(CDC* pDC);
	void RenderMove(CDC* pDC,int nOffsetX, int nOffsetY, bool bReDraw = false);

	void ReDraw(void) {m_bRedrawAll = true;};
	//bool Move(int nDirection);
	bool Zoom(int nInOrOut);
	void AutoZoom(void);//���ݿͻ��������ܹ�չʾ����ͼ��ȫ��������ϵ����ƫ����
	void AutoZoom(CString name);//���ݿͻ����Զ������ܹ�չʾ��name��ͼ��ȫ��������ϵ����ƫ����
	
	enum MOVE_DIRECTION {MOVE_LEFT = 0, MOVE_RIGHT, MOVE_UP, MOVE_DOWN};

	//��ӡ�ɾ������ȡʸ������
	void AddShape(const char* path);
	void AddShape(CShape* pShape);
	CShape* GetShape(int nIndex);
	void RemoveShape(CString name);
	void RemoveAll(void);

	//��ȡ��ͼ����
	inline CDC* GetMainDC(void) {return m_pDCMain;};

	void SetZoomFactor(double dfZoom);
	double  GetZoomFactor(void){return m_dfZoom;};

	void SetOffset(double dfOffsetX, double dfOffsetY) { m_dfGeoOffsetX = dfOffsetX; m_dfGeoOffsetY = dfOffsetY ; m_bRedrawAll = true;};
	double GetOffsetX(void) {return m_dfGeoOffsetX;};
	double GetOffsetY(void) {return m_dfGeoOffsetY;};
	
	//����դ�񻯵�ʸ��ͼ
	const CBitmap* GetBitmap(void) {return &m_BmpMain;};

	//������Ļ�㷵�ص�������
	void TransScreenToGeo(CPoint point, double& dfGeoX, double& dfGeoY);

	int id;
	static int count;

protected:

	//ˢ�±���
	void FlushBkg(CDC* pDC);
	
	//����ָ����������С��DC����ճ������DC
	void PastePatch(CRect rcPatch, CDC* pDCPatch,CDC* pDstDC,double dfOffsetX, double dfOffsetY);
	
private:

	//��Ҫshape����
	vector<CShape*> m_pShapeArray;

	//��ʾ�豸��ͼ�������ڻ��Ƶ�Ŀ����
	CRect m_rcDest;

	//Ҫ���Ƶĵ�����������Ͻǵ�����
	double m_dfGeoOffsetX;
	double m_dfGeoOffsetY;

	//��̨DC
	CDC *m_pDCMain, *m_pDCMove,*m_pDCPatch;
	CBitmap m_BmpMain, m_BmpMove,m_BmpPatch;

	//�Ƿ�ȫ���ػ�
	bool m_bRedrawAll;

	//background brush
	CBrush m_BkgBrush;

	//ÿ���ƶ�����Ļƫ����
	int m_nScreenStepX;
	int m_nScreenStepY;

	//����ϵ��
	double m_dfZoom;

	//�Ƿ��ѳ�ʼ��
	bool m_bInit;


};

typedef CShapeArray ShapeManager;
