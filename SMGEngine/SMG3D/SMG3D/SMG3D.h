#pragma once

#include <math.h>
#include <memory.h>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

const double SMGPI = 3.14159265;
const double SMGPI2 = 1.5707963;
const double SMG2PI = 6.2831853;
const float  SMGG = -32.174f; // ft/s^2
const float  SMGEPSILON = 0.00001f;

#define SMGFRONT    0
#define SMGBACK     1
#define SMGPLANAR   2
#define SMGCLIPPED  3
#define SMGCULLED   4
#define SMGVISIBLE  5

float _fabs(float f);

class SMGMatrix;
class SMGObb;
class SMGAabb;
class SMGPlane;
class SMGQuat;
class SMGPolygon;

class SMGVector
{
	friend class SMGMatrix;
	friend class SMGQuat;
public:
	float &x, &y, &z;      // coordinateset

	SMGVector() : x(m_v.x), y(m_v.y), z(m_v.z) { x = 0, y = 0, z = 0; }
	SMGVector(float _x, float _y, float _z) : x(m_v.x), y(m_v.y), z(m_v.z)
	{
		x = _x; y = _y; z = _z;
	}
	SMGVector(const SMGVector&);

	inline void  Set(float _x, float _y, float _z);
	inline float GetLength();                   // length
	inline float GetSqrLength() const;          // square length
	inline void  Negate();                      // vector mult -1
	inline void  Normalize();                   // normalize
	inline float AngleWith(SMGVector &v);          // angle in rad
	inline void  RotateWith(const SMGMatrix&);      // apply rotation part
	inline void  InvRotateWith(const SMGMatrix&);   // apply inverse rotation

	void operator = (const SMGVector &v);
	void operator += (const SMGVector &v);          // operator +=
	void operator -= (const SMGVector &v);          // operator -=
	void operator *= (float f);                     // scale vector
	void operator /= (float f);                     // scale down
	void operator += (float f);                     // add scalar
	void operator -= (float f);                     // subtract scalar
	float     operator * (const SMGVector &v)const; // dot product
	SMGVector operator * (float f)const;            // scale vector
	SMGVector operator / (float f)const;            // scalar divide
	SMGVector operator + (float f)const;            // add scalar
	SMGVector operator - (float f)const;            // scale down
	SMGQuat   operator * (const SMGQuat   &q)const; // vector * quaternion
	SMGVector operator * (const SMGMatrix &m)const; // vector-matrix product
	SMGVector operator + (const SMGVector &v)const; // addition
	SMGVector operator - (const SMGVector &v)const; // subtraction

	void Cross(const SMGVector &v1,          // cross product
					  const SMGVector &v2);
private:
	glm::vec3 m_v;

	SMGVector(const glm::vec3 &vc) : x(m_v.x), y(m_v.y), z(m_v.z)
	{
		x = vc.x; y = vc.y; z = vc.z;
	}
};

class SMGMatrix
{
	friend class SMGVector;
	friend class SMGQuat;
public:
	float &_11, &_12, &_13, &_14;
	float &_21, &_22, &_23, &_24;
	float &_31, &_32, &_33, &_34;
	float &_41, &_42, &_43, &_44;


	SMGMatrix();
	SMGMatrix(const SMGMatrix&);

	inline void Identity();							  // identity matrix
	inline void RotaX(float a);                       // x axis
	inline void RotaY(float a);                       // y axis
	inline void RotaZ(float a);                       // z axis
	inline void Rota(const SMGVector &vc);            // x, y and z
	inline void Rota(float x, float y, float z);      // x, y and z
	inline void RotaArbi(const SMGVector &vcAxis, float a);
	inline void ApplyInverseRota(SMGVector *pvc);
	inline void Translate(float dx, float dy, float dz);
	inline void SetTranslation(SMGVector vc, bool EraseContent = false);
	inline SMGVector GetTranslation();

	void TransposeOf(const SMGMatrix &m);       // transpose m, save result in this
	void InverseOf(const SMGMatrix &m);         // invert m, save result in this

	void operator = (const SMGMatrix &m);

	SMGMatrix operator * (const SMGMatrix &m)const;    // matrix multiplication
	SMGVector operator * (const SMGVector &vc)const;   // matrix vector multiplication
	
private:
	glm::mat4 m_m;

	SMGMatrix(const glm::mat4&);
};

class SMGRay
{
public:
	SMGVector m_vcOrig,  // ray origin
		m_vcDir;   // ray direction


	SMGRay(SMGVector vcOrig = SMGVector(0, 0, 0), SMGVector vcDir = SMGVector(0, 0, 0)) : m_vcOrig(vcOrig), m_vcDir(vcDir) {}

	inline void Set(const SMGVector& orig, const SMGVector& direction);

	inline void DeTransform(const SMGMatrix &_m); // move to matrixspace

	bool Intersects(const SMGVector &vc0, const SMGVector &vc1,
					const SMGVector &vc2, bool bCull,
					float *t);
	bool Intersects(const SMGVector &vc0, const SMGVector &vc1,
					const SMGVector &vc2, bool bCull,
					float fL, float *t);
	bool Intersects(const SMGPlane &plane, bool bCull,
					float *t, SMGVector *vcHit);
	bool Intersects(const SMGPlane &plane, bool bCull,
					float fL, float *t, SMGVector *vcHit);
	bool Intersects(const SMGAabb &aabb, float *t);
	bool Intersects(const SMGAabb &aabb, float fL, float *t);
	bool Intersects(const SMGObb &obb, float *t);
	bool Intersects(const SMGObb &obb, float fL, float *t);
};

class SMGPlane
{
public:
	SMGVector m_vcN,       // plane normal vector
		m_vcPoint;   // point on plane
	float     m_fD;        // distance to origin


	SMGPlane() { /* nothing to do */; }

	inline void  Set(const SMGVector &vcN, const SMGVector &vcP);
	inline void  Set(const SMGVector &vcN, const SMGVector &vcP, float fD);
	inline void  Set(const SMGVector &v0, const SMGVector &v1, const SMGVector &v2);
	inline float Distance(const SMGVector &vcPoint);
	inline int   Classify(const SMGVector &vcPoint);
	int   Classify(const SMGPolygon &Polygon);

	bool Clip(const SMGRay*, float, SMGRay*, SMGRay*);

	bool Intersects(const SMGVector &vc0, const SMGVector &vc1,
					const SMGVector &vc2);
	bool Intersects(const SMGPlane &plane, SMGRay *pIntersection);
	bool Intersects(const SMGAabb &aabb);
	bool Intersects(const SMGObb &obb);
};

class SMGObb
{
public:
	float     fA0, fA1, fA2; // half axis length
	SMGVector vcA0, vcA1, vcA2; // box axis
	SMGVector vcCenter;         // centerpoint

	SMGObb() { /* nothing to do */; }

	inline void DeTransform(const SMGObb &obb,
							const SMGMatrix &m);

	bool Intersects(const SMGRay &Ray, float *t);
	bool Intersects(const SMGRay &Ray, float fL, float *t);
	bool Intersects(const SMGObb &Obb);
	bool Intersects(const SMGVector &v0,
					const SMGVector &v1,
					const SMGVector &v2);

	int  Cull(const SMGPlane *pPlanes, int nNumPlanes);

private:
	void ObbProj(const SMGObb &Obb, const SMGVector &vcV,
				 float *pfMin, float *pfMax);
	void TriProj(const SMGVector &v0, const SMGVector &v1,
				 const SMGVector &v2, const SMGVector &vcV,
				 float *pfMin, float *pfMax);
};

class SMGAabb
{
public:
	SMGVector vcMin, vcMax; // box extreme points
	SMGVector vcCenter;     // centerpoint

	SMGAabb() { /* nothing to do */; }
	SMGAabb(SMGVector vcMin, SMGVector vcMax);

	void Construct(const SMGObb *pObb);                  // build from obb
	int  Cull(const SMGPlane *pPlanes, int nNumPlanes);

	// normals pointing outwards
	void GetPlanes(SMGPlane *pPlanes);

	bool Contains(const SMGRay &Ray, float fL);
	bool Intersects(const SMGRay &Ray, float *t);
	bool Intersects(const SMGRay &Ray, float fL, float *t);
	bool Intersects(const SMGAabb &aabb);
	bool Intersects(const SMGVector &vc);
};

class SMGPolygon
{
	friend class SMGPlane;        // access for easier classifying

private:
	SMGPlane       m_Plane;    // plane which poly lies in

	int            m_NumP;     // number of points
	int            m_NumI;     // number of indices
	SMGAabb        m_Aabb;     // bounding box
	unsigned int   m_Flag;     // whatever you want it to be

	void CalcBoundingBox();

public:
	SMGPolygon();
	SMGPolygon(const SMGPolygon&);
	~SMGPolygon();


	SMGVector    *m_pPoints;  // list of points
	unsigned int *m_pIndis;   // index list

	void          Set(const SMGVector *pPoints, int nNumP,
					  const unsigned int *pIndis, int nNumI);

	void          Clip(const SMGPlane &Plane,
					   SMGPolygon *pFront,
					   SMGPolygon *pBack);
	void          Clip(const SMGAabb &aabb);
	int           Cull(const SMGAabb &aabb);

	void          CopyOf(const SMGPolygon &Poly);

	void          SwapFaces();

	bool          Intersects(const SMGRay &Ray, bool, float *t);
	bool          Intersects(const SMGRay &Ray, bool, float fL, float *t);

	int           GetNumPoints() { return m_NumP; }
	int           GetNumIndis() { return m_NumI; }
	SMGVector*    GetPoints() { return m_pPoints; }
	unsigned int* GetIndices() { return m_pIndis; }
	SMGPlane      GetPlane() { return m_Plane; }
	SMGAabb       GetAabb() { return m_Aabb; }
	unsigned int  GetFlag() { return m_Flag; }
	void          SetFlag(unsigned int n) { m_Flag = n; }
};

class SMGPolylist
{
public:
	SMGPolylist();
	~SMGPolylist();

	void AddPolygon(const SMGPolygon&);
	void Reset();

	std::vector<SMGPolygon>  GetPolylist() { return m_pPolys; }
	unsigned int  GetNum() { return m_pPolys.size(); }

private:
	std::vector<SMGPolygon> m_pPolys;
};

class SMGQuat
{
	friend class SMGMatrix;
	friend class SMGVector;
public:
	float &x, &y, &z, &w;

	SMGQuat() : x(m_q.x), y(m_q.y), z(m_q.z), w(m_q.w) { x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f; }
	SMGQuat(float _x, float _y, float _z, float _w) : x(m_q.x), y(m_q.y), z(m_q.z), w(m_q.w)
	{
		x = _x; y = _y; z = _z; w = _w;
	}

	void  MakeFromEuler(float fPitch, float fYaw, float fRoll);
	void  Normalize();
	void  Conjugate(SMGQuat q);
	void  GetEulers(float *fPitch, float *fYaw, float *fRoll);
	SMGMatrix  GetMatrix();
	float GetMagnitude();


	void    operator /= (float f);
	SMGQuat operator /  (float f);

	void    operator *= (float f);
	SMGQuat operator *  (float f);

	SMGQuat operator *  (const SMGVector &v) const;

	SMGQuat operator *  (const SMGQuat &q) const;
	void    operator *= (const SMGQuat &q);

	void    operator += (const SMGQuat &q);
	SMGQuat operator +  (const SMGQuat &q) const;

	void operator = (const SMGQuat &q);

	SMGQuat operator~() const { return SMGQuat(-x, -y, -z, w); }

	void Rotate(const SMGQuat &q1, const SMGQuat &q2);

	SMGVector Rotate(const SMGVector &v);

private:
	glm::quat m_q;

	SMGQuat(glm::quat q) : x(m_q.x), y(m_q.y), z(m_q.z), w(m_q.w) { m_q = q; }
};
