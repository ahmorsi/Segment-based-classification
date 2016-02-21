#include "KMeans.h"

#include <algorithm>
#include <cassert>
#include <rv/Stopwatch.h>
#include <iostream>
#include<cfloat>
#include<cmath>
using namespace rv;

KMeans::KMeans()
{

}

std::vector<std::vector<float> > KMeans::cluster(const std::vector<std::vector<float> >& data, uint32_t C)
{
  const uint32_t N = data.size();
  const uint32_t D = data[0].size();

  std::vector<std::vector<float> > centers(C, std::vector<float>(D, 0.0f));

  // TODO: (1) initialize centers and (2) compute new centers until convergence.
  std::vector<KMeans::Cluster*> clusters(C);
  //(1) Initalize Centers with C random Feature Vectors
  std::vector<uint32_t> indexes;
  for(uint32_t i = 0; i < N; ++i)
  indexes.push_back(i);
  std::random_shuffle(indexes.begin(), indexes.end());

  for(int i=0;i<C;++i)
	  clusters[i] = new Cluster(indexes[i],data[indexes[i]]);
  bool IsConveraged;
  do
  {
  for(int i=0;i<data.size();++i)
  {
	  uint32_t nearestClusterIdx = getNearestCluster(clusters,data[i]);
	  clusters[nearestClusterIdx]->add(i);
  }
  IsConveraged = true;
  for(int i=0;i<C;++i)
  {
	  IsConveraged &= !(clusters[i]->updateCentroid(data));
	  clusters[i]->clear();
  }

  }while(!IsConveraged);
  for(int i=0;i<C;++i)
	  centers[i]=clusters[i]->getCentroid();
  return centers;
}

bool KMeans::equal(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) const
{
  assert(a.size() == b.size());

  for (uint32_t i = 0; i < a.size(); ++i)
  {
    if (a[i] != b[i]) return false;
  }

  return true;
}

float KMeans::distanceSqr(const std::vector<float>& a, const std::vector<float>& b) const
{
  assert(a.size() == b.size());
  float d = 0.0f;

  for (uint32_t j = 0; j < a.size(); ++j)
    d += (a[j] - b[j]) * (a[j] - b[j]);

  return d;
}
uint32_t KMeans::getNearestCluster(const std::vector<KMeans::Cluster*>& K_Clusters,const std::vector<float>& fv) const
{
	uint32_t nearestClusterIdx;
	float minDist = FLT_MAX ;
	for(int i=0;i<K_Clusters.size();++i)
	{
		float dist = distanceSqr(K_Clusters[i]->getCentroid(),fv);
		if(dist < minDist)
		{
			minDist = dist;
			nearestClusterIdx = i;
		}
	}
	return nearestClusterIdx;
}

KMeans::Cluster::Cluster()
{

}
KMeans::Cluster::Cluster(uint32_t centroidIdx,const std::vector<float>& centroid):m_centroid(centroid)
{
	add(centroidIdx);
}
uint32_t KMeans::Cluster::size() const
{
	return m_clusterData.size();
}

std::vector<float> KMeans::Cluster::getCentroid() const
{
	return m_centroid;
}
void KMeans::Cluster::add(uint32_t featureVectIndx)
{
	if(isExist(featureVectIndx))
		return;
	m_clusterData.push_back(featureVectIndx);
}
bool KMeans::Cluster::updateCentroid(const std::vector<std::vector<float> >& data)
{
	std::vector<float> prevCentroid (m_centroid);
	std::fill(m_centroid.begin(), m_centroid.end(), 0);
	for(int idx=0;idx < m_clusterData.size();++idx)
	{
		uint32_t fvIndx = m_clusterData[idx];
		for(int i=0;i<data[fvIndx].size();++i)
			m_centroid[i] += data[fvIndx][i];
	}
	float centroidSum=0,prevCentroidSum=0;
	for(int i=0;i<m_centroid.size();++i)
	{
		m_centroid[i]/=m_clusterData.size();
		centroidSum += m_centroid[i];
		prevCentroidSum += prevCentroid[i];
	}

	return std::abs((double)(centroidSum - prevCentroidSum)) > FLT_EPSILON ;
}
bool KMeans::Cluster::isExist(uint32_t featureVectIndx) const
{
	for(int i=0;i<m_clusterData.size();++i)
	{
		if (m_clusterData[i] == featureVectIndx)
			return true;
	}
	return false;
}
void KMeans::Cluster::clear()
{
	m_clusterData.clear();
}
