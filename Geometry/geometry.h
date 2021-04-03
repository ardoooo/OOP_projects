#pragma once

#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>

using namespace std;

double eps = 1e-8;
double pi = acos(-1.0);


struct Point {
	double x, y;
	Point(double x, double y) : x(x), y(y) {};
	Point() : x(0), y(0) {};
	~Point() {};
	void rotate(Point center, double angle);
	void reflex(Point a);
	void scale(Point center, double coefficient);
};
bool operator==(const Point& a, const Point& b) {
	return abs(a.x - b.x) < eps && abs(a.y - b.y) < eps;
}
bool operator!=(const Point& a, const Point& b) {
	return !(b == a);
}
void Point::rotate(Point center, double angle) {
	angle *= pi / 180;
	double x1, y1;
	x1 = (x - center.x) * cos(angle) - (y - center.y) * sin(angle) + center.x;
	y1 = (x - center.x) * sin(angle) + (y - center.y) * cos(angle) + center.y;
	if (abs(x1) < eps) x1 = 0;
	if (abs(y1) < eps) y1 = 0;
	x = x1;
	y = y1;
}
void Point::reflex(Point a) {
	x = 2 * a.x - x;
	y = 2 * a.y - y;

}
void Point::scale(Point center, double coefficient) {
	Point radius_vector1(x - center.x, y - center.y);
	radius_vector1.x *= coefficient;
	radius_vector1.y *= coefficient;
	x = center.x + radius_vector1.x;
	y = center.y + radius_vector1.y;
}
double dist(const Point a, const Point b) {
	return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}


class Line {
private:
	double A, B, C;
public:
	Line(double k, double t) : A(-k), B(1), C(-t) {};
	Line(const Point& a, const Point& b) : A(b.y - a.y), B(a.x - b.x), C(a.x* (a.y - b.y) + a.y * (b.x - a.x)) {};
	Line(const Point& a, double k) : A(-k), B(1), C(k* a.x - a.y) {};

	double first_coeff() const {
		return A;
	}
	double second_coeff() const {
		return B;
	}
	double third_coeff() const {
		return C;
	}
	friend bool operator==(const Line& l1, const Line& l2) {
		return abs(l1.A / l2.A - l1.B / l2.B) < eps && abs(l1.C / l2.C - l1.B / l2.B);
	}
	void reflex(Point& a);
};

bool operator!=(const Line& l1, const Line& l2) {
	return !(l1 == l2);
}
void Line::reflex(Point& a) {
	double x1 = B * B * a.x - A * B * a.y - A * C;
	x1 /= B * B + A * A;
	double y1 = A * A * a.y - A * B * a.x - B * C;
	y1 /= B * B + A * A;
	if (abs(x1) < eps) x1 = 0;
	if (abs(y1) < eps) y1 = 0;
	a.x = 2 * x1 - a.x;
	a.y = 2 * y1 - a.y;
}


class Shape {
public:
	virtual ~Shape() = default;

	virtual bool isCongruentTo(const Shape& another) const = 0;
	virtual bool isSimilarTo(const Shape& another) const = 0;
	virtual bool operator==(const Shape& another) const = 0;
	virtual bool operator!=(const Shape& another) const = 0;

	virtual bool containsPoint(Point point) const = 0;

	virtual void rotate(Point center, double angle) = 0;
	virtual void reflex(Point center) = 0;
	virtual void reflex(Line axis) = 0;
	virtual void scale(Point center, double coefficient) = 0;

	virtual double perimeter() const = 0;
	virtual double area() const = 0;
};

class Polygon : public Shape {
public:
	std::vector<Point> polygon;
	Polygon() {};
	Polygon(std::vector<Point>& points) {
		polygon = std::move(points);
	}
	Polygon(std::initializer_list<Point> points) {
		polygon = points;
	}
	bool isCongruentTo(const Shape& another) const override;
	bool isSimilarTo(const Shape& another) const override;

	bool operator==(const Shape& another) const override;
	bool operator!=(const Shape& another) const override;

	int verticesCount() const;
	std::vector<Point> getVertices() const;
	bool isConvex() const;

	bool containsPoint(Point point) const override;

	void rotate(Point center, double angle) override;
	void reflex(Point a) override;
	void reflex(Line axis) override;
	void scale(Point center, double coefficient) override;

	double area() const override;
	double perimeter() const override;
};

bool Polygon::isCongruentTo(const Shape& another) const {
	auto other = dynamic_cast<const Polygon*>(&another);
	if (!other) return false;
	if (other->polygon.size() != polygon.size()) return false;
	double a = dist(polygon[1], polygon[0]);
	for (size_t i = 0; i < polygon.size(); ++i) {
		if (i == polygon.size() - 1) return false;
		if (abs(a - dist(other->polygon[i], other->polygon[i + 1])) < eps) {
			size_t sz = polygon.size();
			bool t = true;
			for (size_t j = 0; j < polygon.size(); ++j) {
				double side = dist(polygon[(j + 1) % sz], polygon[j]);
				double side_other = dist(other->polygon[(i + j + 1) % sz], other->polygon[(j + i) % sz]);
				if (abs(side - side_other) >= eps) {
					t = false;
					break;
				}
			}
			if (t) return true;
			t = true;
			for (size_t j = 0; j < polygon.size(); ++j) {
				double side = dist(polygon[(sz - j + 1) % sz], polygon[(sz - j) % sz]);
				double side_other = dist(other->polygon[(i + j + 1) % sz], other->polygon[(j + i) % sz]);
				if (abs(side - side_other) >= eps) {
					t = false;
					break;
				}
			}
			if (t) return true;
		}
	}

	return false;
}
bool Polygon::operator==(const Shape& another) const {
	if (!isCongruentTo(another)) return false;
	auto other = dynamic_cast<const Polygon*>(&another);
	if (!other) return false;
	if (polygon.size() == 4) {
		return true;
	}
	bool a = false, b = false, c = false;
	for (size_t i = 0; i < polygon.size(); ++i) {
		if (polygon[0] == other->polygon[i]) a = true;
		if (polygon[1] == other->polygon[i]) b = true;
		if (polygon[2] == other->polygon[i]) c = true;
	}

	return a && b && c;
}
bool Polygon::operator!=(const Shape& another) const {
	return !(*this == another);
}
bool Polygon::isSimilarTo(const Shape& another) const {
	const Polygon* other = dynamic_cast<const Polygon*>(&another);
	if (other == NULL) return false;
	if (other->polygon.size() != polygon.size()) return false;
	double k = other->perimeter() / perimeter();
	double k1 = other->area() / area();
	if (abs(k * k - k1) < eps) return true;
	return false;
}
int Polygon::verticesCount() const {
	return polygon.size();
}
std::vector<Point> Polygon::getVertices() const {
	return polygon;
}
bool Polygon::isConvex() const {
	bool product = 0;
	{
		Point ab(polygon[0].x - polygon.back().x, polygon[0].y - polygon.back().y);
		Point bc(polygon[1].x - polygon[0].x, polygon[1].y - polygon[0].y);
		product = (ab.x * bc.y - ab.y * bc.x) > 0;
	}

	for (size_t i = 1; i < polygon.size() - 1; ++i) {
		Point ab(polygon[i].x - polygon[i - 1].x, polygon[i].y - polygon[i - 1].y);
		Point bc(polygon[i + 1].x - polygon[i].x, polygon[i + 1].y - polygon[i].y);
		if (product != ((ab.x * bc.y - ab.y * bc.x) > 0)) return false;
	}
	Point ab(polygon.back().x - polygon[polygon.size() - 2].x, polygon.back().y - polygon[polygon.size() - 2].y);
	Point bc(polygon[0].x - polygon.back().x, polygon[0].y - polygon.back().y);
	if (product != ((ab.x * bc.y - ab.y * bc.x) > 0)) return false;

	return true;
}
bool Polygon::containsPoint(Point point) const {
	double y = point.y;
	double x = point.x;
	size_t sz = polygon.size();
	size_t count = 0;
	for (size_t i = 0; i < polygon.size(); ++i) {
		if (polygon[i] == point) return true;
		if ((y - polygon[i].y) * (y - polygon[(i + 1) % sz].y) > 0) continue;
		Line l(polygon[i], polygon[(i + 1) % sz]);
		if (l.first_coeff() == 0) {
			if (abs(x - polygon[i].x) < eps && x >= std::min(polygon[i].x, polygon[(i + 1) % sz].x)) {
				++count;
			}
			continue;
		}
		double tmp = (-l.second_coeff() * y - l.third_coeff()) / l.first_coeff();
		if (abs(x - tmp) < eps) return true;
		if (tmp > x) continue;
		++count;
	}
	if (count % 2 == 1) return true;
	return false;
}
void Polygon::rotate(Point center, double angle) {
	for (size_t i = 0; i < polygon.size(); ++i) {
		polygon[i].rotate(center, angle);
	}
}
void Polygon::reflex(Point a) {
	for (size_t i = 0; i < polygon.size(); ++i) {
		polygon[i].reflex(a);
	}
}
void Polygon::reflex(Line axis) {
	for (size_t i = 0; i < polygon.size(); ++i) {
		axis.reflex(polygon[i]);
	}
}
void Polygon::scale(Point center, double coefficient) {
	for (size_t i = 0; i < polygon.size(); ++i) {
		polygon[i].scale(center, coefficient);
	}
}
double Polygon::area() const {
	double sum = 0;
	for (size_t i = 0; i < polygon.size() - 1; ++i) {
		sum += polygon[i].x * polygon[i + 1].y;
	}
	for (size_t i = 0; i < polygon.size() - 1; ++i) {
		sum -= polygon[i + 1].x * polygon[i].y;
	}
	sum += polygon.back().x * polygon[0].y;
	sum -= polygon.back().y * polygon[0].x;
	sum /= 2;
	return abs(sum);
}
double Polygon::perimeter() const {
	double p = 0;
	for (size_t i = 0; i < polygon.size() - 1; ++i) {
		p += sqrt((polygon[i].x - polygon[i + 1].x) * (polygon[i].x - polygon[i + 1].x) + (polygon[i].y - polygon[i + 1].y) * (polygon[i].y - polygon[i + 1].y));
	}
	p += sqrt((polygon[0].x - polygon.back().x) * (polygon[0].x - polygon.back().x) + (polygon[0].y - polygon.back().y) * (polygon[0].y - polygon.back().y));
	return p;
}

class Ellipse : public Shape {
public:
	Point f1, f2;
	double distance;
	double a, b, c, e; //ïîëóîñè è ýêñöåíòðèñèòåò

	Ellipse(const Point& f1, const Point& f2, double distance);
	Ellipse();
	~Ellipse() {}

	bool isCongruentTo(const Shape& another) const override;
	bool isSimilarTo(const Shape& another) const override;
	bool operator==(const Shape& another) const override;
	bool operator!=(const Shape& another) const override;

	bool containsPoint(Point point) const override;

	std::pair<Point, Point> focuses() const;
	double eccentricity() const;
	std::pair<Line, Line> directrices() const;
	Point center() const;

	void rotate(Point center, double angle) override;
	void reflex(Point a) override;
	void reflex(Line axis) override;
	void scale(Point center, double coefficient) override;

	double area() const override;
	double perimeter() const override;
};

Ellipse::Ellipse(const Point& f1, const Point& f2, double distance) : f1(f1), f2(f2), distance(distance) {
	c = sqrt((f1.x - f2.x) * (f1.x - f2.x) + (f1.y - f2.y) * (f1.y - f2.y)) / 2;
	a = distance / 2;
	b = sqrt(a * a - c * c);
	e = c / a;
}
Ellipse::Ellipse() : f1({ 0,0 }), f2({ 0, 0 }), distance(0), a(0), b(0), c(0), e(0) {};
bool Ellipse::isCongruentTo(const Shape& another) const {
	auto other = dynamic_cast<const Ellipse*>(&another);
	if (!other) return false;
	if (abs(c - other->c) < eps && abs(distance - other->distance) < eps) return true;
	return false;
}
bool Ellipse::operator==(const Shape& another) const {
	auto other = dynamic_cast<const Ellipse*>(&another);
	if (!other) return false;
	if (f1 == other->f1 && f2 == other->f2 && abs(other->distance - distance) < eps) return true;
	if (f2 == other->f2 && f2 == other->f1 && abs(other->distance == distance) < eps) return true;
	return false;
}
bool Ellipse::operator!=(const Shape& another) const {
	return !(*this == another);
}
bool Ellipse::isSimilarTo(const Shape& another) const {
	const Ellipse* other = dynamic_cast<const Ellipse*>(&another);
	if (!other) return false;
	return (abs(e - other->e) < eps);
}
bool Ellipse::containsPoint(Point point) const {
	if (dist(point, f1) + dist(point, f2) > distance) return false;
	return true;
}
std::pair<Point, Point> Ellipse::focuses() const {
	return { f1, f2 };
}
double Ellipse::eccentricity() const {
	return e;
}
std::pair<Line, Line> Ellipse::directrices() const {
	Line l1({ 0, 0 }, { 0, 0 }), l2({ 0, 0 }, { 0, 0 });
	return { l1, l2 };
}
Point Ellipse::center() const {
	return { (f1.x + f2.x) / 2, (f1.y + f2.y) / 2 };
}
void Ellipse::rotate(Point center, double angle) {
	f1.rotate(center, angle);
	f2.rotate(center, angle);
}
void Ellipse::reflex(Point a) {
	f1.reflex(a);
	f2.reflex(a);
}
void Ellipse::reflex(Line axis) {
	axis.reflex(f1);
	axis.reflex(f2);
}
void Ellipse::scale(Point center, double coefficient) {
	f1.scale(center, coefficient);
	f2.scale(center, coefficient);
}
double Ellipse::area() const {
	return pi * a * b;
}
double Ellipse::perimeter() const {
	return pi * (3 * (a + b) - sqrt((3 * a + b) * (a + 3 * b)));
}
class Circle : public Ellipse {
public:
	Circle(Point center, double radius) : Ellipse(center, center, 2 * radius) {}
	~Circle() {};
	double radius() const {
		return distance / 2;
	}
	double perimeter() const override {
		return pi * distance;
	}
};

class Rectangle : public Polygon {
public:
	Rectangle(Point a, Point b, double c);
	~Rectangle() {};

	Point center() const;
	std::pair<Line, Line> diagonals() const;
};


Rectangle::Rectangle(Point a, Point b, double c) {
	double ac = sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
	double ab = 1;
	double ad = c;
	ab *= sqrt(ac * ac / (1 + c * c));
	ad *= sqrt(ac * ac / (1 + c * c));
	if (ab > ad) std::swap(ab, ad);
	polygon.push_back({ a.x, a.y });
	polygon.push_back({ a.x, a.y + ab });
	polygon.push_back({ a.x + ad, a.y + ab });
	polygon.push_back({ a.x + ad, a.y });
	double bb1 = sqrt((a.x + ad - b.x) * (a.x + ad - b.x) + (a.y + ab - b.y) * (a.y + ab - b.y));
	bb1 /= 2;
	double angle = 2 * asin(bb1 / ac) * 180 / pi;
	for (size_t i = 0; i < 4; ++i) {
		double x, y;
		x = (polygon[i].x - a.x) * cos(angle) - (polygon[i].y - a.y) * sin(angle) + a.x;
		y = (polygon[i].x - a.x) * sin(angle) + (polygon[i].y - a.y) * cos(angle) + a.y;
		if (abs(x) < eps) x = 0;
		if (abs(y) < eps) y = 0;
		polygon[i].x = x;
		polygon[i].y = y;
	}
}
Point Rectangle::center() const {
	return { polygon[0].x + polygon[2].x, polygon[0].y + polygon[2].y };
}
std::pair<Line, Line> Rectangle::diagonals() const {
	Line l1(polygon[0], polygon[2]);
	Line l2(polygon[1], polygon[3]);
	return { l1, l2 };
}
class Square : public Rectangle {
public:
	Square(Point a, Point b) : Rectangle(a, b, 1) {};
	~Square() {};

	Circle circumscribedCircle() const;
	Circle inscribedCircle() const;
};


Circle Square::circumscribedCircle() const {
	double r = (center().x - polygon[0].x) * (center().x - polygon[0].x) + (center().y - polygon[0].y) * (center().y - polygon[0].y);
	return Circle(center(), r);
}
Circle Square::inscribedCircle() const {
	double d = (polygon[1].x - polygon[0].x) * (polygon[1].x - polygon[0].x) + (polygon[1].y - polygon[0].y) * (polygon[1].y - polygon[0].y);
	return Circle(center(), d / 2);
}


class Triangle : public Polygon {
public:
	Triangle(std::initializer_list<Point> lst);
	Triangle(Point a, Point b, Point c);
	~Triangle() = default;

	Circle circumscribedCircle() const;
	Circle inscribedCircle() const;
	Point centroid() const;
	Point orthocenter() const;
	Line EulerLine() const;
	Circle ninePointsCircle() const;
};

Triangle::Triangle(std::initializer_list<Point> lst) : Polygon(lst) {}

Triangle::Triangle(Point a, Point b, Point c) {
	polygon.push_back(a);
	polygon.push_back(b);
	polygon.push_back(c);
}
Circle Triangle::circumscribedCircle() const {
	double ox, oy;
	double r = 1, d = 0;
	ox = (polygon[0].x * polygon[0].x + polygon[0].y * polygon[0].y) * (polygon[1].y - polygon[2].y);
	ox += (polygon[1].x * polygon[1].x + polygon[1].y * polygon[1].y) * (polygon[2].y - polygon[0].y);
	ox += (polygon[2].x * polygon[2].x + polygon[2].y * polygon[2].y) * (polygon[0].y - polygon[1].y);
	oy = (polygon[0].x * polygon[0].x + polygon[0].y * polygon[0].y) * (polygon[2].x - polygon[1].x);
	oy += (polygon[1].x * polygon[1].x + polygon[1].y * polygon[1].y) * (polygon[0].x - polygon[2].x);
	oy += (polygon[2].x * polygon[2].x + polygon[2].y * polygon[2].y) * (polygon[1].x - polygon[0].x);
	d = 2 * (polygon[0].x * (polygon[1].y - polygon[2].y) + polygon[1].x * (polygon[2].y - polygon[0].y) + polygon[2].x * (polygon[0].y - polygon[1].y));
	ox /= d;
	oy /= d;
	Point o(ox, oy);
	r = dist(polygon[0], o);
	return Circle(o, r);
}
Circle Triangle::inscribedCircle() const {
	double r = 2 * area() / perimeter();
	double ox, oy, a, b, c;
	a = dist(polygon[1], polygon[2]);
	b = dist(polygon[0], polygon[2]);
	c = dist(polygon[0], polygon[1]);
	ox = (a * polygon[0].x + b * polygon[1].x + c * polygon[2].x) / (a + b + c);
	oy = (a * polygon[0].y + b * polygon[1].y + c * polygon[2].y) / (a + b + c);
	Point o(ox, oy);
	return Circle(o, r);
}
Point Triangle::centroid() const {
	double ox, oy;
	ox = (polygon[0].x + polygon[1].x + polygon[2].x) / 3;
	oy = (polygon[0].y + polygon[1].y + polygon[2].y) / 3;
	return Point(ox, oy);
}
Point Triangle::orthocenter() const {
	Point m, o;
	m = centroid();
	o = circumscribedCircle().f1;
	o.x = 2 * m.x - o.x;
	o.y = 2 * m.y - o.y;
	m.x = 2 * o.x - m.x;
	m.y = 2 * o.y - m.y;
	return m;
}
Line Triangle::EulerLine() const {
	return Line(circumscribedCircle().f1, centroid());
}
Circle Triangle::ninePointsCircle() const {
	Point ma, mb, mc;
	ma = { (polygon[0].x + polygon[1].x) / 2, (polygon[0].y + polygon[1].y) / 2 };
	mb = { (polygon[0].x + polygon[2].x) / 2, (polygon[0].y + polygon[2].y) / 2 };
	mc = { (polygon[2].x + polygon[1].x) / 2, (polygon[2].y + polygon[1].y) / 2 };
	Triangle abc(ma, mb, mc);
	return abc.circumscribedCircle();
}
