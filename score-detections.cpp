#include <stdint.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <rv/IndexedSegment.h>
#include <rv/Math.h>
#include <rv/string_utils.h>

#include "project/utils.h"

using namespace rv;
using namespace boost::filesystem;

bool writePlots = false;
std::string plot_directory;

struct Annotation
{
  public:
    std::string label;
    uint32_t occlusion;
    float probability;
};

void readAnnotations(const std::string& filename, std::vector<Annotation>& annotations)
{
  std::ifstream in(filename.c_str());
  if (!in.is_open())
  {
    std::stringstream sstr;
    sstr << "error while reading annotation from '" << filename << "'!" << std::endl;
    throw IOError(sstr.str());
  }

  annotations.clear();

  std::string line;
  std::vector<std::string> tokens;

  in.peek();
  while (!in.eof())
  {
    std::getline(in, line);
    tokens = split(line, " ", true); /** skip empty columns. **/
    if (tokens.size() == 0) /* empty line **/
    {
      in.peek();
      continue;
    }
    if (tokens.size() < 15) continue;

    Annotation a;
    a.label = tokens[0];

    a.occlusion = boost::lexical_cast<int32_t>(tokens[2]);

    if (tokens.size() >= 16) // we got a score.
    a.probability = boost::lexical_cast<float>(tokens[15]);

    annotations.push_back(a);

    in.peek();
  }

  in.close();
}

/** \brief meta-data for each detection that is needed to compute precision/recall **/
struct Detection
{
  public:
    bool operator<(const Detection& other) const
    {
      return (score < other.score);
    }

    float score;    // score of detection \in [0, 1]
    bool matched;   // does this detection correspond to a ground truth annotation
    float overlap;  // overlap between detection and ground truth annotation
    uint32_t idx;   // index of ground truth annotation.
};

float evalClass(const std::string& target, const std::string& gt_directory, const std::string& det_directory,
    uint32_t minPoints, uint32_t maxOcclusion, float minOverlap = 0.5)
{
  std::vector<std::string> gLabelFilenames = getDirectoryListing(gt_directory, ".txt");
  std::vector<std::string> gSegmentFilenames = getDirectoryListing(gt_directory, ".seg");
  std::vector<std::string> dLabelFilenames = getDirectoryListing(det_directory, ".txt");
  std::vector<std::string> dSegmentFilenames = getDirectoryListing(det_directory, ".seg");

  if (gLabelFilenames.size() != gSegmentFilenames.size()) throw Error(
      "Unequal number of segment files and annotation files. (Ground Truth)");
  if (dLabelFilenames.size() != dSegmentFilenames.size()) throw Error(
      "Unequal number of segment files and annotation files. (Detections)");
  if (gLabelFilenames.size() != dLabelFilenames.size()) throw Error("Missing detections for ground truth files.");

  // gathering statistics over all files.
  std::vector<Detection> detections;

  // number of ground truth segments with label fulfilling evaluation criterion.
  uint32_t numGtSegments = 0;

  std::vector<Annotation> gt_annotations;
  std::vector<IndexedSegment> gt_segments;
  std::vector<Annotation> det_annotations;
  std::vector<IndexedSegment> det_segments;

  const uint32_t POSITIVE = 1, NEGATIVE = 2, DONTCARE = 3;

  for (uint32_t f = 0; f < gLabelFilenames.size(); ++f)
  {
    readAnnotations(gLabelFilenames[f], gt_annotations);
    readSegments(gSegmentFilenames[f], gt_segments);

    // determine type of the ground truth segments, i.e.,
    //  POSITIVE = segments corresponding to target class with min points and max occlusion,
    //  NEGATIVE = non-target ground truth,
    //  DONTCARE = don't care regions that do not count as false positives or false negatives
    //             due to similarity to target class or difficulty.

    std::vector<uint32_t> type(gt_segments.size());
    for (uint32_t i = 0; i < gt_segments.size(); ++i)
    {
      if (gt_annotations[i].label == target)
      {
        if ((gt_segments[i].size() < minPoints) || (gt_annotations[i].occlusion > maxOcclusion))
        {
          type[i] = DONTCARE;
        }
        else
        {
          type[i] = POSITIVE;
          numGtSegments += 1; // count as positive example.
        }
      }
      else if ((target == "Pedestrian") && (gt_annotations[i].label == "PersonSitting"))
        type[i] = DONTCARE;
      else if ((target == "Car") && (gt_annotations[i].label == "Van"))
        type[i] = DONTCARE;
      else if (gt_annotations[i].label == "DontCare")
        type[i] = DONTCARE;
      else
        type[i] = NEGATIVE;

      assert(type[i] != 0);
    }

    // mapping from ground truth -> detection.
    std::vector<int32_t> matches(gt_segments.size(), -1);
    std::vector<float> max_overlaps(gt_segments.size(), -1.0f);

    readAnnotations(dLabelFilenames[f], det_annotations);
    readSegments(dSegmentFilenames[f], det_segments);

    std::vector<bool> assigned(det_segments.size(), false);

    // determine which detections match a ground truth annotation:
    // 1. detections d matches a ground truth g => overlap(d,g) >= minOverlap
    // 2. detections matching a "don't care" region do not count towards fp/tp (ignored)
    // 3. detections with y != target are ignored.
    for (uint32_t i = 0; i < gt_annotations.size(); ++i)
    {
      if (type[i] == NEGATIVE) continue; // negative ground truth don't need an overlapping detection!

      // determine maximal overlapping unassigned detection of target class
      for (uint32_t j = 0; j < det_annotations.size(); ++j)
      {
        if (assigned[j]) continue;
        if (det_annotations[j].label != target) continue;

        float o = overlap(gt_segments[i], det_segments[j]);
        if (o < minOverlap) continue;
        if (o > max_overlaps[i])
        {
          matches[i] = j;
          max_overlaps[i] = o;
        }
      }

      // we found a matching detection => this cannot be assigned again.
      if (matches[i] > -1) assigned[matches[i]] = true;
    }

    // true positives
    for (uint32_t i = 0; i < matches.size(); ++i)
    {
      if ((matches[i] == -1) || (type[i] != POSITIVE)) continue;

      Detection detection;
      detection.matched = true;
      detection.idx = i;
      detection.overlap = max_overlaps[i];
      detection.score = det_annotations[matches[i]].probability;
      detections.push_back(detection);
    }

    // false positives == unassigned detections
    for (uint32_t j = 0; j < assigned.size(); ++j)
    {
      if (assigned[j] || (det_annotations[j].label != target) || det_segments[j].size() < minPoints) continue;

      Detection detection;
      detection.matched = false;
      detection.score = det_annotations[j].probability;
      detections.push_back(detection);
    }
  }

  // sort detections descending
  std::sort(detections.begin(), detections.end());
  std::reverse(detections.begin(), detections.end());

  std::cout << detections.size() << " detections extracted vs. " << numGtSegments << " ground truth segments."
      << std::endl;

  const uint32_t NUM_SAMPLES = 11;
  std::vector<float> avg_recalls(NUM_SAMPLES);
  std::vector<float> avg_precisions(NUM_SAMPLES);

  std::vector<float> precisions;
  std::vector<float> recalls;

  float curr_recall = 0;
  uint32_t tp = 0, fp = 0;
  uint32_t curr_det = 0;
  for (uint32_t i = 0; i < NUM_SAMPLES; ++i)
  {
    avg_recalls[i] = curr_recall;
    if (curr_det < detections.size())
    {

      // iterate over detections, while below current recall level...
      while (((float) tp / (float) numGtSegments) < curr_recall)
      {
        if (curr_det >= detections.size()) break;
        if (detections[curr_det].matched) tp += 1;
        if (!detections[curr_det].matched) fp += 1;

        float r = (float) tp / (float) numGtSegments;
        float p = (float) (tp) / (float) (tp + fp);

        recalls.push_back(r);
        precisions.push_back(p);

        curr_det += 1;
      }

      // recall at current precision.
      if ((tp + fp) == 0)
        avg_precisions[i] = 0.0f;
      else
        avg_precisions[i] = float(tp) / (float) (tp + fp);
    }

    curr_recall += 1.0f / float(NUM_SAMPLES - 1);
  }

  // generate interpolated precisions
  for (uint32_t i = 0; i < NUM_SAMPLES; ++i)
  {
    avg_precisions[i] = *std::max_element(avg_precisions.begin() + i, avg_precisions.end());
  }

  for (uint32_t i = 0; i < precisions.size(); ++i)
  {
    precisions[i] = *std::max_element(precisions.begin() + i, precisions.end());
  }

  if (writePlots)
  {
    std::string plot_name = plot_directory + target + "_averaged.dat";
    std::ofstream out(plot_name.c_str());
    for (uint32_t i = 0; i < avg_precisions.size(); ++i)
      out << avg_recalls[i] << " " << avg_precisions[i] << std::endl;
    out.close();

    plot_name = plot_directory + target + "_complete.dat";
    out.open(plot_name.c_str());

    for (uint32_t i = 0; i < precisions.size(); ++i)
    {
      out << recalls[i] << " " << precisions[i] << std::endl;
    }

    out.close();
  }

  return (1.f / float(NUM_SAMPLES) * Math::sum(avg_precisions.begin(), avg_precisions.end()));
}

/** \brief scoring of generated detections using Mean Average Precision (MAP)
 *
 * Usage: ./score <gt-directory> <result-directory>
 */
int32_t main(int32_t argc, char** argv)
{
  std::cout << "score v0.1" << std::endl;
  if (argc < 3)
  {
    std::cerr << "Missing arguments." << std::endl;
    std::cerr << "Usage: ./score <ground truth directory> <prediction directory> [<plot directory>]" << std::endl;

    return -1;
  }

  std::string gt_directory = argv[1];
  std::string det_directory = argv[2];
  if (argc > 3) // optional parameter.
  {
    writePlots = true;
    plot_directory = argv[3];
  }

// In contrast to the KITTI vision benchmark, we consider only ground truth annotations, which are
// (a) at most partially visible and (b) contain > 50 laser points. (similar to the "medium" category)

  uint32_t minPoints = 50;
  uint32_t maxOcclusion = 1;
  float minOverlap = 0.5;

  float AP_car = evalClass("Car", gt_directory, det_directory, minPoints, maxOcclusion, minOverlap);
  float AP_pedestrian = evalClass("Pedestrian", gt_directory, det_directory, minPoints, maxOcclusion, minOverlap);
  float AP_cyclist = evalClass("Cyclist", gt_directory, det_directory, minPoints, maxOcclusion, minOverlap);

  std::cout << "Average Precision: Car = " << (100.f * AP_car) << " %, Pedestrian = " << (100.f * AP_pedestrian)
      << " %, Cyclist = " << (100.f * AP_cyclist) << " %" << std::endl;

  return 0;
}
