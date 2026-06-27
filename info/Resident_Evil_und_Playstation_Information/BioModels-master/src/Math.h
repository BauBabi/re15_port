#ifndef MATH_H
#define MATH_H

#include "BioModels.h"

struct Vec2U16
{
	union
	{
		u16 x, u;
	};
	union
	{
		u16 y, v;
	};
};

struct Vec2
{
	union
	{
		float x, u;
	};
	union
	{
		float y, v;
	};
};

struct Vec3S16
{
	s16 x, y, z;
};

struct Vec3
{
	float x, y, z;

	inline Vec3 cross(const Vec3& vec) const
	{
		return Vec3{
			y * vec.z - z * vec.y,
			z * vec.x - x * vec.z,
			x * vec.y - y * vec.x
		};
	}

	inline Vec3 norm() const
	{
		float m = length();
		if (m == 0)
		{
			return Vec3{ 0, 0, 0 };
		}
		return Vec3{ x / m, y / m, z / m };
	}

	inline float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	inline float dot(const Vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	inline Vec3 operator-(const Vec3& rhs) const
	{
		return Vec3{ x - rhs.x, y - rhs.y, z - rhs.z };
	}

	inline Vec3 operator+(const Vec3& rhs) const
	{
		return Vec3{ x + rhs.x, y + rhs.y, z + rhs.z };
	}

	inline void operator+=(const Vec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
	}

	inline void operator-=(const Vec3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
	}

	inline Vec3 operator*(const float scalar) const
	{
		return Vec3{ x * scalar, y * scalar , z * scalar };
	}
};

struct Mat4
{
	float
		m00, m01, m02, m03,
		m10, m11, m12, m13,
		m20, m21, m22, m23,
		m30, m31, m32, m33;

	Mat4() = default;

	Mat4(float identity) :
		m00(identity), m01(0), m02(0), m03(0),
		m10(0), m11(identity), m12(0), m13(0),
		m20(0), m21(0), m22(identity), m23(0),
		m30(0), m31(0), m32(0), m33(identity) {
	}

	Mat4 operator*(const Mat4& rhs) const
	{
		Mat4 res{};

		// Column 0
		res.m00 = m00 * rhs.m00 + m10 * rhs.m01 + m20 * rhs.m02 + m30 * rhs.m03;
		res.m01 = m01 * rhs.m00 + m11 * rhs.m01 + m21 * rhs.m02 + m31 * rhs.m03;
		res.m02 = m02 * rhs.m00 + m12 * rhs.m01 + m22 * rhs.m02 + m32 * rhs.m03;
		res.m03 = m03 * rhs.m00 + m13 * rhs.m01 + m23 * rhs.m02 + m33 * rhs.m03;

		// Column 1
		res.m10 = m00 * rhs.m10 + m10 * rhs.m11 + m20 * rhs.m12 + m30 * rhs.m13;
		res.m11 = m01 * rhs.m10 + m11 * rhs.m11 + m21 * rhs.m12 + m31 * rhs.m13;
		res.m12 = m02 * rhs.m10 + m12 * rhs.m11 + m22 * rhs.m12 + m32 * rhs.m13;
		res.m13 = m03 * rhs.m10 + m13 * rhs.m11 + m23 * rhs.m12 + m33 * rhs.m13;

		// Column 2
		res.m20 = m00 * rhs.m20 + m10 * rhs.m21 + m20 * rhs.m22 + m30 * rhs.m23;
		res.m21 = m01 * rhs.m20 + m11 * rhs.m21 + m21 * rhs.m22 + m31 * rhs.m23;
		res.m22 = m02 * rhs.m20 + m12 * rhs.m21 + m22 * rhs.m22 + m32 * rhs.m23;
		res.m23 = m03 * rhs.m20 + m13 * rhs.m21 + m23 * rhs.m22 + m33 * rhs.m23;

		// Column 3
		res.m30 = m00 * rhs.m30 + m10 * rhs.m31 + m20 * rhs.m32 + m30 * rhs.m33;
		res.m31 = m01 * rhs.m30 + m11 * rhs.m31 + m21 * rhs.m32 + m31 * rhs.m33;
		res.m32 = m02 * rhs.m30 + m12 * rhs.m31 + m22 * rhs.m32 + m32 * rhs.m33;
		res.m33 = m03 * rhs.m30 + m13 * rhs.m31 + m23 * rhs.m32 + m33 * rhs.m33;

		return res;
	}
};

inline Mat4 Translate(Mat4& mat4, const Vec3& translation)
{
	mat4.m30 += translation.x;
	mat4.m31 += translation.y;
	mat4.m32 += translation.z;

	return mat4;
}

inline Mat4 Scale(Mat4& mat4, const Vec3& scale)
{
	Mat4 scaleMat{ 1.0f };

	scaleMat.m00 = scale.x;
	scaleMat.m11 = scale.y;
	scaleMat.m22 = scale.z;

	return mat4 * scaleMat;
}

inline Mat4 Rotate(const Mat4& mat, float angle, const Vec3& rot)
{
	float c = cosf(angle);
	float s = sinf(angle);

	Vec3 norm = rot.norm();

	float x = norm.x;
	float y = norm.y;
	float z = norm.z;

	Mat4 rotation{ 0 };

	rotation.m00 = (1 - c) * (x * x) + c;
	rotation.m01 = (1 - c) * x * y + s * z;
	rotation.m02 = (1 - c) * x * z - s * y;

	rotation.m10 = (1 - c) * x * y - s * z;
	rotation.m11 = (1 - c) * (y * y) + c;
	rotation.m12 = (1 - c) * y * z + s * x;

	rotation.m20 = (1 - c) * x * z + s * y;
	rotation.m21 = (1 - c) * y * z - s * x;
	rotation.m22 = (1 - c) * (z * z) + c;

	rotation.m33 = 1.0f;

	return mat * rotation;
}

inline Mat4 PerspectiveRH(float fov, float aspect, float zNear, float zFar)
{
	Mat4 result{};

	float cotan = 1.0f / tanf(fov / 2.0f);

	result.m00 = cotan / aspect;
	result.m11 = cotan;
	result.m22 = (zNear + zFar) / (zNear - zFar);
	result.m23 = -1.0f;
	result.m32 = (2.0f * zNear * zFar) / (zNear - zFar);

	return result;
}

inline Mat4 LookAtRH(const Vec3& eye, const Vec3& center, const Vec3& worldUp)
{
	Vec3 forward = (center - eye).norm();
	Vec3 side = (forward.cross(worldUp)).norm();
	Vec3 up = side.cross(forward);

	Mat4 result{};

	result.m00 = side.x;
	result.m10 = side.y;
	result.m20 = side.z;

	result.m01 = up.x;
	result.m11 = up.y;
	result.m21 = up.z;

	result.m02 = -forward.x;
	result.m12 = -forward.y;
	result.m22 = -forward.z;

	result.m30 = -side.dot(eye);
	result.m31 = -up.dot(eye);
	result.m32 = forward.dot(eye);
	result.m33 = 1.0f;

	return result;
}

#endif