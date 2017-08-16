#ifndef CGAL_LEVEL_OF_DETAIL_TRAITS_STUB_H
#define CGAL_LEVEL_OF_DETAIL_TRAITS_STUB_H

#include <CGAL/Loader/Level_of_detail_loader_stub.h>

namespace CGAL {

	namespace LOD {

		template<class KernelTraits, class Container>
		struct Level_of_detail_traits_stub {

			typedef KernelTraits Kernel;
			typedef Level_of_detail_loader_stub<Kernel, Container> Loader;

		};
	}
}

#endif // CGAL_LEVEL_OF_DETAIL_TRAITS_STUB_H