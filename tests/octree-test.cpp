#include <string>
#include <map>
#include <queue>
#include <gtest/gtest.h>
#include <boost/random.hpp>
#include <rv/geometry.h>
#include <rv/norms.h>

#include "../project/Octree.h"
#include "test_utils.h"

using namespace rv;

namespace
{

// The fixture for testing the octree and getting access to "private parts".
class OctreeTest: public ::testing::Test
{
  protected:
    typedef Octree::Octant Octant;

    // helper methods to access the protected parts of octree for consistency checks.
    const Octree::Octant* getRoot(const Octree& oct)
    {
      return oct.root_;
    }

    const std::vector<uint32_t>& getSuccessors(const Octree& oct)
    {
      return oct.successors_;
    }
};

void randomPoints(std::vector<Point3f>& pts, uint32_t N, uint32_t seed = 0)
{
  boost::mt11213b mtwister(seed);
  boost::uniform_01<> gen;
  pts.clear();
  pts.reserve(N);
  // generate N random points in [-5.0,5.0] x [-5.0,5.0] x [-5.0,5.0]...
  for (uint32_t i = 0; i < N; ++i)
  {
    float x = 10.0f * gen(mtwister) - 5.0f;
    float y = 10.0f * gen(mtwister) - 5.0f;
    float z = 10.0f * gen(mtwister) - 5.0f;

    pts.push_back(Point3f(x, y, z));
  }
}

TEST_F(OctreeTest, Initialize)
{
  typedef OctreeTest::Octant Octant;

  uint32_t N = 1000;

  uint32_t bucketSize = 16;

  Octree oct(bucketSize);

  const Octant* root = getRoot(oct);
  const std::vector<uint32_t>& successors = getSuccessors(oct);

  ASSERT_EQ(0, root);

  std::vector<Point3f> points;
  randomPoints(points, N, 1337);

  ((NearestNeighborImpl*) &oct)->initialize(points);
  oct.initialize(points);

  root = getRoot(oct);

  // check first some pre-requisits.
  ASSERT_EQ(true, (root != 0))<< "root should be initialized.";
  ASSERT_EQ(N, successors.size())<< "successors should be of size " << N;

  std::vector<uint32_t> elementCount(N, 0);
  uint32_t idx = root->start;
  for (uint32_t i = 0; i < N; ++i)
  {
    ASSERT_LT(idx, N);
    ASSERT_LE(successors[idx], N);
    elementCount[idx] += 1;
    ASSERT_EQ(1, elementCount[idx])<< "point "<< i << " found twice in successors.";
    idx = successors[idx];
  }

  // check that each index was found.
  for (uint32_t i = 0; i < N; ++i)
  {
    ASSERT_EQ(1, elementCount[i])<< "point with index " << i << " not found.";
  }

  // test if each Octant contains only points inside the octant and child octants have only real subsets of parents!
  std::queue<const Octant*> queue;
  queue.push(root);
  std::vector<int32_t> assignment(N, -1);

  while (!queue.empty())
  {
    const Octant* octant = queue.front();
    queue.pop();

    // check points.
    ASSERT_LT(octant->start, N);

    // test if each point assigned to a octant really is inside the octant.

    uint32_t idx = octant->start;
    uint32_t lastIdx = octant->start;
    for (uint32_t i = 0; i < octant->size; ++i)
    {
      float x = points[idx].x() - octant->x;
      float y = points[idx].y() - octant->y;
      float z = points[idx].z() - octant->z;

      ASSERT_LE(std::abs(x), octant->extent);
      ASSERT_LE(std::abs(y), octant->extent);
      ASSERT_LE(std::abs(z), octant->extent);
      assignment[idx] = -1; // reset of child assignments.
      lastIdx = idx;
      idx = successors[idx];
    }
    ASSERT_EQ(octant->end, lastIdx);

    bool shouldBeLeaf = true;
    Octant* firstchild = 0;
    Octant* lastchild = 0;
    uint32_t pointSum = 0;

    for (uint32_t c = 0; c < 8; ++c)
    {
      Octant* child = octant->child[c];
      if (child == 0) continue;
      shouldBeLeaf = false;

      // child nodes should have start end intervals, which are true subsets of the parent.
      if (firstchild == 0) firstchild = child;
      // the child nodes should have intervals, where succ(e_{c-1}) == s_{c}, and \sum_c size(c) = parent size!
      if (lastchild != 0) ASSERT_EQ(child->start, successors[lastchild->end]);

      pointSum += child->size;
      lastchild = child;
      uint32_t idx = child->start;
      for (uint32_t i = 0; i < child->size; ++i)
      {
        // check if points are uniquely assigned to single child octant.
        ASSERT_EQ(-1, assignment[idx]);
        assignment[idx] = c;
        idx = successors[idx];
      }

      queue.push(child);
    }

    // consistent start/end of octant and its first and last children.
    if (firstchild != 0) ASSERT_EQ(octant->start, firstchild->start);
    if (lastchild != 0) ASSERT_EQ(octant->end, lastchild->end);

    // check leafs flag.
    ASSERT_EQ(shouldBeLeaf, octant->isLeaf);
    ASSERT_EQ((octant->size <= bucketSize), octant->isLeaf)<< "Leaf node contains more than " << bucketSize << " points.";

    // test if every point is assigned to a child octant.
    if (!octant->isLeaf)
    {
      ASSERT_EQ(octant->size, pointSum);
      uint32_t idx = octant->start;
      for (uint32_t i = 0; i < octant->size; ++i)
      {
        ASSERT_GT(assignment[idx], -1);
        idx = successors[idx];
      }
    }
  }
}

TEST_F(OctreeTest, InitializeSubset)
{
  typedef OctreeTest::Octant Octant;

  uint32_t N = 500;

  std::vector<Point3f> points;
  randomPoints(points, N, 1337);

  std::vector<uint32_t> indexes(N);
  for (uint32_t i = 0; i < N; ++i)
    indexes[i] = i;
  std::random_shuffle(indexes.begin(), indexes.end());
  indexes.resize(100);

  Octree oct(32);
  oct.initialize(points, indexes);

  const Octant* root = getRoot(oct);
  ASSERT_FALSE(root == 0);
  const std::vector<uint32_t>& successors = getSuccessors(oct);

  std::vector<uint32_t> elementCount(N, 0);
  std::vector<bool> inIndexes(N, false);
  for (uint32_t i = 0; i < indexes.size(); ++i)
    inIndexes[indexes[i]] = true;

  // descend to leafs and count elements inside.
  std::queue<const Octant*> queue;
  queue.push(root);
  while (!queue.empty())
  {
    const Octant* oct = queue.front();
    queue.pop();

    uint32_t idx = oct->start;
    for (;; idx = successors[idx])
    {
      ASSERT_TRUE(inIndexes[idx])<< "point with index " << idx << " should be not inside octree.";
      if(idx == oct->end) break;
    }

    if (oct->isLeaf)
    {
      idx = oct->start;
      for (;; idx = successors[idx])
      {
        ASSERT_EQ(0, elementCount[idx])<< "point with index " << idx << " twice in leafs.";
        elementCount[idx] += 1;
        if(idx == oct->end) break;
      }
    }

    for (uint32_t i = 0; i < 8; ++i)
    {
      if (oct->child[i] != 0) queue.push(oct->child[i]);
    }
  }

  for (uint32_t i = 0; i < indexes.size(); ++i)
  {
    ASSERT_EQ(1, elementCount[indexes[i]])<< "point " << indexes[i] << " not inside octree.";
    elementCount[indexes[i]] = 0; // set all counts to zero.
  }

  // now all counts should be zero!
  for (uint32_t i = 0; i < elementCount.size(); ++i)
  {
    ASSERT_EQ(0, elementCount[i])<< "point " << i << " in octree, but not inside indexes.";
  }
}

template<typename T>
bool similarVectors(std::vector<T>& vec1, std::vector<T>& vec2)
{
  if (vec1.size() != vec2.size())
  {
    std::cout << "expected size = " << vec1.size() << ", but got size = " << vec2.size() << std::endl;
    return false;
  }

  for (uint32_t i = 0; i < vec1.size(); ++i)
  {
    bool found = false;
    for (uint32_t j = 0; j < vec2.size(); ++j)
    {
      if (vec1[i] == vec2[j])
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      std::cout << i << "-th element (" << vec1[i] << ") not found." << std::endl;
      return false;
    }
  }

  return true;
}

TEST_F(OctreeTest, RadiusNeighbors)
{
  uint32_t N = 1000;

  boost::mt11213b mtwister(1234);
  boost::uniform_int<> uni_dist(0, N - 1);

  std::vector<Point3f> points;
  randomPoints(points, N, 1234);

  NaiveNeighborSearch bruteforce;
  bruteforce.initialize(points);
  Octree octree;
  octree.initialize(points);

  float radii[4] =
  { 0.5, 1.0, 2.0, 5.0 };

  for (uint32_t r = 0; r < 4; ++r)
  {
    for (uint32_t i = 0; i < 10; ++i)
    {
      std::vector<uint32_t> neighborsBruteforce;
      std::vector<uint32_t> neighborsOctree;

      const Point3f& query = points[uni_dist(mtwister)];

      bruteforce.radiusNeighbors(query, radii[r], neighborsBruteforce, EuclideanNorm());
      octree.radiusNeighbors(query, radii[r], neighborsOctree, EuclideanNorm());
      ASSERT_EQ(true, similarVectors(neighborsBruteforce, neighborsOctree));

      bruteforce.radiusNeighbors(query, radii[r], neighborsBruteforce, ManhattenNorm());
      octree.radiusNeighbors(query, radii[r], neighborsOctree, ManhattenNorm());

      ASSERT_EQ(true, similarVectors(neighborsBruteforce, neighborsOctree));

      bruteforce.radiusNeighbors(query, radii[r], neighborsBruteforce, MaximumNorm());
      octree.radiusNeighbors(query, radii[r], neighborsOctree, MaximumNorm());

      ASSERT_EQ(true, similarVectors(neighborsBruteforce, neighborsOctree));
    }
  }
}

}
