#include "SMG3D.h"
using namespace glm;

SMGPolygon::SMGPolygon()
{
	m_pPoints = NULL;
	m_pIndis = NULL;
	m_NumP = 0;
	m_NumI = 0;
	m_Flag = 0;
	memset(&m_Aabb, 0, sizeof(SMGAabb));
}

SMGPolygon::SMGPolygon(const SMGPolygon & p)
{
	m_Plane.Set(p.m_Plane.m_vcN, p.m_Plane.m_vcPoint);
	m_NumI = p.m_NumI;
	m_NumP = p.m_NumP;
	m_Flag = p.m_Flag;
	memcpy(&m_Aabb, &(p.m_Aabb), sizeof(SMGAabb));
	memcpy(m_pPoints, p.m_pPoints, m_NumP*sizeof(SMGVector));
	memcpy(m_pIndis, p.m_pIndis, m_NumI * sizeof(unsigned int));
}

SMGPolygon::~SMGPolygon()
{
	if (m_pPoints)
	{
		delete[] m_pPoints;
		m_pPoints = NULL;
	}

	if (m_pIndis)
	{
		delete[] m_pIndis;
		m_pIndis = NULL;
	}
}

void SMGPolygon::Set(const SMGVector * pPoints, int nNumP, const unsigned int * pIndis, int nNumI)
{
	SMGVector vcEdge0,
		vcEdge1;

	bool bGotEm = false;

	if (m_pPoints)
	{
		delete[] m_pPoints;
		m_pPoints = NULL;
	}

	if (m_pIndis)
	{
		delete[] m_pIndis;
		m_pIndis = NULL;
	}

	m_pPoints = new SMGVector[nNumP];
	m_pIndis = new unsigned int[nNumI];

	m_NumP = nNumP;
	m_NumI = nNumI;

	memcpy(m_pPoints, pPoints, sizeof(SMGVector)*nNumP);
	memcpy(m_pIndis, pIndis, sizeof(unsigned int)*nNumI);

	vcEdge0 = m_pPoints[m_pIndis[1]] - m_pPoints[m_pIndis[0]];

	// calculate its plane
	for (int i = 2; bGotEm == false; i++)
	{

		if ((i + 1) > m_NumI) break;

		vcEdge1 = m_pPoints[m_pIndis[i]] - m_pPoints[m_pIndis[0]];

		vcEdge0.Normalize();
		vcEdge1.Normalize();

		// edges must not be parallel
		if (vcEdge0.AngleWith(vcEdge1) != 0.0)
			bGotEm = true;
	} // for

	m_Plane.m_vcN.Cross(vcEdge0, vcEdge1);
	m_Plane.m_vcN.Normalize();
	m_Plane.m_fD = -(m_Plane.m_vcN * m_pPoints[0]);
	m_Plane.m_vcPoint = m_pPoints[0];

	CalcBoundingBox();
}

void SMGPolygon::CalcBoundingBox()
{
	SMGVector vcMax, vcMin;

	// pick random start value from poly
	vcMin = m_pPoints[0];
	vcMax = m_pPoints[0];

	// seek final values for each coordinate
	for (int i = 0; i<m_NumP; i++)
	{
		if (m_pPoints[i].x > vcMax.x)
			vcMax.x = m_pPoints[i].x;
		else if (m_pPoints[i].x < vcMin.x)
			vcMin.x = m_pPoints[i].x;

		if (m_pPoints[i].y > vcMax.y)
			vcMax.y = m_pPoints[i].y;
		else if (m_pPoints[i].y < vcMin.y)
			vcMin.y = m_pPoints[i].y;

		if (m_pPoints[i].z > vcMax.z)
			vcMax.z = m_pPoints[i].z;
		else if (m_pPoints[i].z < vcMin.z)
			vcMin.z = m_pPoints[i].z;
	} // for

	m_Aabb.vcMax = vcMax;
	m_Aabb.vcMin = vcMin;
	m_Aabb.vcCenter = (vcMax + vcMin) / 2.0f;
}

void SMGPolygon::SwapFaces()
{
	unsigned int *pIndis = new unsigned int[m_NumI];

	// change index ordering
	for (int i = 0; i<m_NumI; i++)
		pIndis[m_NumI - i - 1] = m_pIndis[i];

	// change normal orientation
	m_Plane.m_vcN *= -1.0f;
	m_Plane.m_fD *= -1.0f;

	delete[] m_pIndis;
	m_pIndis = pIndis;
}

void SMGPolygon::Clip(const SMGPlane & Plane, SMGPolygon * pFront, SMGPolygon * pBack)
{
	if (!pFront && !pBack) return;

	SMGVector vcHit, vcA, vcB;
	SMGRay    Ray;

	// cast away const
	SMGPlane  *pPlane = ((SMGPlane*)&Plane);

	unsigned int nNumFront = 0,     // number points on frontside
		nNumBack = 0,      // number points on backside
		nLoop = 0,
		nCurrent = 0;

	SMGVector *pvcFront = new SMGVector[m_NumP * 3];
	SMGVector *pvcBack = new SMGVector[m_NumP * 3];

	// classify the first vertex and fill to appropriate list
	switch (pPlane->Classify(m_pPoints[0]))
	{
	case SMGFRONT:
		pvcFront[nNumFront++] = m_pPoints[0];
		break;
	case SMGBACK:
		pvcBack[nNumBack++] = m_pPoints[0];
		break;
	case SMGPLANAR:
		pvcBack[nNumBack++] = m_pPoints[0];
		pvcFront[nNumFront++] = m_pPoints[0];
		break;
	default: return;
	}

	// loop through all points of the polygon
	for (nLoop = 1; nLoop < (m_NumP + 1); nLoop++)
	{

		if (nLoop == m_NumP) nCurrent = 0;
		else nCurrent = nLoop;

		// take two neighboring points from the poly
		vcA = m_pPoints[nLoop - 1];
		vcB = m_pPoints[nCurrent];

		// classify points with respect to the plane
		int nClass = pPlane->Classify(vcB);
		int nClassA = pPlane->Classify(vcA);

		// if planar then put him to both sides
		if (nClass == SMGPLANAR)
		{
			pvcBack[nNumBack++] = m_pPoints[nCurrent];
			pvcFront[nNumFront++] = m_pPoints[nCurrent];
		}
		// else check if this edge intersects the plane
		else
		{
			Ray.m_vcOrig = vcA;
			Ray.m_vcDir = vcB - vcA;

			float fLength = Ray.m_vcDir.GetLength();

			if (fLength != 0.0f) Ray.m_vcDir /= fLength;

			if (Ray.Intersects(Plane, false, fLength, 0, &vcHit)
				&& (nClassA != SMGPLANAR))
			{

				// put the intersection point as new point for both
				pvcBack[nNumBack++] = vcHit;
				pvcFront[nNumFront++] = vcHit;

				// then sort the current point
				if (nClass == SMGFRONT)
				{
					if (nCurrent != 0)
						pvcFront[nNumFront++] = m_pPoints[nCurrent];
				} // if [FRONT]
				else if (nClass == SMGBACK)
				{
					if (nCurrent != 0)
						pvcBack[nNumBack++] = m_pPoints[nCurrent];
				} // else [BACK]		
			}
			// if no intersection then just sort this point
			else
			{
				if (nCurrent == 0) continue;

				if (nClass == SMGFRONT)
				{
					pvcFront[nNumFront++] = m_pPoints[nCurrent];
				}
				else if (nClass == SMGBACK)
				{
					pvcBack[nNumBack++] = m_pPoints[nCurrent];
				}
				else
				{
					return; // should not happen
				}
			}
		}
	} // for [NumP]


	  /* now we have the vertices for both new polys ready.
	  lets take care of the index lists now              */

	unsigned int I0, I1, I2;

	unsigned int *pnFront = NULL;
	unsigned int *pnBack = NULL;

	if (nNumFront > 2)
	{
		pnFront = new unsigned int[(nNumFront - 2) * 3];

		for (nLoop = 0; nLoop < (nNumFront - 2); nLoop++)
		{
			if (nLoop == 0) { I0 = 0; I1 = 1; I2 = 2; }
			else { I1 = I2; I2++; }

			pnFront[(nLoop * 3)] = I0;
			pnFront[(nLoop * 3) + 1] = I1;
			pnFront[(nLoop * 3) + 2] = I2;
		}
	}

	if (nNumBack > 2)
	{
		pnBack = new unsigned int[(nNumBack - 2) * 3];

		for (nLoop = 0; nLoop < (nNumBack - 2); nLoop++)
		{
			if (nLoop == 0) { I0 = 0; I1 = 1; I2 = 2; }
			else { I1 = I2; I2++; }

			pnBack[(nLoop * 3)] = I0;
			pnBack[(nLoop * 3) + 1] = I1;
			pnBack[(nLoop * 3) + 2] = I2;
		}
	}

	// now fuel in the information to the new poly objects
	if (pFront && pnFront)
	{
		pFront->Set(pvcFront, nNumFront, pnFront, (nNumFront - 2) * 3);

		// make sure new one has same orientation as original
		if (pFront->GetPlane().m_vcN * m_Plane.m_vcN < 0.0f)
			pFront->SwapFaces();
	}

	if (pBack && pnBack)
	{
		pBack->Set(pvcBack, nNumBack, pnBack, (nNumBack - 2) * 3);

		// make sure new one has same orientation as original
		if (pBack->GetPlane().m_vcN * m_Plane.m_vcN < 0.0f)
			pBack->SwapFaces();
	}

	// free temporary memory
	if (pvcFront) { delete[] pvcFront; }
	if (pvcBack) { delete[] pvcBack; }
	if (pnFront) { delete[] pnFront; }
	if (pnBack) { delete[] pnBack; }
}

void SMGPolygon::Clip(const SMGAabb & aabb)
{
	SMGPolygon BackPoly, ClippedPoly;
	SMGPlane   Planes[6];
	bool       bClipped = false;

	// cast away const
	SMGAabb *pAabb = ((SMGAabb*)&aabb);

	// get the planes of the aabb, note that
	// the normals are pointing outwards
	pAabb->GetPlanes(Planes);

	// make a copy of this polygon
	ClippedPoly.CopyOf(*this);

	// now clip the poly against the planes 
	for (int i = 0; i<6; i++)
	{
		if (Planes[i].Classify(ClippedPoly) == SMGCLIPPED)
		{
			ClippedPoly.Clip(Planes[i], NULL, &BackPoly);
			ClippedPoly.CopyOf(BackPoly);
			bClipped = true;
		}
	}

	// set this poly to the final clip output
	if (bClipped)
	{
		CopyOf(ClippedPoly);
	}
}

int SMGPolygon::Cull(const SMGAabb & aabb)
{
	SMGPlane Planes[6];
	int      nClass = 0;
	int      nInside = 0, nCurrent = 0;
	bool     bFirst = true;
	SMGRay   Ray;

	// cast away const
	SMGAabb *pAabb = ((SMGAabb*)&aabb);

	// get the planes of the aabb, note that
	// the normals are pointing outwards
	pAabb->GetPlanes(Planes);

	// check if polys aabb is roughly intersecting at all
	if (!m_Aabb.Intersects(aabb))
		return SMGCULLED; // no way

						  // now polygon is eventually but not
						  // necessarily intersecting the aabb
	for (int p = 0; p<6; p++)
	{

		// one time check if all points are inside the aabb
		if (bFirst)
		{
			for (int i = 0; i<m_NumP; i++)
			{
				if (pAabb->Intersects(m_pPoints[i]))
					nInside++;
			}
			bFirst = false;

			// all points inside => poly totally inside aabb
			if (nInside == m_NumP) return SMGVISIBLE;
		}

		// check for intersection of poly with aabb plane
		for (int nLoop = 1; nLoop < (m_NumP + 1); nLoop++)
		{
			if (nLoop == m_NumP) nCurrent = 0;
			else nCurrent = nLoop;

			// build ray from two neighboring points
			Ray.m_vcOrig = m_pPoints[nLoop - 1];
			Ray.m_vcDir = m_pPoints[nCurrent] - m_pPoints[nLoop - 1];

			float fLength = Ray.m_vcDir.GetLength();
			if (fLength != 0.0f) Ray.m_vcDir /= fLength;

			if (Ray.Intersects(Planes[p], false, fLength, 0, NULL))

				/* NEED TO CHECK WHETHER INTERSECTION POINT IS INSIDE AABB AT ALL */

				return SMGCLIPPED;
		}
	}

	// no intersection
	return SMGCULLED;
}

void SMGPolygon::CopyOf(const SMGPolygon & Poly)
{
	Set(Poly.m_pPoints, Poly.m_NumP,
		Poly.m_pIndis, Poly.m_NumI);
}

bool SMGPolygon::Intersects(const SMGRay & Ray, bool bCull, float * t)
{
	// cast away const
	SMGRay *pRay = (SMGRay*)&Ray;

	for (int i = 0; i<m_NumI; i += 3)
	{
		if (pRay->Intersects(m_pPoints[m_pIndis[i]],
							 m_pPoints[m_pIndis[i + 1]],
							 m_pPoints[m_pIndis[i + 2]],
							 false, t))
		{
			return true;
		}
		if (!bCull)
		{
			if (pRay->Intersects(m_pPoints[m_pIndis[i + 2]],
								 m_pPoints[m_pIndis[i + 1]],
								 m_pPoints[m_pIndis[i]],
								 false, t))
			{
				return true;
			}
		}
	}
	return false;
}

bool SMGPolygon::Intersects(const SMGRay & Ray, bool bCull, float fL, float * t)
{
	// cast away const
	SMGRay *pRay = (SMGRay*)&Ray;

	for (int i = 0; i<m_NumI; i += 3)
	{
		if (pRay->Intersects(m_pPoints[m_pIndis[i]],
							 m_pPoints[m_pIndis[i + 1]],
							 m_pPoints[m_pIndis[i + 2]],
							 false, fL, t))
		{
			return true;
		}
		if (!bCull)
		{
			if (pRay->Intersects(m_pPoints[m_pIndis[i + 2]],
								 m_pPoints[m_pIndis[i + 1]],
								 m_pPoints[m_pIndis[i]],
								 false, fL, t))
			{
				return true;
			}
		}
	}
	return false;
}