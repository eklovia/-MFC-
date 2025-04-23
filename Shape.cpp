#include "pch.h"
#include "Shape.h"
extern shape sp;
shape::shape()
{
	//contain = new shape * [1000];
}

shape::~shape()
{
	if (!this->contain.empty())
	{
		for (int i = 0; i < contain.size(); ++i) {
			delete this->contain[i];
		}
	}
	//delete[]this->contain;
}

bool shape::recy(CPoint& p)
{
	for (auto i = contain.begin(); i < contain.end(); ++i) {
		if ((*i)->judge(p)) {
			this->dashSeq = this->Seq = (UINT)(i - contain.begin());
			return true;
		}
	}
	return false;
}

bool shape::recySca(CPoint& p)
{
	for (auto i = contain.begin(); i < contain.end(); ++i) {
		if ((*i)->judgeVert(p)) {
			//this->Seq = (UINT)(i - contain.begin());
			return true;
		}
	}
	return false;
}

void shape::cross(){ 

	if (contain.size() == 1)return;
	double r1 = static_cast<double> (contain.back()->r);
	double r2, k, b, a, tb, c, y2_y1, x2_x1, x0 = static_cast<double>(contain.back()->O.x), y0 = static_cast<double>(contain.back()->O.y);
	
	if (contain.back()->attr == ellipse) {
		for (int i = contain.size() - 2; i >= 0; --i) {
			ccc = true;
			switch (contain[i]->attr)
			{
			case shapeDetect::ellipse: {
				r2 = static_cast<double>(contain[i]->r);
				x2_x1 = static_cast<double>(contain[i]->O.x - contain.back()->O.x);
				y2_y1 = static_cast<double>(contain[i]->O.y - contain.back()->O.y);

				k = -(x2_x1) / y2_y1;
				b = x2_x1 * static_cast<double>(contain[i]->O.x + contain.back()->O.x) / (2. * y2_y1)
					+ static_cast<double>(contain[i]->O.y + contain.back()->O.y) / 2.
					+ (r1 * r1 - r2 * r2) / (2. * y2_y1);
				ccc = false;
				togetPoint(hwnd, k, b, a, tb, c, x0, y0, r1, recforshape);
			}
									 break;
			case shapeDetect::line: {
				yma = max(contain[i]->b.y, contain[i]->a.y);
				ymi = min(contain[i]->b.y, contain[i]->a.y);
				xma = contain[i]->b.x;
				xmi = contain[i]->a.x;
				k = static_cast<double> (contain[i]->k);
				b = static_cast<double> (contain[i]->yb);
				togetPoint(hwnd, k, b, a, tb, c, x0, y0, r1, recforshape);
			}
								  break;
			default:
				break;
			}
		}
	}
	else if (contain.back()->attr == shapeDetect::line) {
		yma = max(contain.back()->b.y, contain.back()->a.y);
		ymi = min(contain.back()->b.y, contain.back()->a.y);
		xma = contain.back()->b.x;
		xmi = contain.back()->a.x;
		for (int i = contain.size() - 2; i>=0; --i) {
			switch (contain[i]->attr)
			{
			case shapeDetect::ellipse: {
				k = static_cast<double> (contain.back()->k);
				b = static_cast<double> (contain.back()->yb);
				
				togetPoint(hwnd, k, b, a, tb, c, x0, y0, r1, recforshape);
				break;
			}
			case shapeDetect::line: {
				k = static_cast<double> (contain.back()->k- contain[i]->k) ;
				b = static_cast<double> (contain[i]->yb - contain.back()->yb);
				x = b / k;
				y = static_cast<double>(contain.back()->k) * x + static_cast<double>(contain.back()->yb);
				
				if (x< contain[i]->a.x || x>contain[i]->b.x || y > yma || y < ymi)
					break;

				ss.Format(L"x=%lf,y=%lf", x - recforshape.Width() / 2., -(y - recforshape.Height() / 2.));
				MessageBoxW(hwnd, ss, L"相交坐标", MB_OK);
				break;
			}
			default:
				break;
			}
		}
	}
	return;
}

void shape::destr()
{
	contain.clear();
}

void shape::DrawBegin(CDC* pdc)
{

	for (int i = 0; i < contain.size(); ++i) {
		contain[i]->draw(pdc);
	}
}



bool shape::save()
{
	std::ofstream fout;
	fout.open(ss, std::ios::out);
	if (fout.is_open() == false) {
		MessageBoxW(hwnd,L"打开失败！", L"错误", MB_OK | MB_ICONERROR);
		return false;
	}
	fout << contain.size()<<' ';
	for (int i = 0; i < contain.size(); ++i) {
		switch (contain[i]->attr)
		{
		case shapeDetect::curve: {
			fout << contain[i]->attr<<' ';
			load(fout, contain[i]->O, contain[i]->r);
			break;
		}
		case shapeDetect::ellipse: {
			fout << contain[i]->attr<<' ';
			load(fout, contain[i]->O, contain[i]->r);
			break;
		}
		case shapeDetect::line: {
			fout << contain[i]->attr << ' ';
			fout << contain[i]->a.x << ' ';
			fout << contain[i]->a.y << ' ';
			fout << contain[i]->b.x << ' ';
			fout << contain[i]->b.y << ' ';
			break;
		}
		case shapeDetect::rectangle: {
			fout << contain[i]->attr << ' ';
			fout << (contain[i]->O.x - contain[i]->a.x) << ' ';
			fout << (contain[i]->O.y - contain[i]->a.y) << ' ';
			fout << (contain[i]->O.x + contain[i]->a.x) << ' ';
			fout << (contain[i]->O.y + contain[i]->a.y) << ' ';
			break;
		}
		default:
			break;
		}
	}
	fout.close();
	return true;
}



void drawScreen(CRect& rec, CDC* pDC) {

	CBrush bk;
	bk.CreateSolidBrush(RGB(0, 0, 0));
	pDC->SetBkColor(RGB(0, 0, 0));
	pDC->FillRect(&rec, &bk);

	CPen pen(PS_DASH, 1, RGB(255, 0, 0));

	auto old = pDC->SelectObject(&pen);

	pDC->MoveTo(rec.Width() / 2, 0);
	pDC->LineTo(rec.Width() / 2, rec.Height());
	pDC->MoveTo(0, rec.Height() / 2);
	pDC->LineTo(rec.Width(), rec.Height() / 2);
	pDC->SelectObject(old);
}

inline LONG C(LONG n, LONG i)
{
	if (i > n)return -1l;
	if (i > n / 2)i = n - i;
	if (i == 0)return 1L;
	if (i == 1)return n;
	static LONG acc, m, t;
	acc = 1; m = 1; t = n;
	for (; m < i; ++m) {
		n *= (t - m);
		acc *= m;
	}
	acc *= m;
	return n / acc;
}

void BezierNn(CPoint* const pArr, int n)
{
	double c;
	int i, j;
	LONG k, l;
	LONG* CTable = new LONG[n];
	for (i = 0; i < n; ++i)
		CTable[i] = C(n - 1, i);
	
	CPoint* tt = new CPoint[n];
	static bool ud;
	for (double t = 0.; t < 1.; t += .1) {
		for (i = 0; i < n; ++i) {
			tt[i].y = tt[i].x = 0;
		}

		for (i = 0; i < n; ++i) {
			for (j = 0; j < n; ++j) {
				c = (double)CTable[j] * pow(t, (double)j) * pow(1. - t, static_cast<double>(n - 1 - j));
				tt[i].x += pArr[j].x * c;
				tt[i].y += pArr[j].y * c;
			}
			if (t > .0)
			{
				pdc->MoveTo(k, l);
				pdc->LineTo(tt[i]);
				class line* e = new class line(CPoint(k, l), tt[i]);
				sp.add(e);
			}
			k = tt[i].x;
			l = tt[i].y;
		}
	}

	delete[]tt;
	delete[]CTable;
}
