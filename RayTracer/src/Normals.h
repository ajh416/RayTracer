#pragma once

template<typename T>
class Normal3
{
public:
	Normal3() { x = y = z = 0; }
	Normal3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

	bool HasNaNs() const
	{
		return std::isnan(x) || std::isnan(y) || std::isnan(z);
	}

	Normal3<T> operator-() const
	{
		return Normal3(-x, -y, -z);
	}

	Normal3<T> operator+(const Normal3<T>& n) const
	{
		return Normal3<T>(x + n.x, y + n.y, z + n.z);
	}

	Normal3<T>& operator+=(const Normal3<T>& n)
	{
		x += n.x; y += n.y; z += n.z;
		return *this;
	}

	Normal3<T> operator-(const Normal3<T>& n) const
	{
		return Normal3<T>(x - n.x, y - n.y, z - n.z);
	}

	Normal3<T>& operator-=(const Normal3<T>& n)
	{
		x -= n.x; y -= n.y; z -= n.z;
		return *this;
	}

	Normal3<T> operator*(T f) const {
		return Normal3<T>(f * x, f * y, f * z);
	}

	Normal3<T>& operator*=(T f) {
		x *= f; y *= f; z *= f;
		return *this;
	}
	Normal3<T> operator/(T f) const {
		Assert(f != 0);
		Float inv = (Float)1 / f;
		return Normal3<T>(x * inv, y * inv, z * inv);
	}

	Normal3<T>& operator/=(T f) {
		Assert(f != 0);
		Float inv = (Float)1 / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	Float LengthSquared() const { return x * x + y * y + z * z; }
	Float Length() const { return std::sqrt(LengthSquared()); }

	T x, y, z;
};