#include "SMG3D.h"
using namespace glm;

void SMGQuat::MakeFromEuler(float fPitch, float fYaw, float fRoll)
{
	m_q = quat(vec3(fPitch, fYaw, fRoll));
}

void SMGQuat::Normalize()
{
	normalize(m_q);
}

void SMGQuat::Conjugate(SMGQuat q)
{
	conjugate(m_q);
}

void SMGQuat::GetEulers(float * fPitch, float * fYaw, float * fRoll)
{
	vec3 eulers = eulerAngles(m_q);
	*fPitch = eulers.x;
	*fYaw = eulers.y;
	*fRoll = eulers.z;
}

SMGMatrix SMGQuat::GetMatrix()
{
	mat4 mat = toMat4(m_q);
	return SMGMatrix(mat);
}

float SMGQuat::GetMagnitude()
{
	return length(m_q);
}

void SMGQuat::operator/=(float f)
{
	m_q /= f;
}

SMGQuat SMGQuat::operator/(float f)
{
	return SMGQuat(m_q / f);
}

void SMGQuat::operator*=(float f)
{
	m_q *= f;
}

SMGQuat SMGQuat::operator*(float f)
{
	return SMGQuat(m_q * f);
}

SMGQuat SMGQuat::operator*(const SMGVector & v) const
{
	return SMGQuat(m_q * v.m_v);
}

SMGQuat SMGQuat::operator*(const SMGQuat & q) const
{
	return SMGQuat(m_q * q.m_q);
}

void SMGQuat::operator*=(const SMGQuat & q)
{
	m_q *= q.m_q;
}

void SMGQuat::operator+=(const SMGQuat & q)
{
	m_q += q.m_q;
}

SMGQuat SMGQuat::operator+(const SMGQuat & q) const
{
	return SMGQuat(m_q + q.m_q);
}

void SMGQuat::operator=(const SMGQuat & q)
{
	m_q = q.m_q;
}

void SMGQuat::Rotate(const SMGQuat & q1, const SMGQuat & q2)
{
	SMGQuat t = q1*q2*(~q1);
	*this = t;
}

SMGVector SMGQuat::Rotate(const SMGVector & v)
{
	vec3 vec = rotate(m_q, v.m_v);
	return SMGVector(vec);
}