#ifndef CANVASGL_H_
#define CANVASGL_H_

#include <QtOpenGL/QGLWidget>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <rv/Laserscan.h>
#include <rv/IndexedSegment.h>
#include <rv/BBox.h>
#include "CameraGL.h"

using namespace rv;

/** \brief canvas which draws all the data.
 *
 *  \author behley
 */
class CanvasGL: public QGLWidget
{
  Q_OBJECT
  public:
    CanvasGL(QWidget* parent = 0, Qt::WindowFlags f = 0);

    void setLaserscan(const Laserscan& scan);
    void setLabels(const std::vector<std::string>& labels);
    void setSegments(const std::vector<IndexedSegment>& segments, const std::vector<BBox>& bboxes);
    void setGroundTruth(const std::vector<BBox>& bboxes, const std::vector<std::string>& labels);

  public slots:
    void setPointSize(int value);
    void setCoordinateAxesVisible(bool value);
    void setGroundGridVisible(bool value);
    void setSegmentsVisible(bool value);
    void setGroundTruthVisible(bool value);

    void setSelectedSegment(int idx);

    void setMinDistance(int value);
    void setMaxDistance(int value);

  protected slots:
    void moveCamera();
  protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void updateStaticScene();

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

    void drawCoordinateAxes();
    void drawGroundGrid();
    void drawLaserscan();
    void drawSegments();
    void drawGroundTruth();

    void drawBBox(const BBox& box);

    CameraGL camera_;
    QTimer moveTimer_;
    QTime lastTick_;
    float headingStart_, tiltStart_;
    bool slideMode_, changedView_;
    QPoint mouseStart_;
    int32_t sceneList_;
    bool updateStaticScene_;

    Laserscan* tempScan_;
    Laserscan* currentScan_;
    std::vector<IndexedSegment> segments_;
    std::vector<BBox> bboxes_;
    std::vector<std::string> labels_;
    uint32_t pointSize_;
    std::vector<BBox> groundTruth_;
    std::vector<std::string> groundTruthLabels_;

    /** flags for visual options. **/
    bool gridVisible_, coordsVisible_, segmentsVisible_, groundTruthVisible_;

    int32_t minDistance_, maxDistance_;

    int32_t selectedSegment_;
};

#endif /* CANVASGL_H_ */
