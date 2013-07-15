//This program is to read input file of Arrangement and transfer
//it into Linear_cell_complex. An output file of Linear_cell_complex
//will be created
#include "convertors.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
namespace po = boost::program_options;
//using namespace std;

typedef CGAL::Exact_predicates_exact_constructions_kernel   Kernel;
typedef Kernel::FT                                          Number_type;
typedef CGAL::Arr_linear_traits_2<Kernel>                   Traits_2;
typedef Traits_2::Point_2                                   Point_2;
typedef Traits_2::Segment_2                                 Segment;
typedef Traits_2::Ray_2                                     Ray;
typedef Traits_2::Line_2                                    Line;
typedef Traits_2::X_monotone_curve_2                        X_monotone_curve;
typedef CGAL::Arrangement_2<Traits_2>                       Arrangement;

typedef CGAL::Linear_cell_complex_traits<2, Kernel>         Traits;
typedef CGAL::Linear_cell_complex<2, 2, Traits>             LCC;
typedef LCC::Dart_handle                                    Dart_handle;
typedef LCC::Point                                          Point;
typedef LCC::FT                                             FT;

int main(int argc, char* argv[])
{
  //declare the objects
  Arrangement arr;
  LCC lcc;
  Dart_handle dart;
    
  //Boost-options to override the input or output files
  boost::program_options::options_description desc;
  desc.add_options()
    ("help", "produce help")
    ("input,i", po::value< std::string>(), "input file")
    ("output,o", po::value<std::string>(),"output file")
  ;
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  //Check whether there exists input or output file
  std::string filename;
  if (vm.count("input")) {
    filename = vm["input"].as<std::string>();
  } else {
    filename = "arr.dat";
  }
  //Read the input file
  std::ifstream fin(filename.c_str());
  //Check whether the inpur file is valid
  if (fin.is_open()) {
    while (!fin.eof()) {
      Segment s;
      Number_type a, b, c, d;
      // Read the coordinates
      fin >> a >> b >> c >> d;
      //Create the segment
      s = Segment(Point_2(a, b), Point_2(c, d));
      // add this curve into the Arrangement
      insert(arr, s);
    }
  }
  else {
    std::cout << "Invalid input file" << std::endl;
    return EXIT_SUCCESS;
  }
  fin.close();
  //Call the function that transfer the Arrangement to Linear_cell_complex
  dart = CGAL::arr2lcc<Arrangement, LCC>(arr, lcc);
  //Get the number of vertices and edges
  int num_ver = arr.number_of_vertices();
  int num_edge = arr.number_of_edges();
    std::cout<<"here9" << std::endl;

    std::cout<<"LCC characteristics: " << std::endl;
    lcc.display_characteristics(std::cout) <<std::endl;
    std::cout<<"valid=" << lcc.is_valid() << std::endl;
    std::cout<<"here 10" << std::endl;
    /*
  //Write file
  std::ofstream myfile;
  //Check whether the output file name is typed
  if (vm.count("output")) {
    myfile.open((vm["output"].as<std::string>()).c_str());
  }
  else myfile.open("lcc.dat");
  //Write the number of vertices and edges
  myfile << num_ver << "  " << num_edge << std::endl;
  std::vector<Point> vert;
  //Write the vertices
  for (LCC::Vertex_attribute_range::iterator it =
       lcc.vertex_attributes().begin(), itend=lcc.vertex_attributes().end();
       it!=itend; ++it) {
    Point temp = it->point();
    bool mark = false;
    int pos = 0;
    for (pos = 0; pos < vert.size(); pos++) {
      if (vert[pos] == temp) mark = true;
    }
    if (!mark) {
      vert.push_back(temp);
      myfile << it->point();
      if (vert.size() == num_ver) {
        myfile << std::endl;
        break;
      }
      else myfile << "  ";
    }
    //else std::cout<<"mark"<<std::endl;
  }
       
  //Write the indexes of vertices according to the associate edges
  int count = 0;
  for (LCC::Vertex_attribute_range::iterator it =
       lcc.vertex_attributes().begin(), itend=lcc.vertex_attributes().end();
       it!=itend; ++it) {
    Point temp = it->point();
      //std::cout<<temp<<std::endl;
      
    ++count;
    for (int i = 0; i < vert.size(); i++) {
      if (vert[i] == temp) {
        myfile << i;
        if ((count % 2 == 0) && (count != 2*num_edge)) myfile << "  ";
        else if (count == 2*num_edge) myfile << "";
        else myfile << ' ';
        break;
      }
    }
  }
  myfile.close();
     */
  return 0;
}
