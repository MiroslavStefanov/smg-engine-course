#include "SMG3D.h"
using namespace glm;

SMGPolylist::SMGPolylist()
{
}

SMGPolylist::~SMGPolylist()
{
}

void SMGPolylist::AddPolygon(const SMGPolygon &p)
{
	m_pPolys.push_back(p);
}

void SMGPolylist::Reset()
{
	m_pPolys.clear();
}