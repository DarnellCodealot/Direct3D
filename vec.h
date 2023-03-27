#pragma once

template <typename T>
class Vec2 {
public:
	Vec2(T x, T y) : x(x), y(y) {}
	T x, y;
};

template <typename T>
class Vec3 {
public:
	Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
	Vec3(Vec2<T> a, T z) : x(a.x), y(a.y), z(z) {}
	Vec3 operator+(Vec3 rhs) {
		return Vec3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
	}
	T x, y, z;
};