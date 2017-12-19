#ifndef CGAL_LEVEL_OF_DETAIL_COMPLEXITY_H
#define CGAL_LEVEL_OF_DETAIL_COMPLEXITY_H

#if defined(WIN32) || defined(_WIN32) 
#define PS "\\"
#else 
#define PS "/"
#endif

// STL includes.
#include <string>
#include <vector>
#include <cassert>
#include <map>

// New CGAL includes.
#include <CGAL/Mylog/Mylog.h>
#include <CGAL/Selector/Level_of_detail_selector.h>
#include <CGAL/Selector/Level_of_detail_selection_strategy.h>

namespace CGAL {

	namespace LOD {

		template<class KernelTraits, class InputContainer, class LodReconstruction>
		class Level_of_detail_complexity {

		public:
			typedef KernelTraits   	  Kernel;
			typedef InputContainer    Container;
			typedef LodReconstruction LODS;

			typedef typename Kernel::FT FT;

			typedef typename LodReconstruction::Mesh Mesh;
			using Facet_handle = typename Mesh::Facet_const_handle;

			using Index   = int;
			using Indices = std::vector<Index>;
			using Log 	  = CGAL::LOD::Mylog;
			using Faces   = std::vector< std::vector<Facet_handle> >;

			typedef CGAL::LOD::Level_of_detail_building_interior<Kernel, Container> Roofs_points_selection_strategy;
			typedef CGAL::LOD::Level_of_detail_building_boundary<Kernel, Container> Walls_points_selection_strategy;
			
			typedef CGAL::LOD::Level_of_detail_selector<Kernel, Roofs_points_selection_strategy> Roofs_points_selector;
			typedef CGAL::LOD::Level_of_detail_selector<Kernel, Walls_points_selection_strategy> Walls_points_selector;

			Level_of_detail_complexity(const Container &input, const LODS &lods) 
			: m_input(input), m_lods(lods), m_complexity(-FT(1)), m_debug(false) { }

			void estimate() {

				const FT num_roofs = get_number_of_roofs();
				const FT num_walls = get_number_of_walls();

				get_roofs_normalization_factor();
				get_walls_normalization_factor();

				m_complexity = num_roofs + num_walls;
			}

			FT get() const {

				assert(m_complexity >= FT(0));
				return m_complexity;
			}

		private:
			const Container &m_input;
			const LODS 		&m_lods;
			
			FT 	 m_complexity;
			bool m_debug;

			FT get_number_of_roofs() const {

				const int num_roofs = m_lods.get_number_of_roofs();
				return static_cast<FT>(num_roofs);
			}

			FT get_number_of_walls() const {

				Faces walls_faces;
				m_lods.get_walls_faces(walls_faces);

				size_t num_walls = 0;
				for (size_t i = 0; i < walls_faces.size(); ++i) num_walls += walls_faces[i].size();

				return static_cast<FT>(num_walls);
			}

			FT get_roofs_normalization_factor() const {
				
				Indices roofs_point_indices;
				Roofs_points_selector selector;
				selector.select_elements(m_input, std::back_inserter(roofs_point_indices));
	
				if (m_debug) {
					Log roofs_points_saver;
					roofs_points_saver.export_points_using_indices(m_input, roofs_point_indices, "tmp" + std::string(PS) + "roofs_points_for_complexity");
				}

				const int roofs_num_points = roofs_point_indices.size();
				if (m_debug) std::cout << roofs_num_points << std::endl;

				return static_cast<FT>(roofs_num_points);
			}

			FT get_walls_normalization_factor() const {			
				
				Indices walls_point_indices;
				Walls_points_selector selector;
				selector.select_elements(m_input, std::back_inserter(walls_point_indices));
	
				if (m_debug) {
					Log walls_points_saver;
					walls_points_saver.export_points_using_indices(m_input, walls_point_indices, "tmp" + std::string(PS) + "walls_points_for_complexity");
				}

				const int walls_num_points = walls_point_indices.size();
				if (m_debug) std::cout << walls_num_points << std::endl;

				return static_cast<FT>(walls_num_points);
			}
		};
	}
}

#endif // CGAL_LEVEL_OF_DETAIL_COMPLEXITY_H