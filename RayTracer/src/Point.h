#pragma once

#include "RayTracer.h"

template<typename T>
class Point2
{
public:
	Point2() { x = y = 0; }
	Point2(T xx, T yy) : x(xx), y(yy) { ASSERT(!HasNaNs()); }

	bool HasNaNs() const
	{
		return std::isnan(x) || std::isnan(y);
	}

	Point2<T> operator+(const Vector2<T>& v) const
	{
		Assert(!v.HasNaNs());
		return Point2<T>(x + v.x, y + v.y);
	}

	Point2<T>& operator+=(const Vector2<T>& v)
	{
		Assert(!v.HasNaNs());
		x += v.x; y += v.y;
		return *this;
	}

	Vector2<T> operator-(const Point2<T>& p) const
	{
		Assert(!p.HasNaNs());
		return Vector2<T>(x - p.x, y - p.y);
	}

	Point2<T> operator-(const Vector2<T>& v) const
	{
		Assert(!v.HasNaNs());
		return Point2<T>(x - v.x, y - v.y);
	}

	Point2<T> operator-() const { return Point2<T>(-x, -y); }

	Point2<T>& operator-=(const Vector2<T>& v)
	{
		Assert(!v.HasNaNs());
		x -= v.x; y -= v.y;
		return *this;
	}

	Point2<T>& operator+=(const Point2<T>& p)
	{
		Assert(!p.HasNaNs());
		x += p.x; y += p.y;
		return *this;
	}

	Point2<T> operator+(const Point2<T>& p) const
	{
		Assert(!p.HasNaNs());
		return Point2<T>(x + p.x, y + p.y);
	}

	Point2<T> operator* (T f) const
	{
		return Point2<T>(f * x, f * y);
	}

	Point2<T>& operator*=(T f)
	{
		x *= f; y *= f;
		return *this;
	}

	Point2<T> operator/ (T f) const
	{
		float inv = (float)1 / f;
		return Point2<T>(inv * x, inv * y);
	}

	Point2<T>& operator/=(T f)
	{
		float inv = (float)1 / f;
		x *= inv; y *= inv;
		return *this;
	}

	bool operator==(const Point2<T>& p) const
	{
		return x == p.x && y == p.y;
	}

	bool operator!=(const Point2<T>& p) const
	{
		return x != p.x || y != p.y;
	}

	Point2<T> operator-()
	{
		return Point2<T>(-x, -y);
	}

	T x, y;
};

template<typename T>
class Point3
{
public:
	Point3() { x = y = z = 0; }
	Point3(T xx) : x(xx), y(xx), z(xx) {}
	Point3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { ASSERT(!HasNaNs()); }

	bool HasNaNs() const
	{
		return std::isnan(x) || std::isnan(y) || std::isnan(z);
	}

	Point3<T> operator+(const Vector3<T>& v) const
	{
		return Point3<T>(x + v.x, y + v.y, z + v.z);
	}

	Point3<T>& operator+=(const Vector3<T>& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	Vector3<T> operator-(const Point3<T>& p) const
	{
		return Vector3<T>(x - p.x, y - p.y, z - p.z);
	}

	Point3<T> operator-(const Vector3<T>& v) const
	{
		return Point3<T>(x - v.x, y - v.y, z - v.z);
	}

	Point3<T>& operator-=(const Vector3<T>& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	Point3<T>& operator+=(const Point3<T>& p)
	{
		x += p.x; y += p.y; z += p.z;
		return *this;
	}

	Point3<T> operator+(const Point3<T>& p) const
	{
		return Point3<T>(x + p.x, y + p.y, z + p.z);
	}

	Point3<T> operator*(T f) const
	{
		return Point3<T>(f * x, f * y, f * z);
	}

	Point3<T>& operator*=(T f)
	{
		x *= f; y *= f; z *= f;
		return *this;
	}

	Point3<T> operator/(T f) const
	{
		float inv = (float)1 / f;
		return Point3<T>(inv * x, inv * y, inv * z);
	}

	Point3<T>& operator/=(T f)
	{
		float inv = (float)1 / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}

	bool operator==(const Point3<T>& p) const
	{
		return x == p.x && y == p.y && z == p.z;
	}

	bool operator!=(const Point3<T>& p) const
	{
		return x != p.x || y != p.y || z != p.z;
	}

	inline float Distance(const Point3<T>& p1, const Point3<T>& p2)
	{
		return (p1 - p2).Length();
	}

	inline float DistanceSquared(const Point3<T>& p1, const Point3<T>& p2)
	{
		return pow((p1 - p2).Length(), 2);
	}

	Point3<T> Lerp(float t, const Point3<T>& p0, const Point3<T>& p1)
	{
		return (1 - t) * p0 + t * p1;
	}

	Point3<T> Min(const Point3<T>& p1, const Point3<T>& p2)
	{
		return Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
			std::min(p1.z, p2.z));
	}

	Point3<T> Max(const Point3<T>& p1, const Point3<T>& p2)
	{
		return Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
			std::max(p1.z, p2.z));
	}

	Point3<T> Floor(const Point3<T>& p)
	{
		return Point3<T>(std::floor(p.x), std::floor(p.y), std::floor(p.z));
	}

	Point3<T> Ceil(const Point3<T>& p)
	{
		return Point3<T>(std::ceil(p.x), std::ceil(p.y), std::ceil(p.z));
	}

	Point3<T> Abs(const Point3<T>& p)
	{
		return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
	}

	Point3<T> Permute(const Point3<T>& p, int x, int y, int z)
	{
		return Point3<T>(p[x], p[y], p[z]);
	}

	Point3<T> operator-() const { return Point3<T>(-x, -y, -z); }

	T x, y, z;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Point2<T>& p)
{
	return os << "(" << p.x << ", " << p.y << ")";
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Point3<T>& p)
{
	return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}
