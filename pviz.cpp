#include <QApplication>
#include <stdint.h>
#include "widget/PointCloudVisualizer.h"

int32_t main(int32_t argc, char** argv)
{
  QApplication app(argc, argv);

  PointCloudVisualizer frame;
  frame.show();

  return app.exec();
}
