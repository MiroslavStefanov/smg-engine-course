#include "SMG3D.h"
using namespace glm;

inline void SMGPlane::Set(const SMGVector & vcN, const SMGVector & vcP)
{
	m_fD = -(vcN * vcP);
	m_vcN = vcN;
	m_vcPoint = vcP;
}

inline void SMGPlane::Set(const SMGVector & vcN, const SMGVector & vcP, float fD)
{
	m_vcN = vcN;
	m_fD = fD;
	m_vcPoint = vcP;
}

inline void SMGPlane::Set(const SMGVector & v0, const SMGVector & v1, const SMGVector & v2)
{
	SMGVector vcEdge1 = v1 - v0;
	SMGVector vcEdge2 = v2 - v0;

	m_vcN.Cross(vcEdge1, vcEdge2);
	m_vcN.Normalize();
	m_fD = -(m_vcN * v0);
	m_vcPoint = v0;
}

inline float SMGPlane::Distance(const SMGVector & vcPoint)
{
	return (_fabs((m_vcN*vcPoint) - m_fD));
}

inline int SMGPlane::Classify(const SMGVector & vcPoint)
{
	float f = (vcPoint * m_vcN) + m_fD;

	if (f >  0.00001) return SMGFRONT;
	if (f < -0.00001) return SMGBACK;
	return SMGPLANAR;
}

int SMGPlane::Classify(const SMGPolygon & Polygon)
{
	int NumFront = 0, NumBack = 0, NumPlanar = 0;
	int nClass;

	// cast away const
	SMGPolygon *pPoly = ((SMGPolygon*)&Polygon);

	int NumPoints = pPoly->GetNumPoints();

	// loop through all points
	for (int i = 0; i < NumPoints; i++)
	{
		nClass = Classify(pPoly->m_pPoints[i]);

		if (nClass == SMGFRONT)     NumFront++;
		else if (nClass == SMGBACK) NumBack++;
		else
		{
			NumFront++;
			NumBack++;
			NumPlanar++;
		}
	} // for

	  // all points are planar
	if (NumPlanar == NumPoints)
		return SMGPLANAR;
	// all points are in front of plane
	else if (NumFront == NumPoints)
		return SMGFRONT;
	// all points are on backside of plane
	else if (NumBack == NumPoints)
		return SMGBACK;
	// poly is intersecting the plane
	else
		return SMGCLIPPED;
}

bool SMGPlane::Clip(const SMGRay *_pRay, float fL, SMGRay *pF, SMGRay *pB)
{
	SMGVector vcHit;

	SMGRay *pRay = (SMGRay*)_pRay;

	// ray intersects plane at all?
	if (!pRay->Intersects(*this, false, fL, NULL, &vcHit))
		return false;

	int n = Classify(_pRay->m_vcOrig);

	// ray comes fron planes backside
	if (n == SMGBACK)
	{
		if (pB) pB->Set(pRay->m_vcOrig, pRay->m_vcDir);
		if (pF) pF->Set(vcHit, pRay->m_vcDir);
	}
	// ray comes from planes front side
	else if (n == SMGFRONT)
	{
		if (pF) pF->Set(pRay->m_vcOrig, pRay->m_vcDir);
		if (pB) pB->Set(vcHit, pRay->m_vcDir);
	}

	return true;
}

bool SMGPlane::Intersects(const SMGPlane & plane, SMGRay * pIntersection)
{
	SMGVector vcCross;
	float     fSqrLength;

	// if crossproduct of normals 0 than planes parallel
	vcCross.Cross(this->m_vcN, plane.m_vcN);
	fSqrLength = vcCross.GetSqrLength();

	if (fSqrLength < 1e-08f)
		return false;

	// find line of intersection
	if (pIntersection)
	{
		float fN00 = this->m_vcN.GetSqrLength();
		float fN01 = this->m_vcN * plane.m_vcN;
		float fN11 = plane.m_vcN.GetSqrLength();
		float fDet = fN00*fN11 - fN01*fN01;

		if (_fabs(fDet) < 1e-08f)
			return false;

		float fInvDet = 1.0f / fDet;
		float fC0 = (fN11*this->m_fD - fN01*plane.m_fD) * fInvDet;
		float fC1 = (fN00*plane.m_fD - fN01*this->m_fD) * fInvDet;

		(*pIntersection).m_vcDir = vcCross;
		(*pIntersection).m_vcOrig = this->m_vcN*fC0 + plane.m_vcN*fC1;
	}

	return true;
}

bool SMGPlane::Intersects(const SMGVector & vc0, const SMGVector & vc1, const SMGVector & vc2)
{
	int n = this->Classify(vc0);

	if ((n == this->Classify(vc1)) &&
		(n == this->Classify(vc2)))
		return false;
	return true;
}

bool SMGPlane::Intersects(const SMGAabb & aabb)
{
	SMGVector Vmin, Vmax;

	// x component
	if (m_vcN.x >= 0.0f)
	{
		Vmin.x = aabb.vcMin.x;
		Vmax.x = aabb.vcMax.x;
	}
	else
	{
		Vmin.x = aabb.vcMax.x;
		Vmax.x = aabb.vcMin.x;
	}

	// y component
	if (m_vcN.y >= 0.0f)
	{
		Vmin.y = aabb.vcMin.y;
		Vmax.y = aabb.vcMax.y;
	}
	else
	{
		Vmin.y = aabb.vcMax.y;
		Vmax.y = aabb.vcMin.y;
	}

	// z component
	if (m_vcN.z >= 0.0f)
	{
		Vmin.z = aabb.vcMin.z;
		Vmax.z = aabb.vcMax.z;
	}
	else
	{
		Vmin.z = aabb.vcMax.z;
		Vmax.z = aabb.vcMin.z;
	}

	if (((m_vcN * Vmin) + m_fD) > 0.0f)
		return false;

	if (((m_vcN * Vmax) + m_fD) >= 0.0f)
		return true;

	return false;
}

bool SMGPlane::Intersects(const SMGObb & obb)
{
	float fRadius = _fabs(obb.fA0 * (m_vcN * obb.vcA0))
		+ _fabs(obb.fA1 * (m_vcN * obb.vcA1))
		+ _fabs(obb.fA2 * (m_vcN * obb.vcA2));

	float fDistance = this->Distance(obb.vcCenter);
	return (fDistance <= fRadius);
}