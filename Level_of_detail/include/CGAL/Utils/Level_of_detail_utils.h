#ifndef CGAL_LEVEL_OF_DETAIL_UTILS_H
#define CGAL_LEVEL_OF_DETAIL_UTILS_H

// STL includes.
#include <map>
#include <cassert>
#include <vector>
#include <memory>
#include <string>

// CGAL includes.
#include <CGAL/Triangulation_conformer_2.h>
#include <CGAL/linear_least_squares_fitting_2.h>
#include <CGAL/linear_least_squares_fitting_3.h>

// New CGAL includes.
#include <CGAL/Mylog/Mylog.h>
#include <CGAL/Level_of_detail_enum.h>

// Boost includes.
#include <boost/tuple/tuple.hpp>

namespace CGAL {

	namespace LOD {

		template<class KernelTraits, class InputContainer, class InputCDT>
		class Level_of_detail_utils {

		public:
			typedef KernelTraits   Kernel;
			typedef InputContainer Input;
			typedef InputCDT 	   CDT;

			typedef typename Kernel::FT 	   FT;
			typedef typename Kernel::Point_2   Point_2;
			typedef typename Kernel::Point_3   Point_3;
			typedef typename Kernel::Plane_3   Plane_3;
			typedef typename Kernel::Line_2    Line_2;
			typedef typename Kernel::Segment_2 Segment_2;

			typedef typename CDT::Vertex_handle 		Vertex_handle;
			typedef typename CDT::Face_handle 			Face_handle;
			typedef typename CDT::Finite_edges_iterator Edge_iterator;

			// Extra.
			using Label       = int;
			using Label_map   = typename Input:: template Property_map<Label>;
			using Point_index = typename Input::Index;

			using Log = CGAL::LOD::Mylog;


			///////////////////////////
			// Triangulation functions!

			// BE CAREFUL: THIS THING CAN INSERT NEW POINTS!
			template<class Structured_points, class Structured_labels, class Boundary_data, class Projected_points>
			int compute_cdt(const Structured_points &points, const Structured_labels &labels, CDT &cdt, 
							const bool add_clutter, const Boundary_data &boundary_clutter, const Projected_points &boundary_clutter_projected,
							const bool add_bbox, const Input &input) const {

				Log log;
				auto number_of_faces = -1;

				assert(!points.empty() && !labels.empty());
				assert(points.size() == labels.size());

				cdt.clear();


				// Add all structured segments/points with the corresponding labels.
				std::vector<std::vector<Vertex_handle> > vhs(points.size());

				// Insert points with labels.
				for (size_t i = 0; i < points.size(); ++i) {
					assert(points[i].size() == labels[i].size());

					vhs[i].resize(points[i].size());
					for (size_t j = 0; j < points[i].size(); ++j) {

						vhs[i][j] = cdt.insert(points[i][j]);
						vhs[i][j]->info().label = labels[i][j];
					}
				}

				// Insert constraints.
				for (size_t i = 0; i < points.size(); ++i)
					for (size_t j = 0; j < points[i].size() - 1; ++j)
						cdt.insert_constraint(vhs[i][j], vhs[i][j + 1]);


				// Add clutter.
				if (add_clutter) {
					for (typename Boundary_data::const_iterator it = boundary_clutter.begin(); it != boundary_clutter.end(); ++it) {
						const size_t num_clutter_points = (*it).second.size();

						for (size_t i = 0; i < num_clutter_points; ++i) {
							const auto point_index = (*it).second[i];

							Vertex_handle vh = cdt.insert(boundary_clutter_projected.at(point_index));
							vh->info().label = Structured_label::CLUTTER;
						}
					}
				}


				// Add bounding box.
				if (add_bbox) {
					
					std::vector<Point_2> bbox;
					compute_bounding_box(input, bbox);

					std::vector<Vertex_handle> bhs(bbox.size());

					for (size_t i = 0; i < bbox.size(); ++i) bhs[i] = cdt.insert(bbox[i]);
					for (size_t i = 0; i < bbox.size(); ++i) {

						const size_t ip = (i + 1) % bbox.size();
						cdt.insert_constraint(bhs[i], bhs[ip]);
					}
				}


				// Create CDT.
				CGAL::make_conforming_Delaunay_2(cdt);
				number_of_faces = cdt.number_of_faces();


				// Save CDT.
				log.save_cdt_obj(cdt, "tmp/cdt");
				return number_of_faces;
			}

			template<class Points>
			int compute_delaunay(const Points &points, CDT &cdt) const {

				Log log;

				auto number_of_faces = -1;
				assert(!points.empty());
				
				cdt.clear();

				using Point_iterator = typename Points::const_iterator;


				// Insert all points.
				for (Point_iterator pit = points.begin(); pit != points.end(); ++pit) {
					
					const auto &point = (*pit).second;
					cdt.insert(point);
				}


				// Create CDT.
				CGAL::make_conforming_Delaunay_2(cdt);
				number_of_faces = cdt.number_of_faces();


				// Save CDT.
				log.save_cdt_obj(cdt, "tmp/cdt");

				// log.save_cdt_ply(cdt, "tmp/visibility_before", "in");
				// log.save_cdt_ply(cdt, "tmp/buildings_before" , "bu");

				return number_of_faces;
			}

			void insert_constraint(CDT &cdt, const Edge_iterator &edge_handle) const {

				const Face_handle face = edge_handle->first;
				const int vertex_index = edge_handle->second;

				const Vertex_handle &source = face->vertex(cdt.ccw(vertex_index));
				const Vertex_handle &target = face->vertex( cdt.cw(vertex_index));

				cdt.insert_constraint(source, target);
			}

			bool is_boundary_edge(const CDT &cdt, const Edge_iterator &edge_handle) const {

				const int vertex_index = edge_handle->second;

				const Face_handle face_1 = edge_handle->first;
				const Face_handle face_2 = face_1->neighbor(vertex_index);

				if (cdt.is_infinite(face_1) || cdt.is_infinite(face_2)) return true;
				return false;
			}


			//////////////////////////
			// Bounding box functions!

			void add_bbox_to(const CDT &cdt, const Input &input, CDT &cdt_with_bbox) {

				CDT tmp = cdt;

				for (Edge_iterator eit = tmp.finite_edges_begin(); eit != tmp.finite_edges_end(); ++eit)
					if (is_boundary_edge(tmp, eit)) insert_constraint(tmp, eit);

				cdt_with_bbox = tmp;

				std::vector<Point_2> bbox;
				compute_bounding_box(input, bbox);

				std::vector<Vertex_handle> bhs(bbox.size());

				for (size_t i = 0; i < bbox.size(); ++i) bhs[i] = cdt_with_bbox.insert(bbox[i]);
				for (size_t i = 0; i < bbox.size(); ++i) {

					const size_t ip = (i + 1) % bbox.size();
					cdt_with_bbox.insert_constraint(bhs[i], bhs[ip]);
				}
			}

			template<class Ground, class Ground_point>
			void compute_ground_bbox(const Input &input, Ground &ground_bbox) const {

				ground_bbox.clear();
				ground_bbox.resize(4);

				std::vector<Point_2> bbox;
				compute_bounding_box(input, bbox);

				assert(bbox.size() == 4);
				ground_bbox[0] = Ground_point(bbox[0].x(), bbox[0].y(), FT(0));
				ground_bbox[1] = Ground_point(bbox[1].x(), bbox[1].y(), FT(0));
				ground_bbox[2] = Ground_point(bbox[2].x(), bbox[2].y(), FT(0));
				ground_bbox[3] = Ground_point(bbox[3].x(), bbox[3].y(), FT(0));
			}

			void compute_bounding_box(const Input &input, std::vector<Point_2> &bbox) const {

				bbox.clear();
				bbox.resize(4);

				const FT big_value = FT(1000000); // change it

				FT minx =  big_value, miny =  big_value;
				FT maxx = -big_value, maxy = -big_value;

				for (typename Input::const_iterator it = input.begin(); it != input.end(); ++it) {
					const Point_3 &p = input.point(*it);

					const FT x = p.x();
					const FT y = p.y();

					minx = CGAL::min(minx, x);
					miny = CGAL::min(miny, y);

					maxx = CGAL::max(maxx, x);
					maxy = CGAL::max(maxy, y);
				}

				bbox[0] = Point_2(minx, miny);
				bbox[1] = Point_2(maxx, miny);
				bbox[2] = Point_2(maxx, maxy);
				bbox[3] = Point_2(minx, maxy);
			}


			//////////////////////////////////
			// Ground plane fitting functions!

			// Not efficient since I need to copy all ground points.
			template<class Indices>
			void fit_ground_plane(const Input &input, const Indices &ground_idxs, Plane_3 &ground_plane) const {

				std::vector<Point_3> tmp_ground(ground_idxs.size());
				for (size_t i = 0; i < ground_idxs.size(); ++i) tmp_ground[i] = input.point(ground_idxs[i]);

				CGAL::linear_least_squares_fitting_3(tmp_ground.begin(), tmp_ground.end(), ground_plane, CGAL::Dimension_tag<0>()); 
			}


			/////////////////////
			// Mapping functions!

			template<class Container_2D, class Face_points_map>
			int get_2d_input_and_face_points_map(const CDT &cdt, const Input &input_3d, Container_2D &input_2d, Face_points_map &fp_map) const {

				Log log_all, log_in_cdt;

				typename CDT::Locate_type locate_type;
				int locate_index_stub = -1;

				Label_map class_labels;
				boost::tie(class_labels, boost::tuples::ignore) = input_3d.template property_map<Label>("label");

				input_2d.clear();
				input_2d.resize(input_3d.number_of_points());

				size_t point_index = 0;
				for (typename Input::const_iterator it = input_3d.begin(); it != input_3d.end(); ++it) {

					const Point_index pi = *it;

					const Point_3 &p = input_3d.point(pi);
					const Point_2 &q = Point_2(p.x(), p.y());

					input_2d[point_index] = std::make_pair(q, class_labels[pi]);
					log_all.out << q << " " << 0 << std::endl;

					const Face_handle face_handle = cdt.locate(q, locate_type, locate_index_stub);
					if (locate_type == CDT::FACE || locate_type == CDT::EDGE || locate_type == CDT::VERTEX) {
						
						fp_map[face_handle].push_back(pi);

						log_in_cdt.out << q << " " << 0 << std::endl;
						++point_index;
					}
				}

				log_all.save("tmp/input_2d_all", ".xyz");
				log_in_cdt.save("tmp/input_2d_in_cdt", ".xyz");
				
				return point_index;
			}


			//////////////////////////////////
			// Segment-line related functions!

			// Not efficient since I need to copy all ground points.
			template<class Projected_points, class Planes, class Lines>
			int fit_lines_to_projected_points(const Projected_points &points, const Planes &planes, Lines &lines) const {

				using Plane_iterator = typename Planes::const_iterator;

				auto number_of_fitted_lines = 0;
				std::vector<Point_2> tmp_points;

				lines.clear();
				lines.resize(planes.size());

				for (Plane_iterator it = planes.begin(); it != planes.end(); ++it, ++number_of_fitted_lines) {
					const auto num_points = (*it).second.size();

					tmp_points.clear();
					tmp_points.resize(num_points);

					for (size_t i = 0; i < num_points; ++i) {
						const auto index = (*it).second[i];
						tmp_points[i] = points.at(index);
					}

					CGAL::linear_least_squares_fitting_2(tmp_points.begin(), tmp_points.end(), lines[number_of_fitted_lines], CGAL::Dimension_tag<0>());
				}
				return number_of_fitted_lines;
			}

			// It may have precision problems: e.g. Line = (1 1.0e+24) -- (1 1.0e+24) or nan!
			template<class Projected_points, class Planes, class Lines, class Segments>
			int create_segments_from_lines(const Projected_points &points, const Planes &planes, const Lines &lines, Segments &segments) const {

				using Plane_iterator = typename Planes::const_iterator;

				segments.clear();
				segments.resize(lines.size());

				std::cout.precision(20);

				auto number_of_segments = 0;
				for (Plane_iterator it = planes.begin(); it != planes.end(); ++it, ++number_of_segments) {

					const auto num_points = (*it).second.size();
					const auto big = +FT(100000);

					auto minx = big, miny = big;
					auto maxx = -minx, maxy = -miny;				

					for (size_t i = 0; i < num_points; ++i) {

						const auto index = (*it).second[i];
						const auto point = points.at(index);

						const auto projected = project(lines[number_of_segments], point);

						minx = CGAL::min(minx, projected.x());
						maxx = CGAL::max(maxx, projected.x());

						miny = CGAL::min(miny, projected.y());
						maxy = CGAL::max(maxy, projected.y());
					}
					segments[number_of_segments] = Segment_2(Point_2(minx, miny), Point_2(maxx, maxy));

					auto v1 = lines[number_of_segments].to_vector();
					auto v2 = segments[number_of_segments].to_vector();

					// Rotate segments if needed.
					const auto eps = FT(1) / FT(1000000);
					if ((v1.y() < FT(0) && v2.y() >= FT(0) && std::fabs(v1.y() - v2.y()) > eps) ||
						(v2.y() < FT(0) && v1.y() >= FT(0) && std::fabs(v1.y() - v2.y()) > eps) ||
						(v1.x() < FT(0) && v2.x() >= FT(0) && std::fabs(v1.x() - v2.x()) > eps) ||
						(v2.x() < FT(0) && v1.x() >= FT(0) && std::fabs(v1.x() - v2.x()) > eps)) {

						segments[number_of_segments] = Segment_2(Point_2(minx, maxy), Point_2(maxx, miny));
					}
				}

				assert(static_cast<size_t>(number_of_segments) == lines.size());
				return number_of_segments;
			}

			// My custom function to handle precision problems when projecting points.
			Point_2 project(const Line_2 &line, const Point_2 &p) const {

				const auto a = line.point(0);
				const auto b = line.point(1);

				const auto projected = a + CGAL::scalar_product(p - a, b - a) / CGAL::scalar_product(b - a, b - a) * (b - a);
				
				if (std::isnan(projected.x()) || std::isnan(projected.y())) return line.projection(p);
				else return projected;
			}
		};
	}
}

#endif // CGAL_LEVEL_OF_DETAIL_UTILS_H