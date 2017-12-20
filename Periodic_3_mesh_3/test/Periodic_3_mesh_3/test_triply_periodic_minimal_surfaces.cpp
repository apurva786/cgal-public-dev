#include <CGAL/Periodic_3_mesh_3/config.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Periodic_3_mesh_3/IO/File_medit.h>
#include <CGAL/Labeled_periodic_3_mesh_domain_3.h>
#include <CGAL/make_periodic_3_mesh_3.h>
#include <CGAL/Periodic_3_mesh_triangulation_3.h>

#include <CGAL/functional.h>
#include <CGAL/Implicit_to_labeling_function_wrapper.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// Kernel
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::FT                                               FT;
typedef K::Point_3                                          Point;
typedef K::Segment_3                                        Segment;

// Function
typedef FT (*Function)(const Point&);
typedef CGAL::Implicit_multi_domain_to_labeling_function_wrapper<Function> Labeling_function;

// Domain
typedef CGAL::Labeled_periodic_3_mesh_domain_3<Labeling_function, K> Periodic_mesh_domain;

// Triangulation
typedef CGAL::Periodic_3_mesh_triangulation_3<Periodic_mesh_domain>::type  Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr>                        C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Periodic_mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;

const FT PI = CGAL_PI;

FT schwarz_p(const Point& p) {
  const FT x2 = std::cos( p.x() * 2*PI ),
           y2 = std::cos( p.y() * 2*PI ),
           z2 = std::cos( p.z() * 2*PI );
  return x2 + y2 + z2;
}

FT schwarz_p_transl (const Point& p) {
  const FT x2 = std::cos(p.x() * 2 * PI + PI / 2.0),
           y2 = std::cos(p.y() * 2 * PI + PI / 2.0),
           z2 = std::cos(p.z() * 2 * PI + PI / 2.0);
  return x2 + y2 + z2;
}

FT gyroid (const Point& p) {
  const FT cx = std::cos(p.x() * 2 * PI),
           cy = std::cos(p.y() * 2 * PI),
           cz = std::cos(p.z() * 2 * PI);
  const FT sx = std::sin(p.x() * 2 * PI),
           sy = std::sin(p.y() * 2 * PI),
           sz = std::sin(p.z() * 2 * PI);
  return cx * sy + cy * sz + cz * sx;
}

FT diamond (const Point& p) {
  const FT cx = std::cos(p.x() * 2 * PI),
           cy = std::cos(p.y() * 2 * PI),
           cz = std::cos(p.z() * 2 * PI);
  const FT sx = std::sin(p.x() * 2 * PI),
           sy = std::sin(p.y() * 2 * PI),
           sz = std::sin(p.z() * 2 * PI);
  return sx * sy * sz + sx * cy * cz + cx * sy * cz + cx * cy * sz;
}

FT double_p (const Point& p) {
  const FT cx = std::cos(p.x() * 2 * PI),
           cy = std::cos(p.y() * 2 * PI),
           cz = std::cos(p.z() * 2 * PI);
  const FT c2x = std::cos(2 * p.x() * 2 * PI),
           c2y = std::cos(2 * p.y() * 2 * PI),
           c2z = std::cos(2 * p.z() * 2 * PI);
  return 0.5 * (cx * cy  + cy * cz + cz * cx ) + 0.2*(c2x + c2y + c2z);
}

FT G_prime (const Point& p) {
  const FT cx = std::cos(p.x() * 2 * PI),
           cy = std::cos(p.y() * 2 * PI),
           cz = std::cos(p.z() * 2 * PI);
  const FT c2x = std::cos(2 * p.x() * 2 * PI),
           c2y = std::cos(2 * p.y() * 2 * PI),
           c2z = std::cos(2 * p.z() * 2 * PI);
  const FT sx = std::sin(p.x() * 2 * PI),
           sy = std::sin(p.y() * 2 * PI),
           sz = std::sin(p.z() * 2 * PI);
  const FT s2x = std::sin(2 * p.x() * 2 * PI),
           s2y = std::sin(2 * p.y() * 2 * PI),
           s2z = std::sin(2 * p.z() * 2 * PI);
  return 5 * (s2x * sz * cy  + s2y * sx * cz  + s2z * sy * cx)
         + 1*(c2x * c2y  + c2y * c2z  + c2z * c2x);
}

FT lidinoid (const Point& p) {
  const FT cx = std::cos(p.x() * 2 * PI),
           cy = std::cos(p.y() * 2 * PI),
           cz = std::cos(p.z() * 2 * PI);
  const FT c2x = std::cos(2 * p.x() * 2 * PI),
           c2y = std::cos(2 * p.y() * 2 * PI),
           c2z = std::cos(2 * p.z() * 2 * PI);
  const FT sx = std::sin(p.x() * 2 * PI),
           sy = std::sin(p.y() * 2 * PI),
           sz = std::sin(p.z() * 2 * PI);
  const FT s2x = std::sin(2 * p.x() * 2 * PI),
           s2y = std::sin(2 * p.y() * 2 * PI),
           s2z = std::sin(2 * p.z() * 2 * PI);
  return 1 * (s2x * sz * cy  + s2y * sx * cz  + s2z * sy * cx)
         - 1 * (c2x * c2y  + c2y * c2z  + c2z * c2x) + 0.3;
}

FT D_prime (const Point& p) {
  const FT cx = std::cos(p.x() * 2 * PI),
           cy = std::cos(p.y() * 2 * PI),
           cz = std::cos(p.z() * 2 * PI);
  const FT c2x = std::cos(2 * p.x() * 2 * PI),
           c2y = std::cos(2 * p.y() * 2 * PI),
           c2z = std::cos(2 * p.z() * 2 * PI);
  const FT sx = std::sin(p.x() * 2 * PI),
           sy = std::sin(p.y() * 2 * PI),
           sz = std::sin(p.z() * 2 * PI);
  return 1 * ( sx * sy * sz) + 1 * ( cx * cy * cz)
         - 1 * ( c2x * c2y  + c2y * c2z  + c2z * c2x) - 0.4;
}

FT split_p (const Point& p) {
  const FT cx = std::cos(p.x() * 2 * PI),
           cy = std::cos(p.y() * 2 * PI),
           cz = std::cos(p.z() * 2 * PI);
  const FT c2x = std::cos(2 * p.x() * 2 * PI),
           c2y = std::cos(2 * p.y() * 2 * PI),
           c2z = std::cos(2 * p.z() * 2 * PI);
  const FT sx = std::sin(p.x() * 2 * PI),
           sy = std::sin(p.y() * 2 * PI),
           sz = std::sin(p.z() * 2 * PI);
  const FT s2x = std::sin(2 * p.x() * 2 * PI),
           s2y = std::sin(2 * p.y() * 2 * PI),
           s2z = std::sin(2 * p.z() * 2 * PI);
  return  1.1 * (s2x * sz * cy  + s2y * sx * cz  + s2z * sy * cx)
          - 0.2 * (c2x * c2y  + c2y * c2z  + c2z * c2x)
          - 0.4 * (cx + cy + cz);
}

struct Segments_function
  : public CGAL::unary_function<Point, FT>
{
  typedef std::vector<Segment> Segments;

  Segments_function() : segments()
  {
    const Point pmid(0.5,0.5,0.5);
    nb_evals = 0;
    segments.push_back( Segment(Point(0,0.5,0), pmid) );
    segments.push_back( Segment(Point(1,0.5,0), pmid) );
    segments.push_back( Segment(Point(0,0.5,1), pmid) );
    segments.push_back( Segment(Point(1,0.5,1), pmid) );
    segments.push_back( Segment(Point(0.5,0,0), pmid) );
    segments.push_back( Segment(Point(0.5,1,0), pmid) );
    segments.push_back( Segment(Point(0.5,0,1), pmid) );
    segments.push_back( Segment(Point(0.5,1,1), pmid) );
  }

  FT operator()(const Point& p)
  {
    ++nb_evals;

    FT min_distance = 1000000;
    for (Segments::const_iterator si = segments.begin(); si != segments.end(); ++si)
      min_distance = std::min(CGAL::squared_distance(p, *si), min_distance);

    return min_distance - 0.01; // Change the squared beam radius here
  }

  Segments segments;
  int nb_evals;
};

FT segments(const Point& p) {
  static Segments_function instance;
  return instance(p);
}

std::vector<std::string> make_vps_in ()
{
  return std::vector<std::string>(1, "-");
}

std::vector<std::string> make_vps_in_out ()
{
  std::vector<std::string> vps;
  vps.push_back("-");
  vps.push_back("+");
  return vps;
}

C3t3 make_mesh(const Labeling_function& labeling_function)
{
  Periodic_mesh_domain domain(labeling_function, CGAL::Iso_cuboid_3<K>(1, 1, 1, 3, 3, 3));

  Periodic_mesh_criteria criteria(facet_angle = 30.,
                                  facet_size = 0.03 * 2 /*domain's edge length*/,
                                  facet_distance = 0.03 * 2 /*domain's edge length*/,
                                  cell_radius_edge_ratio = 2.,
                                  cell_size = 0.05);

  return CGAL::make_periodic_3_mesh_3<C3t3>(domain, criteria);
}

typedef std::vector<std::string> Position_vector;

int main(int, char**)
{
  std::map<std::string, Function> functions;
  functions["D_prime"] = &D_prime;
  functions["G_prime"] = &G_prime;
  functions["diamond"] = &diamond;
  functions["double_p"] = &double_p;
  functions["gyroid"] = &gyroid;
  functions["lidinoid"] = &lidinoid;
  functions["schwarz_p"] = &schwarz_p;
  functions["schwarz_p_transl"] = &schwarz_p_transl;
  functions["segments"] = &segments;
  functions["split_p"] = &split_p;

  std::map<std::string, Position_vector> v_vps;
  v_vps["in"] = make_vps_in();
  v_vps["in-out"] = make_vps_in_out();

  std::vector<unsigned> v_ncopy;
//  v_ncopy.push_back(1);
  v_ncopy.push_back(2);
//  v_ncopy.push_back(4);
//  v_ncopy.push_back(8);

  for (std::map<std::string, Function>::iterator iter = functions.begin();
       iter != functions.end(); ++iter)
  {
    for (std::map<std::string, Position_vector>::iterator it = v_vps.begin();
         it != v_vps.end(); ++it)
    {
      std::ostringstream oss;
      oss << iter->first << "__" << it->first;
      std::string mesh_id = oss.str();

      std::cout << "Meshing " << mesh_id << "..." << std::flush;

      std::vector<Function> funcs;
      funcs.push_back(iter->second);

      Labeling_function labeling_function(funcs, it->second);
      C3t3 c3t3 = make_mesh(labeling_function);

      CGAL_postcondition(c3t3.is_valid());
      CGAL_postcondition(c3t3.triangulation().is_valid());

      std::cout << " Done" << std::flush;

      for (std::vector<unsigned>::iterator i = v_ncopy.begin(); i != v_ncopy.end(); ++i)
      {
        std::ostringstream oss_2;
        oss_2 << iter->first << "__" << it->first << "__" << *i  << ".mesh";
        std::string output_filename = oss_2.str();
        std::ofstream medit_file( output_filename.data() );
        CGAL::output_to_medit(medit_file, c3t3, *i);
        medit_file.close();

        std::cout << ", " << *i << "-copy Saved" << std::flush;
      }
      std::cout << std::endl;
    }
  }

  std::cout << "EXIT SUCCESS" << std::endl;
  return 0;
}
