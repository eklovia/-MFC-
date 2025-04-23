
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "01.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
extern CPoint Bezier[13];
extern int mm;
extern CDC* pdc;
extern CPen pen,*old;
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()

	ON_COMMAND(ID_32771, &CMainFrame::On32771)
	ON_COMMAND(ID_32772, &CMainFrame::On32772)
	ON_COMMAND(ID_32773, &CMainFrame::On32773)
	ON_COMMAND(ID_32774, &CMainFrame::On32774)
	ON_COMMAND(ID_32775, &CMainFrame::On32775)
	ON_COMMAND(ID_32776, &CMainFrame::On32776)
	ON_COMMAND(ID_32777, &CMainFrame::On32777)
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnFileNew)
	ON_COMMAND(ID_32784, &CMainFrame::On32784)
	ON_COMMAND(ID_BRESENHAM_32785, &CMainFrame::OnBresenham32785)
	ON_COMMAND(ID_32786, &CMainFrame::On32786)
	ON_COMMAND(ID_32788, &CMainFrame::On32788)
	ON_COMMAND(ID_32789, &CMainFrame::On32789)
	ON_COMMAND(ID_BEZIER_B32790, &CMainFrame::OnBezierB32790)
	ON_COMMAND(ID_BEZIER_B32791, &CMainFrame::OnBezierB32791)
	ON_COMMAND(ID_32794, &CMainFrame::On32794)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};



// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}


extern shape sp;

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_menu.Detach();
	m_menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_menu);
	return 0;
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序


extern int condition;
void CMainFrame::On32771()
{
	// TODO: 在此添加命令处理程序代码
	condition = 1;

}




void CMainFrame::On32772()
{
	// TODO: 在此添加命令处理程序代码
	condition = 2;

}


void CMainFrame::On32773()
{
	// TODO: 在此添加命令处理程序代码
	condition = 3;
}


void CMainFrame::On32774()
{
	// TODO: 在此添加命令处理程序代码
	condition = 4;
}


void CMainFrame::On32775()
{
	// TODO: 在此添加命令处理程序代码
	condition = 5;

}


void CMainFrame::On32776()//清屏
{
	// TODO: 在此添加命令处理程序代码
	sp.destr();
	sp.getDashSeq() = sp.getCurrentSelect() = -1;
	CRect rec;
	CDC* pDC;
	GetClientRect(&rec);
	pDC = GetDC();
	drawScreen(rec, pDC);

	ReleaseDC(pDC);
	
}


void CMainFrame::On32777()
{
	// TODO: 在此添加命令处理程序代码

	condition = 7;
}
extern HWND hwnd;
extern CRect recforshape;



void CMainFrame::OnFileNew()
{
	// TODO: 在此添加命令处理程序代码
	sp.destr();
	CRect rec;
	GetClientRect(&rec);
	pdc = GetDC();
	drawScreen(rec, pdc);
	ReleaseDC(pdc);
}


void CMainFrame::On32784()
{
	// TODO: 在此添加命令处理程序代码
	condition = 8;
}


void CMainFrame::OnBresenham32785()//bresenham画圆
{
	// TODO: 在此添加命令处理程序代码
	condition = 9;
}

void CMainFrame::On32786()
{
	// TODO: 在此添加命令处理程序代码
	condition = 10;
}


void CMainFrame::On32788()//填充
{
	condition = 11;
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::On32789()
{
	// TODO: 在此添加命令处理程序代码
	condition = 12;
}


void CMainFrame::OnBezierB32790()
{
	// TODO: 在此添加命令处理程序代码
	condition = 13;
}


void CMainFrame::OnBezierB32791()
{

	pdc = GetDC();
	old=pdc->SelectObject(&pen);
	BezierNn(Bezier, mm);
	pdc->SelectObject(old);
	ReleaseDC(pdc);
	mm = 0;
}





void CMainFrame::On32794()
{
	// TODO: 在此添加命令处理程序代码
	condition = 14;
}
