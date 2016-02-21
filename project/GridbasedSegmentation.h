#ifndef GRIDSEGMENTATION_H_
#define GRIDSEGMENTATION_H_

#include <rv/Segmentation.h>
#include <rv/ParameterList.h>

/** \brief Grid-based Segmentation
 * 
 *  The segmentation needs the following parameters:
 *
 *      resolution:float    =  size of grid cells
 *      max distance:float  =  maximal distance of points in the plane to consider
 *      min height:float    =  minimum difference of z-coordinates in grid cells to consider for segmentation
 *      min points:integer  =  minimum number of points of segment
 *
 *  \author you
 */
class GridbasedSegmentation: public rv::Segmentation
{
  public:
    GridbasedSegmentation(const rv::ParameterList& params);

    void segment(const rv::Laserscan& scan, std::vector<rv::IndexedSegment>& segments);

  protected:
    /*
     * Transform 2d Index to 1d index.
     * params: i (row), j (col)
     *
     * author: Sharaf
     * */
    uint32_t _1dIndex(int, int) const;
    /*
     * Map point from a scan world to grid world.
	 *              (P - minA) * (maxB - minB)
	 *  P` = minB + ---------------------------
	 *                   (maxA - minA)
	 * author: Sharaf.
	 * */
    uint32_t mapToGrid(const rv::Point3f&, bool) const;
    bool InsideMaxDistance(const rv::Point3f& p)const;
    bool InsideGrid(int x,int y) const;
    std::vector<uint32_t> RegionGrowing(uint32_t);
    float cellSize_, minHeight_, maxDistance_;
    uint32_t gridSize_, minPoints_;

    int dirs[4] = {-1, -gridSize_, 1, gridSize_};
    int dirX[4] = {1,-1,0,0};
    int dirY[4] = {0,0,1,-1};
    //const int DIRS = 4;
    class GridCell
    {
      public:
        GridCell();

        void reset();
        bool isValid(float minHeight);
        bool visited;                   // indicator for region growing.
        float minZ, maxZ;               // minimum and maximum z-value
        std::vector<uint32_t> indexes;  // indexes of points from point cloud
    };

    std::vector<GridCell> grid_;

};

#endif /* GRIDSEGMENTATION_H_ */
