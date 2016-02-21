#include <rv/ParameterList.h>
#include <rv/Random.h>
#include <rv/Stopwatch.h>
#include <rv/Math.h>

#include "project/Octree.h"
#include "project/KMeans.h"
#include "project/SpinImage.h"
#include "project/utils.h"

using namespace rv;

int main(int32_t argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "missing configuration file." << std::endl;
    return -1;
  }

  ParameterList params;
  parseXmlFile(argv[1], params);

  std::string scan_directory = params["scan-directory"];
  std::string model_directory = params["model-directory"];
  std::string vocabulary_filename = "vocabulary.dat";
  if (argc == 3) vocabulary_filename = argv[2];

  ParameterList bowParams = params["bag-of-words"];
  uint32_t num_words = bowParams["num words"];
  uint32_t sample_size = bowParams["num samples"];

  ParameterList descriptorParams = bowParams["descriptor"];
  SpinImage si(descriptorParams);
  Normalizer* normalizer = getNormalizerByName(descriptorParams["normalizer"]);

  std::vector<std::vector<float> > sampled_descriptors;
  sampled_descriptors.reserve(sample_size);

  std::vector<float> feature(si.dim());

  std::cout << "Sampling of descriptors..." << std::flush;
  Stopwatch::tic();

  // to keep things somehow simple, we simply sample equally from each each scan.
  DirectoryUtil dir(scan_directory);
  uint32_t samples_per_scan = sample_size / dir.count();

  Laserscan scan;
  Octree oct;
  std::vector<IndexedSegment> segments;
  Normal3f upvector(0., 0., 1.);

  Random rand(1122);
  while (dir.hasNextFile())
  {
    dir.next();
    readLaserscan(dir.getLaserscanFilename(), scan);
    readSegments(dir.getSegmentFilename(), segments);

    uint32_t num_sampled = 0;
    const uint32_t samples_per_segment = samples_per_segment / segments.size();
    for (uint32_t i = 0; i < segments.size(); ++i)
    {
      const IndexedSegment& segment = segments[i];
      oct.initialize(scan.points(), segment.indexes);

      std::vector<uint32_t> idxes = rand.sample(Math::range(segment.size()), samples_per_segment);
      for (uint32_t s = 0; s < samples_per_segment; ++s)
      {
        const Point3f& p = scan.point(segment[idxes[s]]);
        si.evaluate(&feature[0], p, upvector, scan, oct);
        normalizer->normalize(&feature[0], si.dim());
        sampled_descriptors.push_back(feature);

        num_sampled += 1;
      }
    }

    // fill with randomly sampled descriptors.
    while (num_sampled < samples_per_scan)
    {
      const IndexedSegment& segment = segments[rand.getInt(segments.size())];
      oct.initialize(scan.points(), segment.indexes);

      const Point3f& p = scan.point(segment[rand.getInt(segment.size())]);
      si.evaluate(&feature[0], p, upvector, scan, oct);
      normalizer->normalize(&feature[0], si.dim());

      sampled_descriptors.push_back(feature);
      num_sampled += 1;
    }
  }

// simply fill with random descriptors from last scan.
  while (sampled_descriptors.size() < sample_size)
  {
    const IndexedSegment& segment = segments[rand.getInt(segments.size())];
    oct.initialize(scan.points(), segment.indexes);

    const Point3f& p = scan.point(segment[rand.getInt(segment.size())]);
    si.evaluate(&feature[0], p, upvector, scan, oct);
    normalizer->normalize(&feature[0], si.dim());

    sampled_descriptors.push_back(feature);
  }
  std::cout << "finished in " << Stopwatch::toc() << " s." << std::endl;

  std::cout << "Learning vocabulary from " << sampled_descriptors.size() << " descriptors..." << std::flush;
  Stopwatch::tic();
  KMeans kmeans;
  std::vector<std::vector<float> > vocabulary = kmeans.cluster(sampled_descriptors, num_words);
  std::cout << "finished in " << Stopwatch::toc() << " s." << std::endl;

  std::string voc_filename = model_directory + vocabulary_filename;
  std::cout << "Writing vocabulary to '" << voc_filename << "'!" << std::endl;
  writeVocabulary(voc_filename, vocabulary);

  return 0;
}
