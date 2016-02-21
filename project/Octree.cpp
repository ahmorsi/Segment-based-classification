#include "Octree.h"
#include<queue>
#include "stdio.h"
using namespace rv;

Octree::Octree(uint32_t bucketSize) :
    data_(0), root_(0), bucketSize_(bucketSize)
{

}

Octree::Octree(const Octree& other) :
    data_(0), root_(0), bucketSize_(0)
{

}

Octree& Octree::operator=(const Octree& other)
{
  return *this;
}

Octree::~Octree()
{
  delete root_;
}

Octree::Octant::Octant() :
    isLeaf(true), x(0.0f), y(0.0f), z(0.0f), extent(0.0f), start(0), end(0), size(0)
{
  memset(&child, 0, 8 * sizeof(Octant*));

}

Octree::Octant::~Octant()
{
  for (uint32_t i = 0; i < 8; ++i)
    delete child[i];
}

void Octree::clear()
{
  delete root_;
  root_ = 0;
  data_ = 0;
  successors_.clear();
}

void Octree::initialize(const std::vector<Point3f>& points, const std::vector<uint32_t>& indexes)
{
  clear();

  if (indexes.size() == 0) return;

  const uint32_t N = points.size();
  successors_ = std::vector<uint32_t>(N);
  data_ = &points;

  // determine axis-aligned bounding box.
  uint32_t lastIdx = indexes[0];
  float min[3], max[3];
  min[0] = points[lastIdx].x();
  min[1] = points[lastIdx].y();
  min[2] = points[lastIdx].z();
  max[0] = min[0];
  max[1] = min[1];
  max[2] = min[2];

  for (uint32_t i = 1; i < indexes.size(); ++i)
  {
    uint32_t idx = indexes[i];
    // initially each element links simply to the following element.
    successors_[lastIdx] = idx;

    const Point3f& p = points[idx];

    if (p.x() < min[0]) min[0] = p.x();
    if (p.y() < min[1]) min[1] = p.y();
    if (p.z() < min[2]) min[2] = p.z();
    if (p.x() > max[0]) max[0] = p.x();
    if (p.y() > max[1]) max[1] = p.y();
    if (p.z() > max[2]) max[2] = p.z();

    lastIdx = idx;
  }

  float ctr[3] =
  { min[0], min[1], min[2] };

  float maxextent = 0.5f * (max[0] - min[0]);
  ctr[0] += maxextent;
  for (uint32_t i = 1; i < 3; ++i)
  {
    float extent = 0.5f * (max[i] - min[i]);
    ctr[i] += extent;
    if (extent > maxextent) maxextent = extent;
  }
  root_ = createOctant(ctr[0], ctr[1], ctr[2], maxextent, indexes[0], lastIdx, indexes.size());
}

uint32_t Octree::mortonCode(const Point3f& p, float x, float y, float z) const
{
  uint32_t mortonCode = 0;

  if (p.x() > x) mortonCode |= 1;
  if (p.y() > y) mortonCode |= 2;
  if (p.z() > z) mortonCode |= 4;

  return mortonCode;
}

Octree::Octant* Octree::createOctant(float x, float y, float z, float extent, uint32_t startIdx, uint32_t endIdx,
    uint32_t size)
{
	Octant* oct = new Octant();
	oct->x=x;oct->y=y;oct->z=z;
	oct->size = size;
	oct->start = startIdx;
	oct->end = endIdx;
	oct->extent = extent;

	uint32_t M[8],Start[8],End[8];
	for(int i=0;i<8;++i)
		M[i]=0,Start[i]=0,End[i]=0;

	if(size > bucketSize_)
	{
		uint32_t i = startIdx,j=0;
		uint32_t lastChild = j;
		uint32_t firstChild = 8;
		while(j<size)
		{
			Point3f curP;
			curP = (*data_)[i];
			uint32_t k = mortonCode(curP,x,y,z);
			if(M[k] ==0)
				Start[k] = i;
			else
			{
				successors_[End[k]]=i;
			}
			End[k] = i;
			++ M[k];
			i = successors_[i];
			++j;

			if(k < firstChild)
				firstChild = k;
		}

		for(int k=0;k<8;++k)
		{
			if(M[k]>0)
			{
				oct->isLeaf = false;
				float cX,cY,cZ;
				cX = x + (((k & 1) > 0 ? 0.5 : -0.5)*extent);
				cY = y + (((k & 2) > 0 ? 0.5 : -0.5)*extent);
				cZ = z + (((k & 4) > 0 ? 0.5 : -0.5)*extent);
				oct->child[k] = createOctant(cX,cY,cZ,extent/2.0,Start[k],End[k],M[k]);
				Start[k] = oct->child[k]->start;
				End[k] = oct->child[k]->end;
				if(k == firstChild)
					oct->start=Start[k];
				else
				{
					successors_[End[lastChild]]=Start[k];
				}
				oct->end = End[k];
				lastChild = k;
			}
		}
	}
  return oct;
}

void Octree::radiusNeighbors(const Point3f& query, float radius, std::vector<uint32_t>& neighbors,
    const Norm& norm) const
{
  neighbors.clear();
  if (root_ == 0) return;

  radiusNeighbors(root_, query, radius, neighbors, norm);
}
bool Octree::PointInSphere(const Point3f& p, float radius, const Point3f& center,const Norm& norm) const
{
	float dist = norm.compute(p,center);
	return dist <= radius;
}
void Octree::radiusNeighbors(const Octant* octant, const Point3f& query, float radius, std::vector<uint32_t>& neighbors,
    const Norm& norm) const
{
	queue<Octant*> Q;
	Q.push((Octant*)octant);
	while(!Q.empty())
	{
		Octant* curOct = Q.front();
		Q.pop();

		if(contains(query,radius,curOct,norm))
		{
			int p=curOct->start;
			while(p!=curOct->end)
			{
				neighbors.push_back(p);
				p = successors_[p];
			}
			neighbors.push_back(p);
			continue;
		}
		if(curOct->isLeaf)
		{
			int p=curOct->start;
			while(p!=curOct->end)
			{
				if (PointInSphere((*data_)[p], radius, query,norm))
					neighbors.push_back(p);
				p = successors_[p];
			}
			if (PointInSphere((*data_)[p], radius, query,norm) )
			neighbors.push_back(p);
		}
		for(int i=0;i<8;++i)
		{
			if(curOct->child[i] == NULL)
				continue;

			if(overlaps(query,radius,curOct->child[i],norm))
				Q.push(curOct->child[i]);
		}
	}
}

bool Octree::overlaps(const Point3f& query, float radius, const Octant* o, const Norm& dist)
{
// we exploit the symmetry to reduce the test to testing if its inside the Minkowski sum around the positive quadrant.
  float x = query.x() - o->x;
  float y = query.y() - o->y;
  float z = query.z() - o->z;

  x = std::abs(x);
  y = std::abs(y);
  z = std::abs(z);

// (1) checking the line region.
  float maxdist = radius + o->extent;

// a. completely outside, since q' is outside the relevant area.
  if (x > maxdist || y > maxdist || z > maxdist) return false;

// b. inside the line region, one of the coordinates is inside the square.
  if (x < o->extent || y < o->extent || z < o->extent) return true;

// (2) checking the corner region...
  x -= o->extent;
  y -= o->extent;
  z -= o->extent;

  return (dist.compute(x, y, z) < radius);
}

bool Octree::contains(const Point3f& query, float radius, const Octant* o, const Norm& dist)
{
// we exploit the symmetry to reduce the test to test
// whether the farthest corner is inside the search ball.
  float x = query.x() - o->x;
  float y = query.y() - o->y;
  float z = query.z() - o->z;

  x = std::abs(x);
  y = std::abs(y);
  z = std::abs(z);
// reminder: (x, y, z) - (-e, -e, -e) = (x, y, z) + (e, e, e)
  x += o->extent;
  y += o->extent;
  z += o->extent;

  return (dist.compute(x, y, z) < radius);
}
