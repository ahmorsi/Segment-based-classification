#ifndef POINTCLOUDVISUALIZER_H_
#define POINTCLOUDVISUALIZER_H_

#include <stdint.h>

#include <QtGui/QMainWindow>
#include <QtCore/QTimer>

#include "ui_pviz.h"

using namespace rv;

/** \brief MainWindow of the Point Cloud Visualizer.
 *
 *  Handles all the input from the widget, opens files, etc. Somehow the "controller" of the app.
 *
 *  \author behley
 **/

class PointCloudVisualizer: public QMainWindow
{
  Q_OBJECT
  public:
    PointCloudVisualizer();
    ~PointCloudVisualizer();

  public slots:
    /** \brief opens laserscans from a directory **/
    void openLaserscans();
    /** \brief open semgents and labels(if available). **/
    void openSegments();

  protected slots:

    void selectLaserscan();

  protected:
    // replicating functionality from utils, just to be safe.
    std::vector<std::string> getDirectoryListing(const std::string& dirname, const std::string& extension);
    void readLaserscan(const std::string& filename, Laserscan& scan);
    void readSegments(const std::string& filename, std::vector<IndexedSegment>& segments);
    void readAnnotations(const std::string& filename, std::vector<std::string>& labels,
        std::vector<float>& confidences);

    Ui::MainWindow ui;

    // data.
    std::vector<std::string> laserscanFilenames_;
    std::vector<std::string> segmentFilenames_;
    std::vector<std::string> labelFilenames_;

    std::vector<std::string> groundTruthSegmentFilenames_;
    std::vector<std::string> groundTruthLabelFilenames_;

//    uint32_t currentSegmentId_;
//    uint32_t currentLaserscanId_;

//    Laserscan currentScan_;
//    std::vector<IndexedSegment> currentSegments_;
//    std::vector<std::string> currentLabels_;
//    std::vector<float> currentConfidences_;
};

#endif /* POINTCLOUDVISUALIZER_H_ */
