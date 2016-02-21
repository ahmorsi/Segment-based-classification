#include "GridbasedSegmentation.h"

#include <cmath>
#include <queue>
#include <algorithm>
#include<cfloat>

using namespace rv;

GridbasedSegmentation::GridCell::GridCell() :
    visited(false), minZ(INT_MAX), maxZ(INT_MIN)
{
}

void GridbasedSegmentation::GridCell::reset()
{
  visited = false;
  indexes.clear();
}

bool GridbasedSegmentation::GridCell::isValid(float mH){
	return this->maxZ - this->minZ >= mH;
}

uint32_t GridbasedSegmentation:: _1dIndex(int i, int j) const
{
	return i*gridSize_ + j;
}

GridbasedSegmentation::GridbasedSegmentation(const ParameterList& params)
{
  cellSize_ = params["resolution"];
  maxDistance_ = params["max distance"];
  gridSize_ = (uint32_t) (std::ceil(2.0 * maxDistance_ / cellSize_));
  minHeight_ = params["min height"];
  minPoints_ = params["min points"];

  // initialize regular grid grid.
  grid_.resize(gridSize_ * gridSize_);
}
uint32_t GridbasedSegmentation::mapToGrid(const rv::Point3f& p, bool show = false) const
{
//	float x = (p.x() + maxDistance_) * (gridSize_) / (2 * maxDistance_);
//	float y = (p.y() + maxDistance_) * (gridSize_) / (2 * maxDistance_);
//
//	int j = (int)x;//(int)floor(x / cellSize_);
//	int i = (int)y;//(int)floor(y / cellSize_);

	int j=(p.x() + cellSize_*gridSize_/2)/cellSize_;
	int i=(p.y() + cellSize_*gridSize_/2)/cellSize_;

	return _1dIndex(i, j);
}
bool GridbasedSegmentation::InsideMaxDistance(const rv::Point3f& p)const
{
	return	p.x() <=  maxDistance_ &&
			p.y() <=  maxDistance_ &&
			p.x() >= -maxDistance_ &&
			p.y() >= -maxDistance_;
}
bool GridbasedSegmentation::InsideGrid(int x,int y) const
{
	return x>=0 && x<gridSize_ && y>=0 && y<gridSize_;
}
std::vector<uint32_t> GridbasedSegmentation::RegionGrowing(uint32_t start)
{
	std::vector<uint32_t> segmentIndexes;
	std::queue<uint32_t> q;
	q.push(start);
	grid_[start].visited = true;
	for(int k = 0; k < grid_[start].indexes.size(); k ++)
		segmentIndexes.push_back(grid_[start].indexes[k]);
	while(!q.empty())
	{
		uint32_t current = q.front();
		q.pop();

		int curY = current/gridSize_;
		int curX = current - (curY*gridSize_);

		for(int i = 0; i < 4; i ++)
		{
			//int nw = current + this->dirs[i];
			int nwX = curX + dirX[i];
			int nwY = curY + dirY[i];
			if(InsideGrid(nwX,nwY))
			{
				uint32_t nw = _1dIndex(nwY,nwX);
				if(!grid_[nw].visited && grid_[nw].indexes.size() > 0)
				{
					grid_[nw].visited = true;
					std::vector<uint32_t> indexes = grid_[nw].indexes;
					for(int k = 0; k < indexes.size(); k ++)
						segmentIndexes.push_back(indexes[k]);
					q.push(nw);
				}
			}
		}
	}
	return segmentIndexes;
}
void GridbasedSegmentation::segment(const Laserscan& scan, std::vector<IndexedSegment>& segments)
{
  // cleanup.
  segments.clear();
  int gridSize1D = gridSize_ * gridSize_;
  for(int i=0;i<gridSize1D;++i)
	  grid_[i].reset();
  const std::vector< Point3f > &pts = scan.points();
  for(int i = 0; i < pts.size(); i ++)
  {
	  if(InsideMaxDistance(pts[i]))
		  {
			  int index = mapToGrid(pts[i]);
			  if(index < 0)
			  {
				  std::cerr<<"Index is less than 0!"<<std::endl;
			  }
			  if(index >= grid_.size())
			  {
				  mapToGrid(pts[i], true);
			  }
			  if(grid_[index].indexes.size() == 0)
				  grid_[index].minZ = grid_[index].maxZ = pts[i].z();

			  grid_[index].indexes.push_back(i);
			  grid_[index].minZ = std::min(grid_[index].minZ, pts[i].z());
			  grid_[index].maxZ = std::max(grid_[index].maxZ, pts[i].z());
		  }
  }
  for(int i = 0; i < grid_.size(); i ++)
  {
	  if(!grid_[i].isValid(minHeight_))
	  {
		  grid_[i].reset();
	  }
  }

  for(int i = 0; i < grid_.size(); i ++)
  {
	  if(grid_[i].visited || grid_[i].indexes.size() == 0)
		  continue;
	  IndexedSegment segment;

	  segment.indexes = RegionGrowing(i);

	  if(segment.size() >= minPoints_)
		  segments.push_back(segment);
  }

  // grid starting a each unvisited grid cell.


}

