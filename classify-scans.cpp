#include <rv/ParameterList.h>
#include <rv/geometry.h>
#include <rv/string_utils.h>
#include <rv/Math.h>
#include <rv/Stopwatch.h>

#include <boost/filesystem.hpp>

#include "project/utils.h"
#include "project/Octree.h"
#include "project/SpinImage.h"
#include "project/BagOfWordsDescriptor.h"
#include "project/GridbasedSegmentation.h"
#include "project/SoftmaxRegression.h"

using namespace rv;
using namespace boost::filesystem;

/**
 * Basic implementation of the scan processing with
 *  1. reading of each scan from given input directory,
 *  2. segmentation of each scan,
 *  3. computation of segment features,
 *  4. classification of each segment,
 *  5. writing of resulting annotation in given result directory.
 *
 */
int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Missing arguments: ./classify-scans <configuration>" << std::endl;
    return -1;
  }

  ParameterList params;
  parseXmlFile(argv[1], params);

  DirectoryUtil dir(params["scan-directory"]);
  std::string model_directory(params["model-directory"]);

  // 1. Initialize descriptors, vocabulary, and classifier from configuration file.
  Octree oct;

  GridbasedSegmentation seg(params["segmentation"]);

  ParameterList bowParams = params["bag-of-words"];
  SpinImage si(bowParams["descriptor"]);
  std::vector<std::vector<float> > vocabulary;
  std::string voc_filename = model_directory + (std::string) bowParams["vocabulary-filename"];
  readVocabulary(voc_filename, vocabulary);
  BagOfWordsDescriptor bow(bowParams, si, vocabulary);

  ParameterList classifierParams = params["classifier"];
  SoftmaxRegression sr;
  std::string classifier_filename = model_directory + (std::string) classifierParams["model-filename"];
  sr.load(classifier_filename);

  ParameterList mappingParams = params["class-mapping"];
  std::map<std::string, uint16_t> label2id;
  std::map<uint16_t, std::string> id2label;
  parseMapping(mappingParams, label2id, id2label);

  std::string result_directory = params["result-directory"];
  if (!exists(result_directory)) create_directories(result_directory);

  Laserscan scan;
  std::vector<IndexedSegment> segments;
  std::vector<float> segment_feature(bow.dim());
  std::vector<float> prob(sr.numClasses());
  const uint32_t numScans = dir.count();
  uint32_t scanNumber = 0;

  Stopwatch::tic();
  std::cout << "Classifying scans: " << std::flush;
  // 2. Read each scan, segment scan, and classify segments.
  while (dir.hasNextFile())
  {
    printProgress(scanNumber++, numScans);

    dir.next();
    std::string filename_laser = dir.getLaserscanFilename();

    readLaserscan(filename_laser, scan);

    seg.segment(scan, segments);
    // readSegments(dir.getSegmentFilename(), segments);

    std::vector<std::string> labels;
    std::vector<float> probabilities;

    for (uint32_t i = 0; i < segments.size(); ++i)
    {
      oct.initialize(scan.points(), segments[i].indexes);

      bow.evaluate(&segment_feature[0], segments[i], scan, oct);

      sr.classify(segment_feature, prob);
      // determine y* = argmax_y P(y|x)
      uint32_t max_id = Math::argmax(prob);
      labels.push_back(id2label[max_id]);
      probabilities.push_back(prob[max_id]);
    }

    std::string out_segments = dir.getSegmentFilename(result_directory);
    std::string out_labels = dir.getAnnotationFilename(result_directory);

    writeSegments(out_segments, segments);
    writeAnnotations(out_labels, labels, probabilities);
  }
  printProgress(numScans, numScans);
  std::cout << "finished in " << Stopwatch::toc() << " s." << std::endl;

  return 0;
}
