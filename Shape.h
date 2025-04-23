#pragma once
static double pi = acos(-1), x, y, yma, ymi, xma, xmi;
static bool ccc = false;
extern CString ss;
extern CDC* pdc;
extern bool a;
extern CRect recforshape;
extern HWND hwnd;
extern CPen* old;
extern CPen pen_x;
extern double dis, kk, ykb;
constexpr auto LCrossWise = true;
constexpr auto RCrossWise = false;
constexpr float M_PI = 3.14159265358979323846;   // pi;

enum shapeDetect
{
	ellipse,line,rectangle,curve
};

class shape {
public:

	
	shape();
	~shape();

	template<class T>
	void add(T ptr) {
		this->contain.push_back(ptr);
	}

	

	inline virtual void scale(CPoint& lat) {};
	inline virtual void draw(CDC* pdc) {};
	inline virtual void dashDraw(CDC* pdc) {};
	inline virtual void transform(CPoint&pre,CPoint&lat) {};
	inline virtual bool judgeVert(CPoint& p) { return false; };
	inline virtual bool judge(CPoint& p) { return false; };
	inline virtual void rotate(CPoint& p) {};
	inline virtual bool judgeHandle(CPoint& p) { return false; };

	inline UINT& getCurrentSelect() { return Seq; };
	inline UINT& getDashSeq() { return dashSeq; };
	void cross();
	void destr();
	void DrawBegin(CDC*pdc);
	bool save();
	bool recy(CPoint& p);
	bool recySca(CPoint& p);

	short attr = 0;
	std::vector<shape*>& getEle() { return this->contain; }
	CPoint O;
	CPoint a, b;//属于直线 
	float k, yb;
	int r;
private:
	std::vector<shape*>contain;
	UINT Seq = -1;
	UINT dashSeq = -1;
	//shape**  = nullptr;
};

void drawScreen(CRect& rec, CDC* pDC);

inline void togetPoint(HWND& hwnd, double& k, double& b, double& a, double& tb, double& c, double x0, double y0, double& r, CRect& rec) {
	a = 1.0 + k * k;
	tb = 2. * k * (b - y0) - 2 * x0;
	c = x0 * x0 + (b - y0) * (b - y0) - r * r;
	if ((tb * tb - 4. * a * c) < 0)return;
	if ((tb * tb - 4. * a * c) == .0)goto f;
	x0 = (-tb + sqrt(tb * tb - 4 * a * c)) / (2. * a);
	y0 = k * x0 + b;

	if (ccc && (x0< xmi || x0>xma || y0 > yma || y0 < ymi))
		goto f;
	ss.Format(L"x=%lf,y=%lf", x0 - rec.Width() / 2., -(y0 - rec.Height() / 2.));
	MessageBoxW(hwnd, ss, L"相交坐标", MB_OK);
f:
	x0 = (-tb - sqrt(tb * tb - 4 * a * c)) / (2. * a);
	y0 = k * x0 + b;
	if (ccc && (x0< xmi || x0>xma || y0 > yma || y0 < ymi))
		return;
	//ss.Format(L"x=%lf,y=%lf", (LONG)x0 - rec.CenterPoint().x, -((LONG)y0 - rec.CenterPoint().y));
	ss.Format(L"x=%lf,y=%lf", x0 - rec.Width() / 2., -(y0 - rec.Height() / 2.));

	MessageBoxW(hwnd, ss, L"相交坐标", MB_OK);
};

inline void load(std::ofstream& fout,CPoint&O,int&r) {
	fout << O.x - r<<' ';
	fout << O.y - r<<' ';
	fout << O.x + r<<' ';
	fout << O.y + r<<' ';
}

inline LONG C(LONG n, LONG i);

void BezierNn(CPoint* const pArr,int n);


class my_ellipse :public shape {
public:
	inline bool judge(CPoint& p) {
		if (abs((p.x - O.x) * (p.x - O.x) + (p.y - O.y) * (p.y - O.y) - r * r) <= 2000) {
			return true;
		}
		return false;
	}
	inline bool judgeVert(CPoint& p) {
		if (abs(p.x - O.x + r) < 10 && abs(p.y - O.y + r) < 10) { a.x = (O.x + r); a.y = (O.y + r); ::a = LCrossWise; return true; }
		if (abs(p.x - O.x - r) < 10 && abs(p.y - O.y - r) < 10) { a.x = (O.x - r); a.y = (O.y - r); ::a = LCrossWise; return true; }
		if (abs(p.x - O.x + r) < 10 && abs(p.y - O.y - r) < 10) { a.x = (O.x + r); a.y = (O.y - r); ::a = RCrossWise; return true; }
		if (abs(p.x - O.x - r) < 10 && abs(p.y - O.y + r) < 10) { a.x = (O.x - r); a.y = (O.y + r); ::a = RCrossWise; return true; }
		return false;
	}

	inline void scale(CPoint& lat) {
		if (::a)
		{
			r = (lat.x + lat.y - a.y - a.x) / 4;
			O.x = a.x + r;
			O.y = a.y + r;
		}
		else if (!::a) {
			r = (-a.x + a.y + lat.x - lat.y) / 4;
			O.x = a.x + r;
			O.y = a.y - r;
		}
	}

	inline void draw(CDC* pdc) {
		pdc->Ellipse(O.x - r, O.y - r, O.x + r, O.y + r);
	}

	inline void dashDraw(CDC* pdc) {
		old = pdc->SelectObject(&pen_x);
		pdc->SelectStockObject(NULL_BRUSH);
		pdc->Rectangle(O.x - r, O.y - r, O.x + r, O.y + r);
		pdc->SelectObject(old);
	}

	inline void transform(CPoint& pre, CPoint& lat) {
		O.x += (lat.x - pre.x);
		O.y += (lat.y - pre.y);
	}

	my_ellipse(const CRect& rec) {
		O.x = (rec.TopLeft().x + rec.BottomRight().x) / 2;
		O.y = (rec.TopLeft().y + rec.BottomRight().y) / 2;
		r = rec.Width() / 2;
		this->attr = shapeDetect::ellipse;

	}
};

class rec :public shape {
public:
	float  q, r, s, t;
	LONG u, v;

	LONG handlex,handley;
	bool judge(CPoint& p) {

		static float temp1;
		static float temp2;
		temp1 = fabs((float)(p.x - O.x) * cos(yb) + (float)(p.y - O.y) * sin(yb)) - (float)a.x;
		temp2 = fabs((float)(O.x - p.x) * sin(yb) + (float)(p.y - O.y) * cos(yb)) - (float)a.y;

		if (((fabs(temp1) <= 4.0f) && (temp2 >= -(float)(a.y << 1)) && (temp2 <= 0.f)) || ((fabs(temp2) <= 4.0f) && (temp1 >= (float)(-(a.x << 1))) && (temp1 <= 0.f))) {
			return true;
		}
		return false;
	}

	inline bool judgeVert(CPoint& p) {
		if (abs(p.x + q - t - O.x) < 10 && abs(p.y + r + s - O.y) < 10) { b.x = O.x + a.x; b.y = O.y + a.y; ::a = LCrossWise; return true; }
		if (abs(p.x - q + t - O.x) < 10 && abs(p.y - r - s - O.y) < 10) { b.x = O.x - a.x; b.y = O.y - a.y; ::a = LCrossWise; return true; }
		if (abs(p.x + q + t - O.x) < 10 && abs(p.y + r - s - O.y) < 10) { b.x = O.x + a.x; b.y = O.y - a.y; ::a = RCrossWise; return true; }
		if (abs(p.x - q - t - O.x) < 10 && abs(p.y - r + s - O.y) < 10) { b.x = O.x - a.x; b.y = O.y + a.y; ::a = RCrossWise; return true; }
		return false;
	}

	inline void transform(CPoint& pre, CPoint& lat) {
		O.x += (lat.x - pre.x);
		O.y += (lat.y - pre.y);
		handlex += (lat.x - pre.x);
		handley += (lat.y - pre.y);

	}

	inline void rotate(CPoint& p) {

		if ((p.y - O.y) == 0)
			yb = M_PI / 2.f;
		else
			yb = atan(static_cast<float>(O.x - p.x) / (p.y - O.y));

		q = a.x * cos(yb);
		r = a.x * sin(yb);

		s = a.y * cos(yb);
		t = a.y * sin(yb);

		handlex = -t + O.x - 50 * sin(yb);
		handley = s + 50 * cos(yb) + O.y;
	}

	inline bool judgeHandle(CPoint& p) {
		if (abs(p.x - handlex)<10&&abs(p.y-handley)<10)return true;
		return false;
	}

	inline void dashDraw(CDC* pdc) {
		old = pdc->SelectObject(&pen_x);

		pdc->MoveTo(t + O.x - q, O.y - r - s);
		pdc->LineTo(q + t + O.x, r - s + O.y);

		pdc->MoveTo(q + t + O.x, r - s + O.y);
		pdc->LineTo(q - t + O.x, r + s + O.y);

		pdc->MoveTo(q - t + O.x, r + s + O.y);
		pdc->LineTo(O.x - q - t, s - r + O.y);

		pdc->MoveTo(O.x - q - t, s + O.y - r);
		pdc->LineTo(t + O.x - q, O.y - r - s);

		pdc->MoveTo(O.x - t, s + O.y);

		pdc->LineTo(handlex, handley);

		pdc->SelectObject(old);
	}

	rec(const CRect& rec) {
		this->O.x = (rec.TopLeft().x + rec.BottomRight().x) >>1;
		this->O.y = (rec.TopLeft().y + rec.BottomRight().y) >> 1;
		this->a.x = rec.Width() >> 1;
		this->a.y = rec.Height() >> 1;
		attr = shapeDetect::rectangle;
		k = (static_cast<float>(rec.TopLeft().y) - rec.BottomRight().y) / (static_cast<float>(rec.TopLeft().x - rec.BottomRight().x));
		yb = .0f;
		handlex = O.x;
		handley = rec.BottomRight().y + 50;
		q = a.x, r = 0.f, s = a.y, t = 0.f;
	}

	inline void draw(CDC* pdc) {

		pdc->MoveTo(t + O.x - q, O.y - r - s);
		pdc->LineTo(q + t + O.x, r - s + O.y);

		pdc->MoveTo(q + t + O.x, r - s + O.y);
		pdc->LineTo(q - t + O.x, r + s + O.y);

		pdc->MoveTo(q - t + O.x, r + s + O.y);
		pdc->LineTo(O.x - q - t, s + O.y - r);

		pdc->MoveTo(O.x - q - t, s + O.y - r);
		pdc->LineTo(t + O.x - q, O.y - r - s);
	}

	inline void scale(CPoint& lat) {
		
		u = (lat.x - O.x) * cos(yb) + (lat.y - O.y) * sin(yb);
		v = (lat.y - O.y) * cos(yb) - (lat.x - O.x) * sin(yb);
		
		if (::a)
			k = fabs(k);
		else if (!::a)
			k = -fabs(k);

		a.x = (u + v * k) / (k * k + 1.f);
		a.y = k * a.x;
		O.x = b.x + a.x;
		O.y = b.y + a.y;

		a.x = abs(a.x);
		a.y = abs(a.y);

		

		q = a.x * cos(yb);
		r = a.x * sin(yb);

		s = a.y * cos(yb);
		t = a.y * sin(yb);

		handlex = -t + O.x - 50 * sin(yb);
		handley = s + 50 * cos(yb) + O.y;
		
		
	}

};

class line :public shape {
public:

	line(const CPoint& a, const CPoint& b) {
		this->a = a;
		this->b = b;
		if (this->a.x > this->b.x) {
			this->a.x ^= this->b.x ^= this->a.x ^= this->b.x;
			this->a.y ^= this->b.y ^= this->a.y ^= this->b.y;
		}
		this->k = static_cast<float>(a.y - b.y) / static_cast<float>(a.x - b.x);
		yb = static_cast<float>(a.y) - this->k * static_cast<float>(a.x);
		attr = shapeDetect::line;
	}
	inline bool judge(CPoint& p) {

		if (fabs(static_cast<float>(p.y) - static_cast<float>(this->k * p.x) - static_cast<float>(yb)) <= 4.) {

			if (p.x<this->a.x || p.x>this->b.x)
				return false;
			kk = this->k;
			ykb = this->yb;
			return true;
		}
		return false;
	}

	inline void transform(CPoint& pre, CPoint& lat) {
		a.x += (lat.x - pre.x);
		a.y += (lat.y - pre.y);
		b.x += (lat.x - pre.x);
		b.y += (lat.y - pre.y);
		yb = a.y - k * a.x;

	}

	inline void dashDraw(CDC* pdc) {
		old = pdc->SelectObject(&pen_x);
		pdc->MoveTo(a);
		pdc->LineTo(b);
		pdc->SelectObject(old);
	}

	inline void draw(CDC* pdc) {
		pdc->MoveTo(a);
		pdc->LineTo(b);
	}

	inline bool judgeVert(CPoint& p) {
		if (abs(p.x - a.x) < 10 && abs(p.y - a.y) < 10)return true;
		if (abs(p.x - b.x) < 10 && abs(p.y - b.y) < 10)return true;
		return false;
	}

};

class curve :public shape {
public:
	curve(CRect& rec) {
		attr = shapeDetect::curve;
		r = rec.Width() / 2;

		O.x = (rec.TopLeft().x + rec.BottomRight().x) / 2;
		O.y = (rec.TopLeft().y + rec.BottomRight().y) / 2;
	}

	inline bool judge(CPoint& p) {
		if ((p.y <= O.y) && (abs((p.x - O.x) * (p.x - O.x) + (p.y - O.y) * (p.y - O.y) - r * r) <= 2000)) {
			return true;
		}
		return false;
	}

	inline void dashDraw(CDC* pdc) {
		old = pdc->SelectObject(&pen_x);
		this->draw(pdc);
		pdc->SelectObject(old);
	}

	inline void transform(CPoint& pre, CPoint& lat) {
		O.x += (lat.x - pre.x);
		O.y += (lat.y - pre.y);
	}

	inline void draw(CDC* pdc) {
		pdc->Arc(O.x - r / 2, O.y - r / 2, O.x + r / 2, O.y + r / 2, O.x + r, O.y, O.x - r, O.y);
	}
};
