#include <iostream>
#include <fstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/vsa_mesh_segmentation.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef typename boost::graph_traits<Polyhedron>::face_descriptor face_descriptor;
typedef boost::unordered_map<face_descriptor, std::size_t> Facet_index_map;
typedef boost::associative_property_map<Facet_index_map> Facet_proxy_pmap;

int main()
{
  // create polyhedral surface and read input surface triangle mesh 
  Polyhedron input;
  std::ifstream file("data/mask.off");
  if (!file || !(file >> input) || input.empty()) {
    std::cerr << "Invalid off file." << std::endl;
    return EXIT_FAILURE;
  }

  Facet_index_map fidx_map;
  BOOST_FOREACH(face_descriptor f, faces(input))
    fidx_map[f] = 0;
  // facet proxy index property map
  Facet_proxy_pmap fpxmap(fidx_map);

  // free function interface with named parameters
  CGAL::VSA::mesh_segmentation(input,
    fpxmap, // output indexed face set
    CGAL::VSA::parameters::init_method(CGAL::VSA::Hierarchical). // hierarchical init
    max_nb_proxies(200). // refine until target number of proxies
    iterations(30)); // number of relaxation iterations after seeding

  return EXIT_SUCCESS;
}
