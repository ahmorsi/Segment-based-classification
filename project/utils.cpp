#include "utils.h"

#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <rv/IOError.h>
#include <rv/string_utils.h>

using namespace boost::filesystem;
using namespace rv;

DirectoryUtil::DirectoryUtil(const std::string& directoryName) :
    currentIndex_(-1)
{
  // find filenames in directory.
  path dir(directoryName);
  if (!exists(dir)) throw IOError("Directory does not exist.");
  if (is_regular_file(dir)) dir = dir.parent_path();
  if (!is_directory(dir)) throw IOError("Given directory name is not a directory.");
  dirName_ = dir.string();

  for (directory_iterator it = directory_iterator(dir); it != directory_iterator(); it++)
  {
    if (it->path().extension().string() == ".bin") scannames_.push_back(it->path().stem().string());
  }

  std::sort(scannames_.begin(), scannames_.end());
}

uint32_t DirectoryUtil::count() const
{
  return scannames_.size();
}

bool DirectoryUtil::hasNextFile() const
{
  return (currentIndex_ + 1 < (int32_t) scannames_.size());
}

/** \brief iterate to next laser range scan file. **/
void DirectoryUtil::next()
{
  currentIndex_ += 1;
}

std::string DirectoryUtil::getLaserscanFilename() const
{
  return getLaserscanFilename(dirName_);
}

std::string DirectoryUtil::getAnnotationFilename() const
{
  return getAnnotationFilename(dirName_);
}

std::string DirectoryUtil::getSegmentFilename() const
{
  return getSegmentFilename(dirName_);
}

/** \brief build next segment filename with given directory. **/
std::string DirectoryUtil::getSegmentFilename(const std::string& dirname) const
{
  if (currentIndex_ < 0 || currentIndex_ >= scannames_.size()) throw Error("Invalid directory entry.");
  path file(dirname);
  file /= scannames_[currentIndex_] + ".seg";
  return file.string();
}

std::string DirectoryUtil::getLaserscanFilename(const std::string& dirname) const
{
  if (currentIndex_ < 0 || currentIndex_ >= scannames_.size()) throw Error("Invalid directory entry.");

  path file(dirname);
  file /= scannames_[currentIndex_] + ".bin";
  return file.string();
}

std::string DirectoryUtil::getAnnotationFilename(const std::string& dirname) const
{
  if (currentIndex_ < 0 || currentIndex_ >= scannames_.size()) throw Error("Invalid directory entry.");

  path file(dirname);
  file /= scannames_[currentIndex_] + ".txt";
  return file.string();
}

std::vector<std::string> getDirectoryListing(const std::string& dirname)
{
  std::vector<std::string> filenames;
  path dir(dirname);
  if (!is_directory(dir)) return filenames;

  for (directory_iterator it = directory_iterator(dir); it != directory_iterator(); it++)
    filenames.push_back(it->path().string());

  std::sort(filenames.begin(), filenames.end());

  return filenames;
}

std::vector<std::string> getDirectoryListing(const std::string& dirname, const std::string& extension)
{
  std::vector<std::string> filenames;
  path dir(dirname);
  if (!is_directory(dir)) return filenames;

  for (directory_iterator it = directory_iterator(dir); it != directory_iterator(); it++)
  {
    if (it->path().extension().string() == extension) filenames.push_back(it->path().string());
  }

  std::sort(filenames.begin(), filenames.end());

  return filenames;
}

std::string stripExtension(const std::string& path, uint32_t level)
{
  std::string::size_type slash = path.rfind('/');
  if (slash == std::string::npos) slash = 0;
  std::string::size_type ext = path.rfind('.');
  if (ext == std::string::npos || ext < slash) return path;
  while (level > 1)
  {
    if (ext == 0) break;
    std::string::size_type dot = path.rfind('.', ext - 1);
    if (dot == std::string::npos || dot < slash) break;
    ext = dot;
    --level;
  }
  return path.substr(0, ext);
}

/** \brief read binary laser range scan from given filename. **/
void readLaserscan(const std::string& filename, rv::Laserscan& scan)
{
  std::ifstream in(filename.c_str(), std::ios::binary);
  if (!in.is_open()) throw IOError("Could not open laser scan file");

  scan.clear();

  in.seekg(0, std::ios::end);/** seeking to the end. **/
  uint32_t num_points = in.tellg() / (4 * sizeof(float));
  in.seekg(0, std::ios::beg);/** and again to the beginning. **/

  std::vector<float> values(4 * num_points);
  in.read((char*) &values[0], 4 * num_points * sizeof(float));

  in.close(); /** done with reading. **/

  std::vector<Point3f>& points = scan.points();
  std::vector<float>& remissions = scan.remissions();

  points.resize(num_points);
  remissions.resize(num_points);

  for (uint32_t i = 0; i < num_points; ++i)
  {
    points[i].x() = values[4 * i];
    points[i].y() = values[4 * i + 1];
    points[i].z() = values[4 * i + 2];
    remissions[i] = values[4 * i + 3];
  }

}

/** \brief read binary segments from given filename. **/
void readSegments(const std::string& filename, std::vector<rv::IndexedSegment>& segments)
{
  std::ifstream in(filename.c_str());
  if (!in.is_open()) throw IOError("Could not open segment file");

  segments.clear();

  std::string line;
  std::getline(in, line);
  std::vector<std::string> tokens = split(line, ":");

  // version string:
  if (tokens.size() < 2) throw IOError("Invalid segments file" + filename);
  if (tokens[0] != "SEG" && tokens[1] != "1.0") throw IOError("Invalid segments file" + filename);
  uint32_t num_segments = boost::lexical_cast<uint32_t>(tokens[2]);

  in.peek();
  for (uint32_t i = 0; i < num_segments; ++i)
  {
    if (!in.good()) throw IOError("Error while reading segment from file.");

    IndexedSegment s;
    uint32_t size;

    in.read((char*) &size, sizeof(uint32_t));
    s.indexes.resize(size);

    in.read((char*) &s.indexes[0], size * sizeof(uint32_t));

    segments.push_back(s);
    in.peek();
  }

  in.close();
}

/** \brief read annotation from given filename. **/
void readAnnotations(const std::string& filename, std::vector<std::string>& labels)
{
  std::ifstream in(filename.c_str());
  if (!in.is_open())
  {
    std::stringstream sstr;
    sstr << "error while reading annotation from '" << filename << "'!" << std::endl;
    throw IOError(sstr.str());
  }

  labels.clear();

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

    std::string label = tokens[0];
    // The remaining tokens contain other annotations.
//    truncated = boost::lexical_cast<double>(tokens[1]);
//    occluded = boost::lexical_cast<int32_t> (tokens[2]);
//    alpha = boost::lexical_cast<double>(tokens[3]);
//
//    float left = boost::lexical_cast<double>(tokens[4]);
//    float top = boost::lexical_cast<double>(tokens[5]);
//    float right = boost::lexical_cast<double>(tokens[6]);
//    float bottom = boost::lexical_cast<double>(tokens[7]);
//
//    bbox2d = BBox2D(left, top, right, bottom);
//
//    float height = boost::lexical_cast<double>(tokens[8]);
//    float width = boost::lexical_cast<double>(tokens[9]);
//    float length = boost::lexical_cast<double>(tokens[10]);
//
//    Vector3f extent(0.5f * length, 0.5f * height, 0.5f * width);
//    float x = boost::lexical_cast<double>(tokens[11]);
//    float y = boost::lexical_cast<double>(tokens[12]);
//    float z = boost::lexical_cast<double>(tokens[13]);
//    float rot = boost::lexical_cast<double>(tokens[14]);
//
//    a.x = x;
//    a.y = y;
//    a.z = z;
//    a.r_y = rot;
//
//    a.w = width;
//    a.h = height;
//    a.l = length;
//
//    Transform t = Translate(Vector3f(x, y - 0.5 * height, z)) * RotateY(Math::rad2deg(rot));
//
//    a.bbox3d = rv2::BBox(t, extent);
//    if (tokens.size() >= 16) // we got a score.
//    score = boost::lexical_cast<double>(tokens[15]);

    labels.push_back(label);

    in.peek();
  }

  in.close();
}

/** \brief write the given segments to the file with filename. **/
void writeSegments(const std::string& filename, const std::vector<rv::IndexedSegment>& segments)
{
  std::ofstream out(filename.c_str());
  if (!out.is_open()) throw IOError("Could not open segment file");

  // version string:
  out << "SEG:1.0:" << segments.size() << std::endl;

  for (uint32_t i = 0; i < segments.size(); ++i)
  {
    const IndexedSegment& segment = segments[i];
    uint32_t size = segment.indexes.size();
    out.write((const char*) &size, sizeof(uint32_t));
    out.write((const char*) &segment.indexes[0], size * sizeof(uint32_t));
  }

  out.close();
}

/** \brief write annotation to given filename. **/
void writeAnnotations(const std::string& filename, const std::vector<std::string>& labels,
    const std::vector<float>& prob)
{
  if (labels.size() != prob.size()) throw Error("Size of labels and probabilities different!");
  std::ofstream out(filename.c_str());
  if (!out.is_open()) throw IOError("Unable to open annotation file for output.");

  for (uint32_t i = 0; i < labels.size(); ++i)
  {
    // KITTI compliant format (however, 2d bbox is missing...)
    out << labels[i] << " 0.0 0 0.0 0 0 0 0 0 0 0 0 0.0 0.0 0.0 " << prob[i] << std::endl;
  }
  out.close();
}

void readVocabulary(const std::string& filename, std::vector<std::vector<float> >& vocabulary)
{
  std::ifstream in(filename.c_str());

  if (!in.is_open()) throw IOError("Unable to open vocabulary file.");

  std::string line;
  std::getline(in, line);
  std::vector<std::string> tokens = split(line, ":");
  if (tokens.size() != 4) throw IOError("Invalid vocabulary file.");
  if (tokens[1] != "1.0") throw IOError("Unknown version of vocabulary file.");

  const uint32_t M = boost::lexical_cast<uint32_t>(tokens[2]);
  const uint32_t D = boost::lexical_cast<uint32_t>(tokens[3]);

  vocabulary.clear();
  vocabulary.reserve(M);
  std::vector<float> word(D);
  for (uint32_t i = 0; i < M; ++i)
  {
    in.read((char*) &word[0], D * sizeof(float));
    vocabulary.push_back(word);
  }

  in.close();
}

void writeVocabulary(const std::string& filename, const std::vector<std::vector<float> >& vocabulary)
{
  std::ofstream out(filename.c_str());

  if (!out.is_open()) throw IOError("Unable to open vocabulary file.");
  if (vocabulary.size() == 0)
  {
    out << "VOC:1.0:0:0" << std::endl;
    out.close();
    return;
  }

  const uint32_t M = vocabulary.size();
  const uint32_t D = vocabulary[0].size();

  out << "VOC:1.0:" << M << ":" << D << std::endl;
  for (uint32_t i = 0; i < M; ++i)
    out.write((const char*) &vocabulary[i][0], D * sizeof(float));

  out.close();
}

void parseMapping(const ParameterList& params, std::map<std::string, uint16_t>& label2id,
    std::map<uint16_t, std::string>& id2label)
{
  for (ParameterList::const_iterator it = params.begin(); it != params.end(); ++it)
  {
    std::string mapping = *it;
    std::vector<std::string> tokens = split(mapping, ":");
    std::string label = tokens[0];
    uint16_t id = boost::lexical_cast<uint16_t>(tokens[1]);
    label2id[label] = id;
    id2label[id] = label;
  }
}

void printProgress(uint32_t scan, uint32_t totalScans)
{
  static uint32_t progress = 10;
  while (progress < 100.f * scan / totalScans)
  {
    std::cout << progress << "% " << std::flush;
    progress += 10;
  }
}

float overlap(const IndexedSegment& first, const IndexedSegment& second)
{
  // Intersection/Union perform merge on sorted indexes.
  std::vector<uint32_t> a = first.indexes;
  std::vector<uint32_t> b = second.indexes;
  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());

  uint32_t i = 0, j = 0;
  uint32_t Intersection = 0, Union = 0;

  while ((i < a.size()) && (j < b.size()))
  {
    Union += 1;

    if (a[i] == b[j]) // both list must be advanced by 1.
    {
      Intersection += 1;
      ++i;
      ++j;
    }
    else if (a[i] < b[j])
    {
      ++i;
    }
    else if (a[i] > b[j])
    {
      ++j;
    }
  }

  // all remaining points are also in the union.
  Union += (a.size() - i) + (b.size() - j);

  if (Union == 0) return 0.0f;

  return float(Intersection) / float(Union);
}

