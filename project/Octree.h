#ifndef OCTREE_H_
#define OCTREE_H_

#include <vector>
#include <rv/geometry.h>
#include <rv/norms.h>
#include <rv/NearestNeighborImpl.h>
#include<iostream>
#include <fstream>
using namespace std;
// forward declaration needed for gtest access to protected/private members ...
namespace
{
class OctreeTest;
}

/** \brief Octree for searching radius neighbors
 *
 *  \author you
 */
class Octree: public rv::NearestNeighborImpl
{
    friend class ::OctreeTest;
  public:
    Octree(uint32_t bucketSize = 32);
    ~Octree();

    void clear();


    void initialize(const std::vector<rv::Point3f>& points, const std::vector<uint32_t>& indexes);

    void radiusNeighbors(const rv::Point3f& query, float radius, std::vector<uint32_t>& neighbors, const rv::Norm& dist) const;

    // "unhide" the base implementation of initialize.
    using rv::NearestNeighborImpl::initialize;
  protected:
    // Octree can not be copied/assigned.
    Octree(const Octree& other);
    Octree& operator=(const Octree& other);

    class Octant
    {
      public:
        Octant();
        ~Octant();

        bool isLeaf;

        // bounding box of the octant needed for overlap and contains tests...
        float x, y, z;
        float extent;

        // start and end in successors_
        uint32_t start, end;
        // number of points
        uint32_t size;

        Octant* child[8];
    };

    /** \brief get Morton code of p in respect to x, y, z **/
    uint32_t mortonCode(const rv::Point3f& p, float x, float y, float z) const;

    /**
     * \brief creation of an octant using the elements starting at startIdx.
     *
     * The method reorders the index such that all points are correctly linked to successors belonging
     * to the same octant.
     *
     * \param x,y,z           center coordinates of octant
     * \param extent          extent of octant ( half of side length)
     * \param startIdx        first index of points inside octant
     * \param endIdx          last index of points inside octant
     * \param size            number of points in octant
     *
     * \return  octant with children nodes.
     */
    Octant* createOctant(float x, float y, float z, float extent, uint32_t startIdx, uint32_t endIdx, uint32_t size);

    void radiusNeighbors(const Octant* octant, const rv::Point3f& query, float radius, std::vector<uint32_t>& neighbors,
        const rv::Norm& dist) const;

    /** \brief test if search ball S(q,r) overlaps with octant
     *
     * @param query   query point
     * @param radius  "squared" radius
     * @param o       pointer to octant
     *
     * @return true, if search ball overlaps with octant, false otherwise.
     */
    static bool overlaps(const rv::Point3f& query, float radius, const Octant* o, const rv::Norm& dist);

    /** \brief test if search ball S(q,r) contains octant
     *
     * @param query    query point
     * @param sqRadius "squared" radius
     * @param octant   pointer to octant
     *
     * @return true, if search ball overlaps with octant, false otherwise.
     */
    static bool contains(const rv::Point3f& query, float radius, const Octant* octant, const rv::Norm& dist);
    bool PointInSphere(const rv::Point3f&, float, const rv::Point3f&,const rv::Norm&) const;
    const std::vector<rv::Point3f>* data_;
    Octant* root_;
    uint32_t bucketSize_;
    std::vector<uint32_t> successors_;
    fstream logger;
};

#endif /* OCTREE_H_ */
