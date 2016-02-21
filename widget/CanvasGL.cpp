#include "CanvasGL.h"
#include <rv/ColorGL.h>
#include <rv/CoordinateSystems.h>
#include <rv/opengl_utils.h>

#include <QtGui/QMouseEvent>
#include <GL/glu.h>
#include <rv/Math.h>

// trick 17: swallowing the ';' by using the do-while(0)-statement.
#define PRINT(exp)  \
  do  \
  { \
    std::cout << #exp << " = "<< (exp) << std::endl; \
  } \
  while(0)

CanvasGL::CanvasGL(QWidget* parent, Qt::WindowFlags f) :
    QGLWidget(parent, 0, f), slideMode_(false), changedView_(false), updateStaticScene_(true), tempScan_(0), currentScan_(
        0), pointSize_(1), gridVisible_(true), coordsVisible_(true), segmentsVisible_(false), groundTruthVisible_(true), minDistance_(
        0), maxDistance_(100), selectedSegment_(-1)
{
  connect(&moveTimer_, SIGNAL(timeout()), this, SLOT(moveCamera()));
  moveTimer_.setSingleShot(false);
  setFocusPolicy(Qt::ClickFocus);
}

void CanvasGL::moveCamera()
{
  QTime tick = QTime::currentTime();
  if (tick > lastTick_)
  {
    float diff = lastTick_.msecsTo(tick) / 1000.f;
    camera_.applyVelocity(diff);
    updateGL();
    lastTick_ = tick;
  }
}

void CanvasGL::mousePressEvent(QMouseEvent* e)
{

  mouseStart_ = e->pos();
  headingStart_ = camera_.yaw();
  tiltStart_ = camera_.pitch();
  slideMode_ = e->modifiers() & Qt::ShiftModifier;
  lastTick_ = QTime::currentTime();

  e->accept();
}

/*****************************************************************************/

void CanvasGL::mouseReleaseEvent(QMouseEvent* e)
{

  camera_.setVelocity(0.0f, 0.0f, 0.0f, 0.0f);
  if (moveTimer_.isActive()) moveTimer_.stop();

  e->accept();
}

void CanvasGL::mouseMoveEvent(QMouseEvent* e)
{

  static const int MIN_MOVE = 20;
  static const float WALK_SENSITIVITY = -0.1f;
  static const float TURN_SENSITIVITY = -0.01f;
  static const float SLIDE_SENSITIVITY = 0.1f;
  static const float RAISE_SENSITIVITY = -0.1f;
  static const float LOOK_SENSITIVITY = -0.01f;
  static const float FREE_TURN_SENSITIVITY = -0.01f;

  int32_t dx = e->pos().x() - mouseStart_.x();
  int32_t dy = e->pos().y() - mouseStart_.y();
  bool changedView = false;
  if (abs(dx) < MIN_MOVE)
  {
    dx = 0;
  }
  else
  {
    if (dx > 0)
      dx -= MIN_MOVE;
    else
      dx += MIN_MOVE;
  }
  if (abs(dy) < MIN_MOVE)
  {
    dy = 0;
  }
  else
  {
    if (dy > 0)
      dy -= MIN_MOVE;
    else
      dy += MIN_MOVE;
  }
  if (e->buttons() & Qt::LeftButton)
  {
    camera_.setVelocity(WALK_SENSITIVITY * dy, 0.0f, 0.0f, TURN_SENSITIVITY * dx);
  }
  else if (e->buttons() & Qt::MidButton)
  {
    camera_.setVelocity(0.0f, RAISE_SENSITIVITY * dy, SLIDE_SENSITIVITY * dx, 0.0f);
  }
  else if (e->buttons() & Qt::RightButton)
  {
    if (slideMode_)
    {
      camera_.setVelocity(0.0, RAISE_SENSITIVITY * dy, SLIDE_SENSITIVITY * dx, 0.0f);
    }
    else
    {
      camera_.setVelocity(0.0f, 0.0f, 0.0f, 0.0f);
      float turn = FREE_TURN_SENSITIVITY * (e->pos().x() - mouseStart_.x());
      float tilt = LOOK_SENSITIVITY * (e->pos().y() - mouseStart_.y());
      camera_.setYaw(headingStart_ + turn);
      camera_.setPitch(tiltStart_ + tilt);
      changedView = true;
    }
  }
  else
  {
    camera_.setVelocity(0.0f, 0.0f, 0.0f, 0.0f);
  }

  if (camera_.hasVelocity())
  {
    if (!moveTimer_.isActive()) moveTimer_.start();
  }
  else
  {
    if (moveTimer_.isActive()) moveTimer_.stop();
    if (changedView) updateGL();
  }

  e->accept();
}

void CanvasGL::setPointSize(int value)
{
  pointSize_ = std::max(1, value);
  updateGL();
}

void CanvasGL::setLaserscan(const Laserscan& scan)
{
  /** copy laser range scan **/
  tempScan_ = new Laserscan(scan);
}

void CanvasGL::setCoordinateAxesVisible(bool value)
{
  coordsVisible_ = value;
  updateStaticScene_ = true;
  updateGL();
}

void CanvasGL::setGroundGridVisible(bool value)
{
  gridVisible_ = value;
  updateStaticScene_ = true;
  updateGL();
}

void CanvasGL::setSegmentsVisible(bool value)
{
  segmentsVisible_ = value;
  updateGL();
}

void CanvasGL::setGroundTruthVisible(bool value)
{
  groundTruthVisible_ = value;
  updateGL();
}

void CanvasGL::setMinDistance(int value)
{
  minDistance_ = value;
  updateGL();
}

void CanvasGL::setMaxDistance(int value)
{
  maxDistance_ = value;
  updateGL();
}

void CanvasGL::initializeGL()
{
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_LIGHTING);
  glClearDepth(1.0f); // Depth Buffer Setup
  glDepthFunc(GL_LEQUAL); // Type Of Depth Testing
  glEnable(GL_DEPTH_TEST);

  /** anti-aliasing and nicer renderings **/
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  sceneList_ = glGenLists(1);

  int32_t listId = 1000;
}

void CanvasGL::resizeGL(int width, int height)
{
  static const double FOV = 45.0;
//  static const double ASPECT_RATIO = 4.0 / 3.0;
  double ASPECT_RATIO = (double) width / (double) height;
  static const double MIN_Z = 0.1, MAX_Z = 327.68;
  static const float LIGHT_POS[] =
  { 0.0f, 1.0f, 0.0f, 1.0f };

  glViewport(0, 0, width, height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POS);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOV, ASPECT_RATIO, MIN_Z, MAX_Z);
}

void CanvasGL::paintGL()
{

  /** fast copy...**/
  if (tempScan_ != 0)
  {
    delete currentScan_;
    currentScan_ = tempScan_;
    tempScan_ = 0;
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (sceneList_ != 0 && updateStaticScene_)
  {
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glNewList(sceneList_, GL_COMPILE);
//    if (gridVisible_) drawGroundGrid();
    if (coordsVisible_) drawCoordinateAxes();

    glEndList();
    glPopAttrib();
    updateStaticScene_ = false;
  }

  camera_.applyGL();

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  if (sceneList_ != 0)
    glCallList(sceneList_);
  else
  {
//    if (gridVisible_) drawGroundGrid();
    if (coordsVisible_) drawCoordinateAxes();
  }

  drawLaserscan();
  if (groundTruthVisible_) drawGroundTruth();
  if (segmentsVisible_) drawSegments();

  glPopAttrib();
}

void CanvasGL::drawCoordinateAxes()
{
// coordinate system
  glPushMatrix();
  glMultMatrixf(RoSe2GL);

  glBegin(GL_LINES);
  glColor3ub(255, 0, 0); // red
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(5.0f, 0.0f, 0.0f); // x-axis
  glColor3ub(0, 255, 0); // green
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 5.0f, 0.0f); // y-axis
  glColor3ub(0, 0, 255); // blue
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 5.0f); // z-axis
  glEnd();

  glPopMatrix();
}

void CanvasGL::drawGroundGrid()
{
// ground grid
  glColor3f(0.25f, 0.25f, 0.0f);
  for (int y = -500; y < 500; y += 10)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUAD_STRIP);
    for (int x = -500; x < 500; x += 10)
    {
      glVertex3f(-y, -2.0f, -x);
      glVertex3f(-y - 10, -2.0f, -x);
    }
    glEnd();
  }
}

void CanvasGL::drawLaserscan()
{
  if (currentScan_ == 0) return;

  glPushMatrix();

  /** important: the conversion from RoSe to OpenGL coordinates. **/
  glMultMatrixf(RoSe2GL);
  /** the Velodyne pose **/
  glMultMatrixf(currentScan_->pose());

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glPointSize(pointSize_);

  /** draw points from scan **/
  glBegin(GL_POINTS);
  glColor3f(0, 0, 0);

  for (uint32_t i = 0; i < currentScan_->size(); ++i)
  {
    const Point3f& p = currentScan_->point(i);
    if (!std::isfinite(p.x())) continue;

    float dist = std::sqrt(p.x() * p.x() + p.y() * p.y() + p.z() * p.z());
    if (dist < minDistance_) continue;
    if (dist > maxDistance_) continue;

    float in = 0.1f;
    if (currentScan_->hasRemission())
    {
      float in = currentScan_->remission(i);
      if (in > 0.9) in = 0.9;
      if (in < 0.1) in = 0.1;
    }

    glColor3f(in, in, in);

    glVertex3f(p.x(), p.y(), p.z());
  }

  glEnd();

  glPopMatrix();
}

void CanvasGL::setSelectedSegment(int idx)
{
  selectedSegment_ = idx;
  updateGL();
}

void CanvasGL::drawGroundTruth()
{
  glPushMatrix();
  glMultMatrixf(RoSe2GL);

  for (uint32_t i = 0; i < groundTruth_.size(); ++i)
  {
    glColor3f(0.0f, 0.0f, 0.0f);
    if (groundTruthLabels_.size() > i)
    {
      if (groundTruthLabels_[i] == "Pedestrian") glColor3fv(ColorGL::BLUE);
      if (groundTruthLabels_[i] == "Car") glColor3fv(ColorGL::RED);
      if (groundTruthLabels_[i] == "Cyclist") glColor3fv(ColorGL::GREEN);
    }
    drawBBox(groundTruth_[i]);
  }

  glPopMatrix();
}

void CanvasGL::drawSegments()
{
  if (currentScan_ == 0) return;

  glPushMatrix();
  glMultMatrixf(RoSe2GL);
  glPointSize(pointSize_ + 1);

  glBegin(GL_POINTS);
  // draw segments.
  for (uint32_t i = 0; i < segments_.size(); ++i)
  {
    glColor3f(0.0f, 0.0f, 0.0f);
    if (labels_.size() > i)
    {
      if (labels_[i] == "Pedestrian") glColor3fv(ColorGL::BLUE);
      if (labels_[i] == "Car") glColor3fv(ColorGL::RED);
      if (labels_[i] == "Cyclist") glColor3fv(ColorGL::GREEN);
    }

    for (uint32_t j = 0; j < segments_[i].size(); ++j)
    {
      const Point3f& p = currentScan_->point(segments_[i].indexes[j]);
      glVertex3f(p.x(), p.y(), p.z());
    }

  }
  glEnd();

  if (selectedSegment_ >= 0 && selectedSegment_ < segments_.size())
  {
    glColor3fv(ColorGL::GOLD);
    glLineWidth(2.0f);
    drawBBox(bboxes_[selectedSegment_]);
    glLineWidth(1.0f);
  }

  glPopMatrix();
}

void CanvasGL::setLabels(const std::vector<std::string>& labels)
{
  labels_ = labels;
}

void CanvasGL::setGroundTruth(const std::vector<BBox>& bboxes, const std::vector<std::string>& labels)
{
  groundTruth_ = bboxes;
  groundTruthLabels_ = labels;
}

void CanvasGL::setSegments(const std::vector<IndexedSegment>& segments, const std::vector<BBox>& bboxes)
{
  segments_ = segments;
  selectedSegment_ = -1;
  bboxes_ = bboxes;
}

void CanvasGL::drawBBox(const BBox& box)
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glPushMatrix();
  /** first transform the bounding box. **/
  glMultMatrixf(box.t);

  /** draw the bbox. **/
  float sx = box.extent.x();
  float sy = box.extent.y();
  float sz = box.extent.z();

  glBegin(GL_QUAD_STRIP);
  glVertex3f(-sx, -sy, -sz);    //1
  glVertex3f(+sx, -sy, -sz);    //2
  glVertex3f(-sx, -sy, +sz);    //3
  glVertex3f(+sx, -sy, +sz);    //4
  glVertex3f(-sx, +sy, +sz);    //5
  glVertex3f(+sx, +sy, +sz);    //6
  glVertex3f(-sx, +sy, -sz);    //7
  glVertex3f(+sx, +sy, -sz);    //8
  glVertex3f(-sx, -sy, -sz);    //9
  glVertex3f(+sx, -sy, -sz);    //10
  glEnd();

  glBegin(GL_QUADS);
  glVertex3f(+sx, -sy, -sz);    //2
  glVertex3f(+sx, +sy, -sz);    //8
  glVertex3f(+sx, +sy, +sz);    //6
  glVertex3f(+sx, -sy, +sz);    //4
  glEnd();

  glBegin(GL_QUADS);
  glVertex3f(-sx, -sy, -sz);    //1
  glVertex3f(-sx, +sy, -sz);    //7
  glVertex3f(-sx, +sy, +sz);    //5
  glVertex3f(-sx, -sy, +sz);    //3
  glEnd();

  glPopMatrix();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
