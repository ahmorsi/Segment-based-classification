# build robovision library with core functionality
add_library(robovision
  BBox.cpp
	Classifier.cpp
	ColorGL.cpp
	CompositeParameter.cpp
	Laserscan.cpp
	Math.cpp
	NearestNeighborImpl.cpp
	Normalizer.cpp
	Objective.cpp
	Optimization.cpp
	Parameter.cpp
	ParameterList.cpp
	PointDescriptor.cpp
	PrimitiveParameters.cpp
	Random.cpp
	ScaledCG.cpp
	SegmentDescriptor.cpp
	StringTokenizer.cpp
	Stopwatch.cpp
	Transform.cpp
	tinyxml2.cpp
	opengl_utils.cpp
	string_utils.cpp)
	
target_link_libraries(robovision ${OPENGL_LIBRARIES} ${Boost_LIBRARIES})
