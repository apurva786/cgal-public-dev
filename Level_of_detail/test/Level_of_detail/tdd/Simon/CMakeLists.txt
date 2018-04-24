# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

project( Simon )


cmake_minimum_required(VERSION 2.8.11)

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()

# include helper file
include( ${CGAL_USE_FILE} )


# Boost and its components
find_package( Boost REQUIRED COMPONENTS filesystem)

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()

# 3rd party libraries
find_package(GDAL REQUIRED)
if (NOT GDAL_FOUND)
  message(STATUS "This project requires the GDAL library, and will not be compiled.")
  return()
endif()
  
find_package(OpenCV 3.2 REQUIRED)
if (NOT OpenCV_FOUND)
  message(STATUS "This project requires the OpenCV 3.2 library, and will not be compiled.")
  return()
endif()

find_package(Eigen3 3.1.0 REQUIRED)
if (NOT EIGEN3_FOUND)
  message(STATUS "This project requires the Eigen library, and will not be compiled.")
  return()
endif()

include_directories(${OpenCV_INCLUDE_DIRS} ${GDAL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
  
# 3rd parties to clean
include_directories("/usr/local/include/ooqp")
set(ma27_LIBRARY ma27)
set(OOQP_LIBS ooqpbase ooqpdense ooqpgensparse ooqpgondzio ooqpsparse)

# include for local package


# Creating entries for all C++ files with "main" routine
# ##########################################################

include( CGAL_CreateSingleSourceCGALProgram )

set(CMAKE_CXX_STANDARD 11)

set(sources lod.cpp
	../../../../include/CGAL/Regularizer/jean_philippe/indexed_event.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/kinetic_model.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/line_item.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/parameters.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/partition_elements.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/partition.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/quadtree_point.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/segment_ray.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/segment_tree.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/geometry.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/means.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/regularization_angles.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/regularization_ordinates.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/quadratic_problem.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/regularization_angles_quadratic.cpp 
	../../../../include/CGAL/Regularizer/jean_philippe/regularization_ordinates_quadratic.cpp
	../../../../include/CGAL/Regularizer/jean_philippe/propagation.cpp)

add_executable(lod ${sources})

target_link_libraries(lod ${OpenCV_LIBS} ${GDAL_LIBRARY} ${Boost_LIBRARIES} ${ma27_LIBRARY} ${OOQP_LIBS})



