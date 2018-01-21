#include "SMG3D.h"
using namespace std;

SMGAabb::SMGAabb(SMGVector _vcMin, SMGVector _vcMax)
{
	vcMin = _vcMin;
	vcMax = _vcMax;
	vcCenter = (vcMax - vcMin) / 2.0f;
}

void SMGAabb::Construct(const SMGObb * pObb)
{
	SMGVector vcA0, vcA1, vcA2;
	SMGVector _vcMax, _vcMin;

	vcA0 = pObb->vcA0 * pObb->fA0;
	vcA1 = pObb->vcA1 * pObb->fA1;
	vcA2 = pObb->vcA2 * pObb->fA2;

	// find x extensions
	if (vcA0.x > vcA1.x)
	{
		if (vcA0.x > vcA2.x)
		{
			vcMax.x = vcA0.x;
			vcMin.x = -vcA0.x;
		}
		else
		{
			vcMax.x = vcA2.x;
			vcMin.x = -vcA2.x;
		}
	}
	else
	{
		if (vcA1.x > vcA2.x)
		{
			vcMax.x = vcA1.x;
			vcMin.x = -vcA1.x;
		}
		else
		{
			vcMax.x = vcA2.x;
			vcMin.x = -vcA2.x;
		}
	}

	// find y extensions
	if (vcA0.y > vcA1.y)
	{
		if (vcA0.y > vcA2.y)
		{
			vcMax.y = vcA0.y;
			vcMin.y = -vcA0.y;
		}
		else
		{
			vcMax.y = vcA2.y;
			vcMin.y = -vcA2.y;
		}
	}
	else
	{
		if (vcA1.y > vcA2.y)
		{
			vcMax.y = vcA1.y;
			vcMin.y = -vcA1.y;
		}
		else
		{
			vcMax.y = vcA2.y;
			vcMin.y = -vcA2.y;
		}
	}

	// find z extensions
	if (vcA0.z > vcA1.z)
	{
		if (vcA0.z > vcA2.z)
		{
			vcMax.z = vcA0.z;
			vcMin.z = -vcA0.z;
		}
		else
		{
			vcMax.z = vcA2.z;
			vcMin.z = -vcA2.z;
		}
	}
	else
	{
		if (vcA1.z > vcA2.z)
		{
			vcMax.z = vcA1.z;
			vcMin.z = -vcA1.z;
		}
		else
		{
			vcMax.z = vcA2.z;
			vcMin.z = -vcA2.z;
		}
	}
}

int SMGAabb::Cull(const SMGPlane * pPlanes, int nNumPlanes)
{
	SMGVector  vcMin, vcMax;
	bool       bIntersects = false;

	// find and test extreme points
	for (int i = 0; i<nNumPlanes; i++)
	{
		// x coordinate
		if (pPlanes[i].m_vcN.x >= 0.0f)
		{
			vcMin.x = this->vcMin.x;
			vcMax.x = this->vcMax.x;
		}
		else
		{
			vcMin.x = this->vcMax.x;
			vcMax.x = this->vcMin.x;
		}
		// y coordinate
		if (pPlanes[i].m_vcN.y >= 0.0f)
		{
			vcMin.y = this->vcMin.y;
			vcMax.y = this->vcMax.y;
		}
		else
		{
			vcMin.y = this->vcMax.y;
			vcMax.y = this->vcMin.y;
		}
		// z coordinate
		if (pPlanes[i].m_vcN.z >= 0.0f)
		{
			vcMin.z = this->vcMin.z;
			vcMax.z = this->vcMax.z;
		}
		else
		{
			vcMin.z = this->vcMax.z;
			vcMax.z = this->vcMin.z;
		}

		if (((pPlanes[i].m_vcN*vcMin) + pPlanes[i].m_fD) > 0.0f)
			return SMGCULLED;

		if (((pPlanes[i].m_vcN*vcMax) + pPlanes[i].m_fD) >= 0.0f)
			bIntersects = true;
	} // for

	if (bIntersects) return SMGCLIPPED;
	return SMGVISIBLE;
}

bool SMGAabb::Intersects(const SMGRay & Ray, float * t)
{
	SMGRay *r = (SMGRay*)&Ray;
	return r->Intersects(*this, t);
}

bool SMGAabb::Intersects(const SMGRay & Ray, float fL, float * t)
{
	SMGRay *r = (SMGRay*)&Ray;
	return r->Intersects(*this, fL, t);
}

bool SMGAabb::Intersects(const SMGAabb & aabb)
{
	if ((vcMin.x > aabb.vcMax.x) || (aabb.vcMin.x > vcMax.x))
		return false;
	if ((vcMin.y > aabb.vcMax.y) || (aabb.vcMin.y > vcMax.y))
		return false;
	if ((vcMin.z > aabb.vcMax.z) || (aabb.vcMin.z > vcMax.z))
		return false;
	return true;
}

bool SMGAabb::Intersects(const SMGVector & vc)
{
	if (vc.x > vcMax.x) return false;
	if (vc.y > vcMax.y) return false;
	if (vc.z > vcMax.z) return false;
	if (vc.x < vcMin.x) return false;
	if (vc.y < vcMin.y) return false;
	if (vc.z < vcMin.z) return false;
	return true;
}

bool SMGAabb::Contains(const SMGRay & Ray, float fL)
{
	SMGVector vcEnd = Ray.m_vcOrig + (Ray.m_vcDir*fL);
	return (Intersects(Ray.m_vcOrig) &&
			Intersects(vcEnd));
}

void SMGAabb::GetPlanes(SMGPlane * pPlanes)
{
	SMGVector vcN;

	if (!pPlanes) return;

	// right side
	vcN.Set(1.0f, 0.0f, 0.0f);
	pPlanes[0].Set(vcN, vcMax);

	// left side
	vcN.Set(-1.0f, 0.0f, 0.0f);
	pPlanes[1].Set(vcN, vcMin);

	// front side
	vcN.Set(0.0f, 0.0f, -1.0f);
	pPlanes[2].Set(vcN, vcMin);

	// back side
	vcN.Set(0.0f, 0.0f, 1.0f);
	pPlanes[3].Set(vcN, vcMax);

	// top side
	vcN.Set(0.0f, 1.0f, 0.0f);
	pPlanes[4].Set(vcN, vcMax);

	// bottom side
	vcN.Set(0.0f, -1.0f, 0.0f);
	pPlanes[5].Set(vcN, vcMin);
}