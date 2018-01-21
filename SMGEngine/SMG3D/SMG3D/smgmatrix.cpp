#include "SMG3D.h"
using namespace glm;

SMGMatrix::SMGMatrix() :
	_11(m_m[0][0]),
	_12(m_m[0][1]),
	_13(m_m[0][2]),
	_14(m_m[0][3]),

	_21(m_m[1][0]),
	_22(m_m[1][1]),
	_23(m_m[1][2]),
	_24(m_m[1][3]),

	_31(m_m[2][0]),
	_32(m_m[2][1]),
	_33(m_m[2][2]),
	_34(m_m[2][3]),

	_41(m_m[3][0]),
	_42(m_m[3][1]),
	_43(m_m[3][2]),
	_44(m_m[3][3])
{
}

SMGMatrix::SMGMatrix(const mat4 & m) :
	_11(m_m[0][0]),
	_12(m_m[0][1]),
	_13(m_m[0][2]),
	_14(m_m[0][3]),

	_21(m_m[1][0]),
	_22(m_m[1][1]),
	_23(m_m[1][2]),
	_24(m_m[1][3]),

	_31(m_m[2][0]),
	_32(m_m[2][1]),
	_33(m_m[2][2]),
	_34(m_m[2][3]),

	_41(m_m[3][0]),
	_42(m_m[3][1]),
	_43(m_m[3][2]),
	_44(m_m[3][3])
{
	m_m = m;
}

SMGMatrix::SMGMatrix(const SMGMatrix &m) :
	_11(m_m[0][0]),
	_12(m_m[0][1]),
	_13(m_m[0][2]),
	_14(m_m[0][3]),

	_21(m_m[1][0]),
	_22(m_m[1][1]),
	_23(m_m[1][2]),
	_24(m_m[1][3]),

	_31(m_m[2][0]),
	_32(m_m[2][1]),
	_33(m_m[2][2]),
	_34(m_m[2][3]),

	_41(m_m[3][0]),
	_42(m_m[3][1]),
	_43(m_m[3][2]),
	_44(m_m[3][3])
{
	*this = m;
}

inline void SMGMatrix::Identity()
{
	m_m = mat4(1.0f);
}

inline void SMGMatrix::RotaX(float a)
{
	m_m = rotate(m_m, a, vec3(1.0f, 0.0f, 0.0f));
}

inline void SMGMatrix::RotaY(float a)
{
	m_m = rotate(m_m, a, vec3(0.0f, 1.0f, 0.0f));
}

inline void SMGMatrix::RotaZ(float a)
{
	m_m = rotate(m_m, a, vec3(0.0f, 0.0f, 1.0f));
}

inline void SMGMatrix::Rota(const SMGVector & vc)
{
	RotaX(vc.x); RotaY(vc.y); RotaZ(vc.z);
}

inline void SMGMatrix::Rota(float x, float y, float z)
{
	Rota(SMGVector(x, y, z));
}

inline void SMGMatrix::RotaArbi(const SMGVector & vcAxis, float a)
{
	SMGVector v = vcAxis;

	if (v.GetSqrLength() != 1.0f)
		v.Normalize();

	m_m = rotate(m_m, a, v.m_v);
}

inline void SMGMatrix::ApplyInverseRota(SMGVector * pvc)
{
	pvc->x = pvc->x * _11 + pvc->y * _12 + pvc->z * _13;
	pvc->y = pvc->x * _21 + pvc->y * _22 + pvc->z * _23;
	pvc->z = pvc->x * _31 + pvc->y * _32 + pvc->z * _33;
}

inline void SMGMatrix::Translate(float dx, float dy, float dz)
{
	_41 = dx;
	_42 = dy;
	_43 = dz;
}

inline void SMGMatrix::SetTranslation(SMGVector vc, bool EraseContent)
{
	if (EraseContent) Identity();
	_41 = vc.x;
	_42 = vc.y;
	_43 = vc.z;
}

inline SMGVector SMGMatrix::GetTranslation()
{
	return SMGVector(_41, _42, _43);
}

void SMGMatrix::TransposeOf(const SMGMatrix & m)
{
	m_m = transpose(m.m_m);
}

void SMGMatrix::InverseOf(const SMGMatrix & m)
{
	m_m = inverse(m.m_m);
}

void SMGMatrix::operator=(const SMGMatrix & m)
{
	_11 = m._11;
	_12 = m._12;
	_13 = m._13;
	_14 = m._14;

	_21 = m._21;
	_22 = m._22;
	_23 = m._23;
	_24 = m._24;

	_31 = m._31;
	_32 = m._32;
	_33 = m._33;
	_34 = m._34;

	_41 = m._41;
	_42 = m._42;
	_43 = m._43;
	_44 = m._44;
}

SMGMatrix SMGMatrix::operator*(const SMGMatrix & m) const
{
	return SMGMatrix(m_m*m.m_m);
}

SMGVector SMGMatrix::operator*(const SMGVector & vc) const
{
	vec3 v = (vec3)(m_m*vec4(vc.m_v, 1.0f));
	return SMGVector(v);
}


