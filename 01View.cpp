
// 01View.cpp: CMy01View 类的实现
//
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "01.h"
#endif

#include "01Doc.h"
#include "01View.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPoint Or;
CBitmap* oldBitMap;
CPen* old;
CPen pen(PS_SOLID, 1, RGB(255, 0, 255));
CPen pen_x(PS_DASH, 1, RGB(255, 0, 255));
CPen pen_b(PS_DASH, 1, RGB(0, 255, 255));
CDC* pdc;
extern int condition = -1;
CRect recforshape;
CString ss;
shape sp;
double dis,kk,ykb;
bool a = false;
int dx, dy;
unsigned int SeqPub;
bool ifDash = false;
bool LRel=true;
CPoint Bezier[13];
HWND hwnd;
int mm = 0;


// CMy01View 

IMPLEMENT_DYNCREATE(CMy01View, CView)

BEGIN_MESSAGE_MAP(CMy01View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_OPEN, &CMy01View::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CMy01View::OnFileSave)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CMy01View 构造/析构

CMy01View::CMy01View() noexcept
{
	// TODO: 在此处添加构造代码

}

CMy01View::~CMy01View()
{
}

BOOL CMy01View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	
	return CView::PreCreateWindow(cs);
}

void BoundaryFill_4Connection(CPoint& startPoint)
{
	COLORREF currentCol= RGB(255, 0, 255);
	std::stack<LONG>ss;
	ss.push(startPoint.x);
	ss.push(startPoint.y);
	LONG x, y;
	while (!ss.empty())
	{
		y = ss.top();
		ss.pop();
		x = ss.top();
		ss.pop();
		pdc->SetPixelV(x, y, currentCol);

		if (pdc->GetPixel(x, y + 1) != currentCol) {
			ss.push(x);
			ss.push(y + 1);
		}

		if (pdc->GetPixel(x + 1, y) != currentCol) {
			ss.push(x + 1); 
			ss.push(y);
		}

		if (pdc->GetPixel(x - 1, y) != currentCol) {
			ss.push(x - 1);
			ss.push(y);
		}

		if (pdc->GetPixel(x, y - 1) != currentCol) {
			ss.push(x);
			ss.push(y - 1);
		}
	}
}

void ScanLineFill( CPoint startPoint)
{
	CArray<CPoint, CPoint>Stack;
	CPoint fillPoint = startPoint;
	COLORREF currentCol = pdc->GetPixel(startPoint);
	int xl, xr;
	bool spanNeedFill;
	CPoint pt;

	Stack.RemoveAll();
	pt.x = fillPoint.x;
	pt.y = fillPoint.y;
	Stack.Add(pt);
	while (Stack.GetSize() != 0)
	{
		pt = Stack[Stack.GetSize() - 1];
		Stack.RemoveAt(Stack.GetSize() - 1);
		fillPoint.y = pt.y;
		fillPoint.x = pt.x;

		while (pdc->GetPixel(fillPoint.x, fillPoint.y) == currentCol)
		{
			pdc->SetPixelV(fillPoint, RGB(255, 0, 255));
			fillPoint.x++;
		}
		xr = fillPoint.x - 1;
		fillPoint.x = pt.x - 1;

		while (pdc->GetPixel(fillPoint.x, fillPoint.y) == currentCol)
		{
			pdc->SetPixelV(fillPoint, RGB(255, 0, 255));
			fillPoint.x--;
		}
		xl = fillPoint.x + 1;

		for (int I = 0; I < 2; I++)
		{
			fillPoint.x = xl;
			if (I == 0) fillPoint.y = fillPoint.y + 1;
			else fillPoint.y = fillPoint.y - 2;

			while (fillPoint.x < xr)
			{
				spanNeedFill = FALSE;
				while (pdc->GetPixel(fillPoint.x, fillPoint.y) == currentCol)
				{
					spanNeedFill = TRUE;
					fillPoint.x++;
				}

				if (spanNeedFill)
				{
					pt.x = fillPoint.x - 1;
					pt.y = fillPoint.y;
					Stack.Add(pt);
					spanNeedFill = FALSE;
				}
				while (pdc->GetPixel(fillPoint.x, fillPoint.y) != currentCol && fillPoint.x < xr)
					fillPoint.x++;
			}
		}
	}

}

void circle_points(int xc, int yc, int x, int y)//根据对称性画出另外 7 部分的点
{
	pdc->SetPixelV(xc + x, yc + y,RGB(255,0,255));
	pdc->SetPixelV(xc - x, yc + y,RGB(255,0,255));
	pdc->SetPixelV(xc + x, yc - y,RGB(255,0,255));
	pdc->SetPixelV(xc - x, yc - y,RGB(255,0,255));
	pdc->SetPixelV(xc + y, yc + x,RGB(255,0,255));
	pdc->SetPixelV(xc - y, yc + x,RGB(255,0,255));
	pdc->SetPixelV(xc + y, yc - x,RGB(255,0,255));
	pdc->SetPixelV(xc - y, yc - x,RGB(255,0,255));
}

void BresenhamCircle(int x1, int y1, int r)//圆的生成
{
	int x, y, p;
	x = 0;
	y = r;
	p = 3 - 2 * r;
	while (x < y)
	{
		circle_points(x1, y1, x, y);
		if (p < 0)
			p = p + 4 * x + 6;
		else
		{
			p = p + 4 * (x - y) + 10;
			y -= 1;
		}
		x += 1;
	}
	if (x == y)
		circle_points(x1, y1, x, y);
}


void Line_Midpoint(int x1, int y1, int x2, int y2,CDC*&pDC)
{
	int x = x1, y = y1;	//赋初始点
	int dy = y1 - y2, dx = x2 - x1;
	int delta_x = (dx >= 0 ? 1 : (dx = -dx, -1));	//若dx>0则步长为1，否则为-1，同时dx变正
	int delta_y = (dy <= 0 ? 1 : (dy = -dy, -1));	//注意这里dy<0,才是画布中y的增长方向

	pDC->SetPixelV(x, y, RGB(255,0,255));		//画起始点

	int d, incrE, incrNE;
	if (-dy <= dx)		// 斜率绝对值 <= 1
		//这里-dy即画布中的dy
	{ 
		d = 2 * dy + dx;	//初始化判断式d
		incrE = 2 * dy;		//取像素E时判别式增量
		incrNE = 2 * (dy + dx);//NE
		while (x != x2)
		{
			if (d < 0)
				y += delta_y, d += incrNE;
			else
				d += incrE;
			x += delta_x;
			pDC->SetPixelV(x, y, RGB(255, 0, 255));
		}
	}
	else				// 斜率绝对值 > 1
						// x和y情况互换
	{
		d = 2 * dx + dy;
		incrE = 2 * dx;
		incrNE = 2 * (dy + dx);
		while (y != y2)
		{
			if (d < 0)	//注意d变化情况
				d += incrE;
			else
				x += delta_x, d += incrNE;
			y += delta_y;
			pDC->SetPixelV(x, y, RGB(255, 0, 255));
		}
	}
}


// CMy01View 绘图

void CMy01View::OnDraw(CDC* pDC)
{
	CMy01Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	GetClientRect(&recforshape);
	pDC = GetDC();
	drawScreen(recforshape, pDC);
	old=pDC->SelectObject(&pen);
	pDC->SelectStockObject(NULL_BRUSH);
	sp.DrawBegin(pDC);

	hwnd = this->m_hWnd;
	pDC->SelectObject(old);
	ReleaseDC(pDC);


};


// CMy01View 打印

BOOL CMy01View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备


	return DoPreparePrinting(pInfo);
}

void CMy01View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程	
}

void CMy01View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMy01View 诊断

#ifdef _DEBUG
void CMy01View::AssertValid() const
{
	CView::AssertValid();
}

void CMy01View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
	
}

CMy01Doc* CMy01View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy01Doc)));
	return (CMy01Doc*)m_pDocument;
}
#endif //_DEBUG


// CMy01View 消息处理程序

void CMy01View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Or = point;
	LRel = false;
	GetClientRect(&recforshape);
	CView::OnLButtonDown(nFlags, point);
}


void CMy01View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	LRel = true;
	pdc = GetDC();
	old = pdc->SelectObject(&pen);
	switch (condition)
	{
	case 1://直线
	{	
		if (Or.x == point.x || Or.y == point.y)return;

		pdc->MoveTo(Or);
		pdc->LineTo(point);
		class line* e = new class line(Or, point);
		sp.add(e);
		GetClientRect(&recforshape);
		sp.cross();
	}
		break;
	case 2://曲线
	{

		if (Or.x == point.x || Or.y == point.y)return;

		pdc->SelectStockObject(NULL_BRUSH);
		dis = sqrt(pow(static_cast<double>(Or.x - point.x), 2.0) + pow(static_cast<double>(Or.y - point.y), 2.0));
		Or.x = (Or.x + point.x) / 2;
		Or.y = (Or.y + point.y) / 2;
		recforshape.SetRect(Or.x - dis / 2, Or.y - dis / 2, Or.x + dis / 2, Or.y + dis / 2);
		class curve* e = new class curve(recforshape);
		pdc->Arc(Or.x - dis / 2, Or.y - dis / 2, Or.x + dis / 2, Or.y + dis / 2, Or.x + dis, Or.y, Or.x - dis, Or.y);
		sp.add(e);
		break;
	}
	case 3: {//圆
		if (Or.x == point.x || Or.y == point.y)return;

		pdc->SelectStockObject(NULL_BRUSH);
		dis = sqrt(pow(static_cast<double>(Or.x - point.x), 2.0) + pow(static_cast<double>(Or.y - point.y), 2.0));
		Or.x = (Or.x + point.x) / 2;
		Or.y = (Or.y + point.y) / 2;
		recforshape.SetRect(Or.x - dis / 2, Or.y - dis / 2, Or.x + dis / 2, Or.y + dis / 2);
		my_ellipse* e = new my_ellipse(recforshape);
		pdc->Ellipse(recforshape);
		sp.add(e);
		GetClientRect(&recforshape);
		//sp.cross();
		break;
	}
	case 4://矩形
	{
		if (Or.x == point.x || Or.y == point.y)return;

		pdc->SelectStockObject(NULL_BRUSH);
		if (Or.x > point.x)
		{
			Or.x ^= point.x ^= Or.x^=point.x;
		}

		if (Or.y > point.y)
		{
			Or.y ^= point.y ^= Or.y^=point.y;
		}
		
		recforshape.SetRect(Or.x, Or.y, point.x, point.y);
		rec* my_rec = new rec(recforshape);
		pdc->Rectangle(recforshape);
		sp.add(my_rec);
	}
	break;
	case 5: {
		Or = point;
	}
		  break;
	case 7: {

		GetClientRect(&recforshape);

		if (sp.getCurrentSelect() != -1) {
			drawScreen(recforshape, pdc);
			pdc->SelectStockObject(NULL_BRUSH);
			sp.DrawBegin(pdc);
			sp.getEle()[sp.getCurrentSelect()]->dashDraw(pdc);
			ifDash = true;

		}
		else {
			drawScreen(recforshape, pdc);
			pdc->SelectStockObject(NULL_BRUSH);
			sp.DrawBegin(pdc);
			sp.getCurrentSelect() = -1;
			ifDash = false;
			sp.getDashSeq() = -1;
		}

		break;
	}
	case 102:
	case 101:
	case 100: 
		condition = 7;
		break;
	case 8: {
		if (Or.x == point.x || Or.y == point.y)return;	
		class line* e = new class line(Or, point);
		sp.add(e);

		Line_Midpoint(Or.x, Or.y, point.x, point.y, pdc);
		break;
	}
	case 9: {
		dis = pow((Or.x - point.x), 2.) + pow(Or.y - point.y, 2.);
		dis = sqrt(dis)/2.;
		Or.x = (Or.x + point.x) / 2;
		Or.y = (Or.y + point.y) / 2;
		recforshape.SetRect(Or.x - dis, Or.y - dis, Or.x + dis, Or.y + dis);
		my_ellipse* e = new my_ellipse(recforshape);
		
		sp.add(e);

		BresenhamCircle(Or.x, Or.y, dis);
	}
		  break;
	case 10: {
		a = false;
	}
		   break;
	case 11: {

		BoundaryFill_4Connection(point);
	}
		   break;
	case 12: {
		pdc = GetDC();
		ScanLineFill(point);
	}
		   break;
	case 13: {
		if (mm >= 13)break;

		Bezier[mm++] = CPoint(point);
		pdc->SetPixelV(point, RGB(255, 0, 255));
		break;
	}
	case 14: {
		if (Or.x > point.x)
		{
			Or.x ^= point.x ^= Or.x ^= point.x;
		}

		if (Or.y > point.y)
		{
			Or.y ^= point.y ^= Or.y ^= point.y;
		}
		pdc->SelectStockObject(NULL_BRUSH);
		drawScreen(recforshape, pdc);
		sp.DrawBegin(pdc);
		Bezier[0].x = point.x - Or.x;
		Bezier[0].y = point.y - Or.y;

		if (Bezier[0].x == 0 || Bezier[0].y == 0)break;
		HDC hdcScreen;
		HDC hdcWindow;
		HDC hdcMemDC = NULL;
		HBITMAP hbmScreen = NULL;

		// Retrieve the handle to a display device context for the client 
		// area of the window. 
		hdcScreen = ::GetDC(m_hWnd);
		hdcWindow = ::GetDC(m_hWnd);

		// Create a compatible DC, which is used in a BitBlt from the window DC.
		hdcMemDC = CreateCompatibleDC(hdcWindow);

		if (!hdcMemDC)
		{
			MessageBox(L"CreateCompatibleDC has failed", L"Failed", MB_OK);

		}

		// Get the client area for size calculation.

		// This is the best stretch mode.
		SetStretchBltMode(hdcWindow, HALFTONE);
		
		// The source DC is the entire screen, and the destination DC is the current window (HWND).
		if (!StretchBlt(hdcWindow,
			0, 0,
			recforshape.Width(), recforshape.Height(),
			hdcScreen,
			Or.x, Or.y,
			Bezier[0].x, Bezier[0].y,
			SRCCOPY))
		{
			MessageBox(L"StretchBlt has failed", L"Failed", MB_OK);
		}

		// Create a compatible bitmap from the Window DC.
		hbmScreen = CreateCompatibleBitmap(hdcWindow, recforshape.Width(),recforshape.Height());

		if (!hbmScreen)
		{
			MessageBox(L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		}

		// Select the compatible bitmap into the compatible memory DC.
		SelectObject(hdcWindow, hbmScreen);

		// Bit block transfer into our compatible memory DC.
		if (!BitBlt(hdcWindow,
			0, 0,
			recforshape.Width(), recforshape.Height(),
			hdcWindow,
			0, 0,
			SRCCOPY))
		{
			MessageBox(L"BitBlt has failed", L"Failed", MB_OK);
		}
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		::ReleaseDC(NULL, hdcScreen);
		::ReleaseDC(m_hWnd, hdcWindow);
		break;
	}

	default:
	{

		drawScreen(recforshape, pdc);
		pdc->SelectStockObject(NULL_BRUSH);
		pdc->SelectObject(&pen);
		sp.DrawBegin(pdc);
		break;
	}
	}
	pdc->SelectObject(old);
	ReleaseDC(pdc);

	CView::OnLButtonUp(nFlags, point);
}

void CMy01View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (condition)
	{
	case 7:
	{

		if (sp.getDashSeq() != -1)
			if (sp.getEle()[sp.getDashSeq()]->judgeVert(point)) {
				SetCursor(LoadCursor(NULL, IDC_CROSS));
				if (!LRel)condition = 101;
				break;
			}
			else if (sp.getEle()[sp.getDashSeq()]->judgeHandle(point)) {
				SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				if (!LRel)condition = 102;
				break;
			}

		if (sp.recy(point)) {
			SetCursor(LoadCursor(NULL, IDC_HAND));
			if (!LRel) {
				condition = 100;
			}
		}
		else {
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			sp.getCurrentSelect() = -1;
		}
		break;
	}
	case 10: {
		if (!LRel) {
			pdc = GetDC();
			pdc->SelectObject(&pen);
			pdc->MoveTo(Or);
			pdc->LineTo(point);
			class line* e = new class line(Or, point);
			sp.add(e);
			Or = point;
			ReleaseDC(pdc);
		}
		break;
	}
	case 14: {
		if (LRel)return;
		CDC dcMem;
		CBitmap bmp;
		GetClientRect(&recforshape);
		pdc = GetDC();

		bmp.CreateCompatibleBitmap(pdc, recforshape.Width(), recforshape.Height());
		dcMem.CreateCompatibleDC(pdc);
		oldBitMap = dcMem.SelectObject(&bmp);
		old = dcMem.SelectObject(&pen);
		dcMem.SelectStockObject(NULL_BRUSH);
		dcMem.FillSolidRect(recforshape, RGB(0, 0, 0));

		drawScreen(recforshape, &dcMem);

		sp.DrawBegin(&dcMem);
		dcMem.SelectObject(old);
		old = dcMem.SelectObject(&pen_b);
		dcMem.Rectangle(Or.x, Or.y, point.x, point.y);
		pdc->BitBlt(0, 0, recforshape.Width(), recforshape.Height(), &dcMem, 0, 0, SRCCOPY);

		dcMem.SelectObject(old);
		dcMem.SelectObject(oldBitMap);
		dcMem.DeleteDC();
		bmp.DeleteObject();
		ReleaseDC(pdc);
		break;
	}
	case 100: {
		if (sp.getCurrentSelect() == -1)break;
		CDC dcMem;
		CBitmap bmp;
		GetClientRect(&recforshape);
		pdc = GetDC();
		
		bmp.CreateCompatibleBitmap(pdc, recforshape.Width(), recforshape.Height());
		dcMem.CreateCompatibleDC(pdc);
		oldBitMap = dcMem.SelectObject(&bmp);
		old = dcMem.SelectObject(&pen);
		dcMem.SelectStockObject(NULL_BRUSH);
		dcMem.FillSolidRect(recforshape, RGB(0, 0, 0));

		drawScreen(recforshape, &dcMem);
		sp.getEle()[sp.getCurrentSelect()]->transform(Or, point);
		sp.DrawBegin(&dcMem);


		Or = point;
		pdc->BitBlt(0, 0, recforshape.Width(), recforshape.Height(), &dcMem, 0, 0, SRCCOPY);

		dcMem.SelectObject(old);
		dcMem.SelectObject(oldBitMap);
		dcMem.DeleteDC();
		bmp.DeleteObject();
		ReleaseDC(pdc);
		break;
	}
	case 101: {

		CDC dcMem;
		CBitmap bmp;
		GetClientRect(&recforshape);
		pdc = GetDC();

		bmp.CreateCompatibleBitmap(pdc, recforshape.Width(), recforshape.Height());
		dcMem.CreateCompatibleDC(pdc);
		oldBitMap = dcMem.SelectObject(&bmp);
		old = dcMem.SelectObject(&pen);
		dcMem.SelectStockObject(NULL_BRUSH);
		dcMem.FillSolidRect(recforshape, RGB(0, 0, 0));

		drawScreen(recforshape, &dcMem);
		sp.getEle()[sp.getDashSeq()]->scale(point);
		sp.DrawBegin(&dcMem);

		sp.getEle()[sp.getDashSeq()]->dashDraw(&dcMem);

		pdc->BitBlt(0, 0, recforshape.Width(), recforshape.Height(), &dcMem, 0, 0, SRCCOPY);

		dcMem.SelectObject(old);
		dcMem.SelectObject(oldBitMap);
		dcMem.DeleteDC();
		bmp.DeleteObject();
		ReleaseDC(pdc);
		break;
	}

	case 102: {
		if (sp.getDashSeq() < 0)return;
		CDC dcMem;
		CBitmap bmp;
		pdc = GetDC();

		bmp.CreateCompatibleBitmap(pdc, recforshape.Width(), recforshape.Height());
		dcMem.CreateCompatibleDC(pdc);
		oldBitMap = dcMem.SelectObject(&bmp);
		old = dcMem.SelectObject(&pen);
		dcMem.SelectStockObject(NULL_BRUSH);
		dcMem.FillSolidRect(recforshape, RGB(0, 0, 0));

		drawScreen(recforshape, &dcMem);
		sp.getEle()[sp.getDashSeq()]->rotate(point);
		sp.DrawBegin(&dcMem);

		sp.getEle()[sp.getDashSeq()]->dashDraw(&dcMem);

		pdc->BitBlt(0, 0, recforshape.Width(), recforshape.Height(), &dcMem, 0, 0, SRCCOPY);

		dcMem.SelectObject(old);
		dcMem.SelectObject(oldBitMap);
		dcMem.DeleteDC();
		bmp.DeleteObject();
		ReleaseDC(pdc);
		break;
	}
	default:
		break;
	}
	CView::OnMouseMove(nFlags, point);
}


void CMy01View::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CView::OnRButtonDown(nFlags, point);
}


void CMy01View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	pdc = GetDC();
	old=pdc->SelectObject(&pen);
	switch (condition)
	{
	case 5: {
		pdc->MoveTo(Or);
		pdc->LineTo(point);
		class line* e = new class line(Or, point);
		sp.add(e);
		Or = point;
		break;
	}
	case 7: {
		pdc->MoveTo(point);
		Or.SetPoint((point.y + static_cast<double>(1 / kk) * point.x - ykb) / (kk + static_cast<double>(1 / kk)),
			(static_cast<double>(kk * point.y) + static_cast<double>(point.x) + static_cast<double>(ykb / kk)) / static_cast<double>(kk + 1 / kk));
		class line* e = new class line(point, Or);
		pdc->LineTo(Or);
		sp.add(e);
		break;
	}
	default:
		drawScreen(recforshape, pdc);
		pdc->SelectStockObject(NULL_BRUSH);
		sp.DrawBegin(pdc);
		break;
	}
	pdc->SelectObject(old);
	ReleaseDC(pdc);
	CView::OnRButtonUp(nFlags, point);
}


void CMy01View::OnFileSave()
{ 
	// TODO: 在此添加命令处理程序代码
	
	CString strFilter = TEXT("dat file (*.own)||");
	CFileDialog TmpDlg(FALSE, _T("own"), _T("默认文件名"),NULL, strFilter.GetBuffer(), this);

	if (TmpDlg.DoModal() == IDOK) {
		ss = TmpDlg.GetPathName();
	}
	else {
		ss = "";
	}
	if (sp.save()) {
	//	ss.Format(L"下次想要恢复时请点击工具栏里的对应图标，并选择文件，若文件未在弹出窗口中出现，请在输入栏里输入文件名即可");
		MessageBox(L"保存成功！", 0, MB_OK);
	}
}


void CMy01View::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码

	CString szFilter = TEXT("dat file (*.own)||");
	CFileDialog filepick(TRUE, 0, 0,NULL, szFilter.GetBuffer(), this);

	if (filepick.DoModal() == IDOK) {
		ss = filepick.GetPathName();
	}
	else {
		ss = "";
	}
	std::ifstream fin;
	fin.open(ss, std::ios::in);
	int loc[4]{ 0,0,0,0 };
	if (fin.is_open() == false) {
		MessageBoxW(L"打开失败！", L"错误", MB_OK | MB_ICONERROR);
		return;
	}
	int det;
	int cycle;
	fin >> cycle;
	sp.destr();
	while (cycle > 0) {
		fin >> det;
		switch (det)
		{
		case shapeDetect::curve: {
			for (int i = 0; i < 4; ++i)
				fin >> loc[i];
			recforshape.SetRect(loc[0], loc[1], loc[2], loc[3]);

			class curve* e = new class curve(recforshape);
			sp.add(e);
			break;
		}
		case shapeDetect::ellipse: {
			for (int i = 0; i < 4; ++i)
				fin >> loc[i];
			recforshape.SetRect(loc[0], loc[1], loc[2], loc[3]);
			my_ellipse* e = new my_ellipse(recforshape);
			sp.add(e);
			break;
		}
		case shapeDetect::line: {
			for (int i = 0; i < 4; ++i)
				fin >> loc[i];

			CPoint t_a(loc[0], loc[1]);
			CPoint t_b(loc[2], loc[3]);
			class line* e = new class line(t_a, t_b);
			sp.add(e);
			break;
		}
		case shapeDetect::rectangle: {
			for (int i = 0; i < 4; ++i)
				fin >> loc[i];
			recforshape.SetRect(loc[0], loc[1], loc[2], loc[3]);
			class rec* e = new class rec(recforshape);
			sp.add(e);
			break;
		}
		}
		--cycle;
	}
	fin.close();
	pdc = GetDC();
	GetClientRect(&recforshape);

	drawScreen(recforshape, pdc);
	pdc->SelectStockObject(NULL_BRUSH);
	pdc->SelectObject(&pen);
	sp.DrawBegin(pdc);
	ReleaseDC(pdc);

}



void CMy01View::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (condition)
	{
	case 7: {

		break;
	}
	default:
		break;
	}
	CView::OnLButtonDblClk(nFlags, point);
}


BOOL CMy01View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CView::OnEraseBkgnd(pDC);
}
