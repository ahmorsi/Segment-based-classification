#include <iostream>
#include <rv/ParameterList.h>
#include <rv/Laserscan.h>
#include <rv/Stopwatch.h>
#include <rv/Math.h>

#include "project/Octree.h"
#include "project/BagOfWordsDescriptor.h"
#include "project/SpinImage.h"
#include "project/SoftmaxRegression.h"
#include "project/utils.h"

using namespace rv;

/**
 * Basic processing pipeline for learning the classifier.
 *
 * This programs learns the classifier from segmented laser range scans by
 * reading the laser range scans with segments and labels, applying the Bag-of-Words descriptor on
 * each segment and then training the softmax regression on these examples.
 *
 */
int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Missing configuration file." << std::endl;
    std::cerr << "Usage: ./train-classifier <configuration>" << std::endl;
    return -1;
  }

  ParameterList params;
  parseXmlFile(argv[1], params);

  std::string scan_directory = params["scan-directory"];
  std::string model_directory = params["model-directory"];

  // 1. Initialize descriptors, vocabulary, and classifier.
  Octree oct(params["bucket-size"]);

  ParameterList bowParams = params["bag-of-words"];
  SpinImage si(bowParams["descriptor"]);
  std::vector<std::vector<float> > vocabulary;
  readVocabulary(model_directory + (std::string) bowParams["vocabulary-filename"], vocabulary);
  BagOfWordsDescriptor bow(bowParams, si, vocabulary);

  // get the mapping of string labels to label ids:
  ParameterList mappingParams = params["class-mapping"];
  std::map<std::string, uint16_t> label2id;
  std::map<uint16_t, std::string> id2label;
  parseMapping(mappingParams, label2id, id2label);

  // 2. compute features from training data and convert labels to "numerical" entries.

  DirectoryUtil dir(scan_directory);
  Laserscan scan;
  std::vector<IndexedSegment> segments;
  std::vector<std::string> original_labels;

  std::vector<std::vector<float> > features;
  std::vector<uint16_t> labels;

  const uint32_t numScans = dir.count();
  uint32_t scanNumber = 0;

  std::cout << "Computing features: " << std::flush;
  Stopwatch::tic();

  while (dir.hasNextFile())
  {
    printProgress(scanNumber++, numScans);

    dir.next();
    readLaserscan(dir.getLaserscanFilename(), scan);
    readSegments(dir.getSegmentFilename(), segments);
    readAnnotations(dir.getAnnotationFilename(), original_labels);

    std::vector<float> feature(bow.dim());

    for (uint32_t i = 0; i < segments.size(); ++i)
    {
      const IndexedSegment& segment = segments[i];

      oct.initialize(scan.points(), segment.indexes);
      bow.evaluate(&feature[0], segment, scan, oct);
      features.push_back(feature);

      assert(label2id.find(original_labels[i]) != label2id.end());
      labels.push_back(label2id[original_labels[i]]);
    }
  }

  printProgress(numScans, numScans);
  std::cout << "finished in " << Stopwatch::toc() << " s." << std::endl;

  // 3. optimize and store classifier.
  ParameterList classifierParams = params["classifier"];

  std::cout << "Learning classifier: " << std::flush;
  Stopwatch::tic();

  SoftmaxRegression sr;
  sr.setParameters(classifierParams);
  sr.train(features, labels);

  std::cout << "finished in " << Stopwatch::toc() << " s." << std::endl;

  sr.save(model_directory + (std::string) classifierParams["model-filename"], true);

  uint32_t wrong_predictions = 0;
  // 4. finally determine error on the train set.
  for (uint32_t i = 0; i < features.size(); ++i)
  {
    std::vector<float> prob;
    sr.classify(features[i], prob);
    if (Math::argmax(prob) != labels[i]) wrong_predictions += 1;
  }

  std::cout << "Error on trainset: " << (100.0 * float(wrong_predictions) / float(features.size())) << std::endl;

  return 0;
}
