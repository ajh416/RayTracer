#pragma once

#include <ostream>

template<typename T>
class Vector2
{
public:
	//Vector2() { x = y = 0; }
	//Vector2(T xx, T yy) : x(xx), y(yy) { ASSERT(!HasNaNs()); }
	//Vector2(T&& xx, T&& yy) : x(xx), y(yy) { ASSERT(!HasNaNs()); }
	//Vector2(std::initializer_list<T> list) { auto it = list.begin(); x = *it; ++it; y = *it; ASSERT(!HasNaNs()); }

	constexpr Vector2() { x = y = 0; }
	constexpr Vector2(T xx) : x(xx), y(xx) {}
	constexpr Vector2(T&& xx, T&& yy) : x(xx), y(yy) {}
	constexpr Vector2(std::initializer_list<T> list) { auto it = list.begin(); x = *it; ++it; y = *it; }

	bool HasNaNs() const
	{
		return std::isnan(x) || std::isnan(y);
	}

	constexpr Vector2<T> operator+(const Vector2<T>& v)
	{
		return Vector2(x + v.x, y + v.y);
	}

	constexpr Vector2<T> operator+=(const Vector2<T>& v)
	{
		return Vector2(x += v.x, y += v.y);
	}

	constexpr Vector2<T> operator-(const Vector2<T>& v)
	{
		return Vector2(x - v.x, y - v.y);
	}

	constexpr Vector2<T> operator-=(const Vector2<T>& v)
	{
		return Vector2(x -= v.x, y -= v.y);
	}

	constexpr bool operator==(const Vector2<T>& v)
	{
		return x == v.x && y == v.y;
	}

	constexpr bool operator!=(const Vector2<T>& v)
	{
		return x != v.x || y != v.y;
	}

	constexpr Vector2<T> operator*(T&& f) const { return Vector2<T>(f * x, f * y); }

	constexpr Vector2<T>& operator*=(T f) {
		x *= f; y *= f;
		return *this;
	}
	Vector2<T> operator/(T f) const {
		ASSERT(f != 0);
		float inv = (float)1 / f;
		return Vector2<T>(x * inv, y * inv);
	}

	Vector2<T>& operator/=(T f) {
		ASSERT(f != 0);
		float inv = (float)1 / f;
		x *= inv; y *= inv;
		return *this;
	}

	constexpr Vector2<T> operator-() const { return Vector2<T>(-x, -y); }

	constexpr float LengthSquared() const { return x * x + y * y; }
	float Length() const { return std::sqrt(LengthSquared()); }

	T x, y;
};

template<typename T>
class Vector3
{
public:
	//Vector3() { x = y = z = 0; }
	//Vector3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { ASSERT(!HasNaNs()); }
	//Vector3(T xx) : x(xx), y(xx), z(xx) { ASSERT(!HasNaNs()); }
	constexpr Vector3() { x = y = z = 0; }
	constexpr Vector3(T&& xx) : x(xx), y(xx), z(xx) { }
	constexpr Vector3(T&& xx, T&& yy, T&& zz) : x(xx), y(yy), z(zz) { }

	~Vector3() = default;

	bool HasNaNs() const
	{
		return std::isnan(x) || std::isnan(y) || std::isnan(z);
	}

	constexpr Vector3<T> operator+(const Vector3<T>& v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	constexpr Vector3<T>& operator+=(const Vector3<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	constexpr Vector3<T> operator-(const Vector3<T>& v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	constexpr Vector3<T>& operator-=(const Vector3<T>& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
		//return Vector3(x -= v.x, y -= v.y, z -= v.z);
	}

	constexpr bool operator==(const Vector3<T>& v)
	{
		return x == v.x && y == v.y && z == v.z;
	}

	constexpr bool operator!=(const Vector3<T>& v)
	{
		return x != v.x || y != v.y || z != v.z;
	}

	constexpr bool operator>(const Vector3<T>& v)
	{
		return (x > v.x && y > v.y && z > v.y);
	}

	constexpr Vector3<T> operator*(const Vector3<T> f) const { return Vector3<T>(f.x * x, f.y * y, f.z * z); }

	constexpr Vector3<T> operator*(const T t) const { return Vector3<T>(x * t, y * t, z * t); }

	constexpr Vector3<T>& operator*=(T f) {
		x *= f; y *= f; z *= f;
		return *this;
	}

	constexpr Vector3<T> operator/(T f) const {
		ASSERT(f != 0);
		float inv = (float)1 / f;
		return Vector3<T>(x * inv, y * inv, z * inv);
	}

	constexpr Vector3<T>& operator/=(T f) {
		ASSERT(f != 0);
		float inv = (float)1 / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}

	constexpr Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }

	constexpr float LengthSquared() const { return x * x + y * y + z * z; }
	float Length() const { return std::sqrt(LengthSquared()); }

	T x, y, z;
};

template<typename T, typename U>
constexpr inline Vector3<T> operator*(U t, const Vector3<T> v) {
	return v * t;
}

template<typename T, typename U>
constexpr inline Vector3<T> operator*(const Vector3<T> v, U t) {
	return v * t;
}

template<typename T, typename U>
constexpr inline Vector3<T> operator+(const Vector3<T> v1, const Vector3<U> v2) {
	return Vector3<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<typename T, typename U>
constexpr inline Vector3<T> operator+(const Vector3<T> v, const U other) {
	return Vector3<T>(v.x + other, v.y + other, v.z + other);
}

template<typename T, typename U>
constexpr inline Vector3<T> operator-(const Vector3<T> v1, const Vector3<U> v2) {
	return Vector3<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template <typename T>
constexpr inline T Dot(const Vector3<T>& v1, const Vector3<T>& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T>
constexpr inline T AbsDot(const Vector3<T>& v1, const Vector3<T>& v2) {
	return std::abs(Dot(v1, v2));
}

template <typename T>
constexpr inline Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2) {
	float v1x = v1.x, v1y = v1.y, v1z = v1.z;
	float v2x = v2.x, v2y = v2.y, v2z = v2.z;
	return Vector3<T>((v1y * v2z) - (v1z * v2y),
		(v1z * v2x) - (v1x * v2z),
		(v1x * v2y) - (v1y * v2x));
}

template <typename T>
constexpr inline Vector3<T> Normalize(const Vector3<T>& v) { return v * (1 / v.Length()); }

template <typename T>
inline Vector3<T> Normalize(const Vector3<T>&& v) { return v / v.Length(); }

template<typename T>
inline Vector3<T> Unit(const Vector3<T>& v) { Normalize(v); }

// I - 2.0 * dot(N, I) * N
template<typename T>
inline Vector3<T> Reflect(const Vector3<T>& I, const Vector3<T>& N)
{
	return I - 2.0f * Dot(N, I) * N;
}

// Vector2 ostream operator
template<typename T>
std::ostream& operator<<(std::ostream& os, Vector2<T> vec)
{
	return os << "(" << vec.x << ", " << vec.y << ")";
}

// Vector3 ostream operator
template<typename T>
std::ostream& operator<<(std::ostream& os, Vector3<T> vec)
{
	return os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}

namespace Utils
{
	template<typename T>
	constexpr inline Vector3<T> Lerp(const Vector3<T>& v1, const Vector3<T>& v2, const float t)
	{
		return (1.0f - t) * v1 + t * v2;
	}

	template<typename T>
	constexpr inline Vector3<T> Clamp(const Vector3<T> vec, const Vector3<T> min, const Vector3<T> max)
	{
		if (vec.x < min.x || vec.y < min.y || vec.z < min.z)
			return min;
		if (vec.x > max.x || vec.y > max.y || vec.z > max.z)
			return max;

		return vec;
	}

	inline Vec3f RandomVector(float low, float high)
	{
		return Vec3f(Utils::Random(low, high), Utils::Random(low, high), Utils::Random(low, high));
	}

	// assumes that vec is float
	template<typename T>
	constexpr inline Vector3<uint8_t> Vec3floatToVec3Byte(const Vector3<T>& vec)
	{
		Vector3<uint8_t> newVec;
		newVec.x = static_cast<uint8_t>(255.0 * vec.x);
		newVec.y = static_cast<uint8_t>(255.0 * vec.y);
		newVec.z = static_cast<uint8_t>(255.0 * vec.z);
		return newVec;
	}

	constexpr inline uint32_t VectorToUInt32(Vector3<float> vec)
	{
		auto byte_vec = Vec3floatToVec3Byte(vec);

		return 0xff000000 | (byte_vec.z << 16) | (byte_vec.y << 8) | (byte_vec.x);
	}
}