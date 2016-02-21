#ifndef KMEANS_H_
#define KMEANS_H_

#include <vector>
#include <stdint.h>


/** \brief 
 *
 *  \author you
 */
class KMeans
{
  public:
    KMeans();

    /** \brief cluster given data with given number of cluster centers.
     *
     *  \return returns cluster centers.
     **/
    std::vector<std::vector<float> > cluster(const std::vector<std::vector<float> >& data, uint32_t C);

  protected:


    // some helper methods:
    float distanceSqr(const std::vector<float>& a, const std::vector<float>& b) const;
    bool equal(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) const;

    class Cluster
    {
    public:
    	Cluster();
    	Cluster(uint32_t,const std::vector<float>&);
    	~Cluster();
    	uint32_t size() const;
    	std::vector<float> getCentroid() const;
    	void add(uint32_t);
        void clear();
        bool isExist(uint32_t) const;
        bool updateCentroid(const std::vector<std::vector<float> >& data);
    private:
        std::vector<uint32_t> m_clusterData;
        std::vector<float> m_centroid;

    };
    uint32_t getNearestCluster(const std::vector<KMeans::Cluster*>& K_Clusters,const std::vector<float>& fv) const;
};

#endif /* KMEANS_H_ */
