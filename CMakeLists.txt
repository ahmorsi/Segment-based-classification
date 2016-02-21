cmake_minimum_required(VERSION 2.6)
project(SegmentClassification)

find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED system filesystem)
find_package(Qt4 REQUIRED QtGui QtXml QtOpenGL)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})     # executable is inside the source directory
set(CMAKE_BUILD_TYPE Debug) ## Uncomment this for debug mode for 'gdb'...

include_directories(${QT_INCLUDES} ${Boost_INCLUDE_DIRS} lib/robovision lib/)

add_subdirectory(lib/robovision)
add_subdirectory(lib/gtest)

QT4_WRAP_UI(pviz_UI_HDRS widget/pviz.ui)

# build visualization widget
add_executable(pviz
  widget/CameraGL.cpp
  widget/PointCloudVisualizer.cpp
  widget/CanvasGL.cpp
  ${pviz_UI_HDRS}
  pviz.cpp)
  
target_link_libraries(pviz ${OPENGL_LIBRARIES} ${Boost_LIBRARIES} Qt4::QtGui Qt4::QtOpenGL robovision)

# build different stages of classification.
add_executable(classify-scans
  project/utils.cpp
  project/Octree.cpp
  project/SpinImage.cpp
  project/BagOfWordsDescriptor.cpp
  project/GridbasedSegmentation.cpp
  project/L2SoftmaxObjective.cpp
  project/SoftmaxRegression.cpp
  classify-scans.cpp)
	
add_executable(train-dictionary
  project/utils.cpp
  project/Octree.cpp
  project/SpinImage.cpp
  project/BagOfWordsDescriptor.cpp
  project/GridbasedSegmentation.cpp
  project/L2SoftmaxObjective.cpp
  project/SoftmaxRegression.cpp
  project/KMeans.cpp
  train-dictionary.cpp)
	
add_executable(train-classifier
  project/utils.cpp
  project/Octree.cpp
  project/SpinImage.cpp
  project/BagOfWordsDescriptor.cpp
  project/GridbasedSegmentation.cpp
  project/L2SoftmaxObjective.cpp
  project/SoftmaxRegression.cpp
  train-classifier.cpp)
  
add_executable(score
  project/utils.cpp
  score-detections.cpp)
  
add_executable(runtests
  tests/test_utils.cpp
  # tested classes
  project/Octree.cpp
  project/KMeans.cpp
  project/utils.cpp
  project/L2SoftmaxObjective.cpp
  project/BagOfWordsDescriptor.cpp
  project/SpinImage.cpp
  project/GridbasedSegmentation.cpp
  tests/octree-test.cpp
  tests/segmentation-test.cpp
  tests/spinimage-test.cpp
  tests/bow-test.cpp
  tests/kmeans-test.cpp
  tests/softmax-test.cpp
  )
  
	
target_link_libraries(classify-scans ${Boost_LIBRARIES} robovision)
target_link_libraries(train-classifier ${Boost_LIBRARIES} robovision)
target_link_libraries(train-dictionary ${Boost_LIBRARIES} robovision)
target_link_libraries(score ${Boost_LIBRARIES} robovision)
target_link_libraries(runtests ${Boost_LIBRARIES} robovision gtest_main)