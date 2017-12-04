#include <CGAL/Periodic_3_mesh_3/config.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/IO/Medit_IO.h>
#include <CGAL/Labeled_periodic_3_mesh_domain_3.h>
#include <CGAL/make_periodic_3_mesh_3.h>
#include <CGAL/Periodic_3_mesh_triangulation_3.h>

#include <CGAL/Implicit_to_labeling_function_wrapper.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/number_type_config.h> // CGAL_PI

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Kernel
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::FT                                               FT;
typedef K::Point_3                                          Point;

// Function
typedef FT (*Function)(const Point&);
typedef CGAL::Implicit_multi_domain_to_labeling_function_wrapper<Function> Wrapper;

// Domain
typedef CGAL::Labeled_periodic_3_mesh_domain_3<Wrapper, K>  Periodic_mesh_domain;

// Triangulation
typedef CGAL::Periodic_3_mesh_triangulation_3<Periodic_mesh_domain>::type  Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr>                        C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr>                         Periodic_mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;

// Implicit functions
FT sphere_function_1 (const Point& p)
{ return CGAL::squared_distance(p, Point(0.5, 0.5, 0.5)) - 0.15; }

FT sphere_function_2 (const Point& p)
{ return CGAL::squared_distance(p, Point(0.66, 0.5, 0.5)) - 0.2; }

FT schwarz_p(const Point& p)
{
  const FT x2 = std::cos( p.x() * 2 * CGAL_PI ),
           y2 = std::cos( p.y() * 2 * CGAL_PI ),
           z2 = std::cos( p.z() * 2 * CGAL_PI );
  return x2 + y2 + z2;
}

int main(int argc, char** argv)
{
  int domain_size = (argc > 1) ? atof(argv[1]) : 1;

  std::vector<Function> funcs;
  funcs.push_back(&schwarz_p);
  funcs.push_back(&sphere_function_1);
  std::vector<std::string> vps;
  vps.push_back("--");
  vps.push_back("-+");
  Wrapper wrapper(funcs, vps);
  Periodic_mesh_domain domain(wrapper, CGAL::Iso_cuboid_3<K>(0, 0, 0, domain_size, domain_size, domain_size));

  Periodic_mesh_criteria criteria(facet_angle = 30,
                                  facet_size = 0.04,
                                  facet_distance = 0.025,
                                  cell_radius_edge_ratio = 2.,
                                  cell_size = 0.04);

  // Mesh generation
  C3t3 c3t3 = CGAL::make_periodic_3_mesh_3<C3t3>(domain, criteria);

  // Output
  std::ofstream medit_file("output_multi_domain.mesh");
  CGAL::output_to_medit(medit_file, c3t3, 1);

  std::cout << "EXIT SUCCESS" << std::endl;
  return 0;
}
