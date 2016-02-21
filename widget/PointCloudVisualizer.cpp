#include "PointCloudVisualizer.h"

#include <sstream>
#include <fstream>

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <rv/geometry.h>
#include <rv/IndexedSegment.h>
#include <rv/string_utils.h>
#include <rv/Math.h>
#include <rv/BBox.h>

PointCloudVisualizer::PointCloudVisualizer()
{
  ui.setupUi(this);

  connect(ui.actionOpenLaserscans, SIGNAL(triggered()), this, SLOT(openLaserscans()));
  connect(ui.actionOpenSegments, SIGNAL(triggered()), this, SLOT(openSegments()));

  connect(ui.spinPointSize, SIGNAL(valueChanged(int)), ui.frmCanvas, SLOT(setPointSize(int)));

//  connect(ui.spinMaxDistance, SIGNAL(valueChanged(int)), ui.frmCanvas, SLOT(setMaxDistance(int)));
//  connect(ui.spinMinDistance, SIGNAL(valueChanged(int)), ui.frmCanvas, SLOT(setMinDistance(int)));

//  connect(ui.chkShowCoordAxes, SIGNAL(toggled(bool)), ui.frmCanvas, SLOT(setCoordinateAxesVisible(bool)));
//  connect(ui.chkShowGroundGrid, SIGNAL(toggled(bool)), ui.frmCanvas, SLOT(setGroundGridVisible(bool)));
  connect(ui.chkShowSegments, SIGNAL(toggled(bool)), ui.frmCanvas, SLOT(setSegmentsVisible(bool)));
  connect(ui.listScans, SIGNAL(itemSelectionChanged()), this, SLOT(selectLaserscan()));
  connect(ui.listSegments, SIGNAL(currentRowChanged(int)), ui.frmCanvas, SLOT(setSelectedSegment(int)));
  connect(ui.chkShowGroundTruth, SIGNAL(toggled(bool)), ui.frmCanvas, SLOT(setGroundTruthVisible(bool)));
}

PointCloudVisualizer::~PointCloudVisualizer()
{

}

void PointCloudVisualizer::openLaserscans()
{
  QString retValue = QFileDialog::getExistingDirectory(this, "Select Laserscan Directory", ".");

  if (!retValue.isNull())
  {
    std::string directoryName = retValue.toStdString();
    std::vector<std::string> filenames = getDirectoryListing(directoryName, ".bin");

    if (filenames.size() == 0) return;

    ui.listScans->clear();
    ui.listSegments->clear();

    laserscanFilenames_ = filenames;
    segmentFilenames_.clear();
    labelFilenames_.clear();
    groundTruthSegmentFilenames_.clear();
    groundTruthLabelFilenames_.clear();
    segmentFilenames_.resize(laserscanFilenames_.size());
    labelFilenames_.resize(laserscanFilenames_.size());
    groundTruthSegmentFilenames_.resize(laserscanFilenames_.size());
    groundTruthLabelFilenames_.resize(laserscanFilenames_.size());

    // find corresponding ground truth files...
    std::vector<std::string> seg_filenames = getDirectoryListing(directoryName, ".seg");
    std::vector<std::string> label_filenames = getDirectoryListing(directoryName, ".txt");

    if (seg_filenames.size() == laserscanFilenames_.size() && label_filenames.size() == laserscanFilenames_.size())
    {
      groundTruthSegmentFilenames_ = seg_filenames;
      groundTruthLabelFilenames_ = label_filenames;

      ui.statusbar->showMessage("Ground truth loaded...", 2000);
    }

    for (uint32_t i = 0; i < filenames.size(); ++i)
    {
      const std::string& filename = laserscanFilenames_[i];
      boost::filesystem::path p(filename);
      ui.listScans->addItem(QString::fromStdString(p.stem().string()));
    }

    ui.listScans->setCurrentRow(0);
  }
}

void PointCloudVisualizer::openSegments()
{
  if (laserscanFilenames_.size() == 0)
  {
    QMessageBox::information(this, "No laser scans loaded", "Please open the corresponding laser scans first.");
    return;
  }

  QString retValue = QFileDialog::getExistingDirectory(this, "Select Segment Directory", ".");

  if (!retValue.isNull())
  {
    // find segment files & possibly label files.
    std::string directoryName = retValue.toStdString();
    std::vector<std::string> filenames = getDirectoryListing(directoryName, ".seg");
    if (filenames.size() == 0) return;

    segmentFilenames_.clear();
    labelFilenames_.clear();
    segmentFilenames_.resize(laserscanFilenames_.size());
    labelFilenames_.resize(laserscanFilenames_.size());

    uint32_t lastFileIdx = 0;
    // try to find for each laser scan a segmentation file.
    // Assume that filenames are lexically sorted.
    for (uint32_t i = 0; i < laserscanFilenames_.size(); ++i)
    {
      std::string laserBasename = boost::filesystem::path(laserscanFilenames_[i]).stem().string();
      for (uint32_t j = lastFileIdx; j < filenames.size(); ++j)
      {
        lastFileIdx = j;
        std::string segmentBasename = boost::filesystem::path(filenames[j]).stem().string();

        if (laserBasename == segmentBasename)
        {
          segmentFilenames_[i] = filenames[j];
          break;
        }
      }
    }

    // label files reside usually in the same directory.
    filenames = getDirectoryListing(directoryName, ".txt");
    if (filenames.size() > 0)
    {
      lastFileIdx = 0;
      for (uint32_t i = 0; i < laserscanFilenames_.size(); ++i)
      {
        std::string laserBasename = boost::filesystem::path(laserscanFilenames_[i]).stem().string();
        for (uint32_t j = lastFileIdx; j < filenames.size(); ++j)
        {
          lastFileIdx = j;
          std::string segmentBasename = boost::filesystem::path(filenames[j]).stem().string();

          if (laserBasename == segmentBasename)
          {
            labelFilenames_[i] = filenames[j];
            break;
          }
        }
      }
    }

    // trigger update by selecting current scan...
    selectLaserscan();
  }
}

void PointCloudVisualizer::selectLaserscan()
{
  int32_t idx = ui.listScans->currentRow();
  if (idx >= 0)
  {
    Laserscan currentScan;
    std::vector<IndexedSegment> currentSegments;
    std::vector<BBox> segmentBBoxes;
    std::vector<std::string> currentLabels;
    std::vector<float> currentConfidences;

    ui.listSegments->clear();
    readLaserscan(laserscanFilenames_[idx], currentScan);

    // update segments if possible.
    if (segmentFilenames_[idx] != "") readSegments(segmentFilenames_[idx], currentSegments);
    if (labelFilenames_[idx] != "") readAnnotations(labelFilenames_[idx], currentLabels, currentConfidences);

    for (uint32_t i = 0; i < currentSegments.size(); ++i)
    {
      QListWidgetItem* item = new QListWidgetItem();
      segmentBBoxes.push_back(BBox(currentScan, currentSegments[i]));

      if (currentLabels.size() > i)
      {
        const std::string& label = currentLabels[i];
        QString caption;
        caption += QString::fromStdString(label);
        if (currentConfidences.size() > i) caption += " (" + QString::number(currentConfidences[i], 'f', 2) + ")";
        item->setText(caption);
        if (label == "Pedestrian")
          item->setTextColor(Qt::blue);
        else if (label == "Car")
          item->setTextColor(Qt::red);
        else if (label == "Cyclist")
          item->setTextColor(Qt::green);
        else
          item->setTextColor(Qt::black);

      }
      else
      {
        item->setText(QString("Segment"));
        item->setTextColor(Qt::black);
      }

      ui.listSegments->addItem(item);
    }

    ui.frmCanvas->setLaserscan(currentScan);
    ui.frmCanvas->setSegments(currentSegments, segmentBBoxes);
    ui.frmCanvas->setLabels(currentLabels);

    if (groundTruthLabelFilenames_.size() > idx && groundTruthSegmentFilenames_.size() > idx)
    {
      std::vector<IndexedSegment> gt_segments;
      std::vector<BBox> gt_bboxes;
      std::vector<std::string> gt_labels;
      std::vector<float> conf;

      readSegments(groundTruthSegmentFilenames_[idx], gt_segments);
      readAnnotations(groundTruthLabelFilenames_[idx], gt_labels, conf);

      for (uint32_t i = 0; i < gt_segments.size(); ++i)
      {
        gt_bboxes.push_back(BBox(currentScan, gt_segments[i]));
      }

      ui.frmCanvas->setGroundTruth(gt_bboxes, gt_labels);
    }

    ui.frmCanvas->updateGL();
  }
}

//void PointCloudVisualizer::selectSegment()
//{
//  int32_t idx = ui.frmCanvas->setSelectedSegment(int32_t)
//}

std::vector<std::string> PointCloudVisualizer::getDirectoryListing(const std::string& dirname,
    const std::string& extension)
{

  std::vector<std::string> filenames;
  boost::filesystem::path dir(dirname);
  if (!boost::filesystem::is_directory(dir)) return filenames;

  for (boost::filesystem::directory_iterator it = boost::filesystem::directory_iterator(dir);
      it != boost::filesystem::directory_iterator(); it++)
  {
    if (it->path().extension().string() == extension) filenames.push_back(it->path().string());
  }
  std::sort(filenames.begin(), filenames.end());

  return filenames;
}

void PointCloudVisualizer::readLaserscan(const std::string& filename, Laserscan& scan)
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

void PointCloudVisualizer::readSegments(const std::string& filename, std::vector<IndexedSegment>& segments)
{
  segments.clear();

  std::ifstream in(filename.c_str());
  if (!in.is_open()) throw IOError("Could not open segment file");

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

void PointCloudVisualizer::readAnnotations(const std::string& filename, std::vector<std::string>& labels,
    std::vector<float>& confidences)
{
  labels.clear();
  confidences.clear();

  std::ifstream in(filename.c_str());
  if (!in.is_open())
  {
    return;
  }

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

    labels.push_back(label);
    if (tokens.size() >= 16)
    {
      double score = boost::lexical_cast<double>(tokens[15]);
      confidences.push_back(score);
    }

    in.peek();
  }

  in.close();
}
