#include "SMG3D.h"
using namespace glm;

float _fabs(float f) { if (f<0.0f) return -f; return f; }

SMGVector::SMGVector(const SMGVector &v) : x(m_v.x), y(m_v.y), z(m_v.z)
{
	x = v.x; y = v.y; z = v.z;
}

inline void SMGVector::Set(float _x, float _y, float _z)
{
	x = _x; y = _y; z = _z;
}

inline float SMGVector::GetLength()
{
	return length(m_v);
}

inline float SMGVector::GetSqrLength() const
{
	return x*x + y*y + z*z;
}

inline void SMGVector::Negate()
{
	x = -x; y = -y; z = -z;
}

inline void SMGVector::Normalize()
{
	m_v = normalize(m_v);
}

inline float SMGVector::AngleWith(SMGVector & v)
{
	return (float)acos(((*this) * v) / (this->GetLength()*v.GetLength()));
}

inline void SMGVector::RotateWith(const SMGMatrix &m)
{
	m_v = (mat3)m.m_m * m_v;
}

inline void SMGVector::InvRotateWith(const SMGMatrix &m)
{
	m_v = transpose((mat3)m.m_m) * m_v;
}

void SMGVector::operator=(const SMGVector & v)
{
	x = v.x; y = v.y; z = v.z;
}

void SMGVector::operator+=(const SMGVector & v)
{
	m_v += v.m_v;
}

void SMGVector::operator-=(const SMGVector & v)
{
	m_v -= v.m_v;
}

void SMGVector::operator*=(float f)
{
	m_v *= f;
}

void SMGVector::operator/=(float f)
{
	m_v /= f;
}

void SMGVector::operator+=(float f)
{
	m_v += f;
}

void SMGVector::operator-=(float f)
{
	m_v -= f;
}

float SMGVector::operator*(const SMGVector & v) const
{
	return dot(m_v, v.m_v);
}

SMGVector SMGVector::operator*(float f) const
{
	return SMGVector(x*f, y*f, z*f);
}

SMGVector SMGVector::operator/(float f) const
{
	return SMGVector(x/f, y/f, z/f);
}

SMGVector SMGVector::operator+(float f) const
{
	return SMGVector(x+f, y+f, z+f);
}

SMGVector SMGVector::operator-(float f) const
{
	return SMGVector(x-f, y-f, z-f);
}

SMGQuat SMGVector::operator*(const SMGQuat & q) const
{
	return SMGQuat(q.w*x + q.z*y - q.y*z,
				   q.w*y + q.x*z - q.z*x,
				   q.w*z + q.y*x - q.x*y,
				   -(q.x*x + q.y*y + q.z*z));
}

SMGVector SMGVector::operator*(const SMGMatrix & m) const
{
	vec4 v4 = vec4(m_v, 1.0f);
	v4 = m.m_m * v4;
	return SMGVector(v4.x, v4.y, v4.z);
}

SMGVector SMGVector::operator+(const SMGVector & v) const
{
	vec3 v1 = m_v + v.m_v;
	return SMGVector(v1.x, v1.y, v1.z);
}

SMGVector SMGVector::operator-(const SMGVector & v) const
{
	vec3 v1 = m_v - v.m_v;
	return SMGVector(v1.x, v1.y, v1.z);
}

void SMGVector::Cross(const SMGVector & v1, const SMGVector & v2)
{
	m_v = cross(v1.m_v, v2.m_v);
}
