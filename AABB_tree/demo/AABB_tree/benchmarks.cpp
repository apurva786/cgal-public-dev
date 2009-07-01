#include "Scene.h"
#include <QInputDialog>

void Scene::benchmark_intersections(const int duration)
{
	QTime time;
	time.start();
	std::cout << "Construct AABB tree...";
	Facet_tree tree(m_pPolyhedron->facets_begin(),m_pPolyhedron->facets_end());
	std::cout << "done (" << time.elapsed() << " ms)" << std::endl;

	bench_do_intersect(tree,duration);
	bench_nb_intersections(tree,duration);
	bench_any_intersection(tree,duration);
	bench_all_intersections(tree,duration);
	bench_all_intersected_primitives(tree,duration);
}

void Scene::benchmark_distances(const int duration)
{
	QTime time;
	time.start();
	std::cout << "Construct AABB tree and internal KD tree...";
	Facet_tree tree(m_pPolyhedron->facets_begin(),m_pPolyhedron->facets_end());
	tree.accelerate_distance_queries();
	std::cout << "done (" << time.elapsed() << " ms)" << std::endl;

	// TODO: check what the KD-tree is doing in there for large models.
	std::cout << "One single call to initialize KD-tree" << std::endl;
	tree.closest_point(CGAL::ORIGIN);

	// benchmark
	bench_closest_point(tree,duration);
	bench_squared_distance(tree,duration);
	bench_closest_point_and_primitive(tree,duration);
}

//////////////////////////////////////////////////////////////
// BENCH INTERSECTIONS
//////////////////////////////////////////////////////////////

void Scene::bench_intersection_rays(Facet_tree& tree,
									const int function,
									const int duration)
{
	QTime time;
	time.start();
	unsigned int nb = 0;
	std::list<Primitive_id> primitive_ids;
	std::list<Object_and_primitive_id> intersections;
	while(time.elapsed() < duration)
	{
		Ray ray = random_ray();
		switch(function)
		{
		case DO_INTERSECT:
			tree.do_intersect(ray);
			break;
		case ANY_INTERSECTION:
			tree.any_intersection(ray);
			break;
		case NB_INTERSECTIONS:
			tree.number_of_intersected_primitives(ray);
			break;
		case ALL_INTERSECTIONS:
			tree.all_intersections(ray,std::back_inserter(intersections));
			break;
		case ALL_INTERSECTED_PRIMITIVES:
			tree.all_intersected_primitives(ray,std::back_inserter(primitive_ids));
			break;
		}
		nb++;
	}
	double speed = 1000.0 * nb / time.elapsed();
	std::cout << speed << " queries/s with ray" << std::endl;
}

void Scene::bench_intersection_lines(Facet_tree& tree,
									const int function,
									const int duration)
{
	QTime time;
	time.start();
	unsigned int nb = 0;
	std::list<Primitive_id> primitive_ids;
	std::list<Object_and_primitive_id> intersections;
	while(time.elapsed() < duration)
	{
		Line line = random_line();
		switch(function)
		{
		case DO_INTERSECT:
			tree.do_intersect(line);
			break;
		case ANY_INTERSECTION:
			tree.any_intersection(line);
			break;
		case NB_INTERSECTIONS:
			tree.number_of_intersected_primitives(line);
			break;
		case ALL_INTERSECTIONS:
			tree.all_intersections(line,std::back_inserter(intersections));
			break;
		case ALL_INTERSECTED_PRIMITIVES:
			tree.all_intersected_primitives(line,std::back_inserter(primitive_ids));
			break;
		}
		nb++;
	}
	double speed = 1000.0 * nb / time.elapsed();
	std::cout << speed << " queries/s with line" << std::endl;
}

void Scene::bench_intersection_segments(Facet_tree& tree,
									const int function,
									const int duration)
{
	QTime time;
	time.start();
	unsigned int nb = 0;
	std::list<Primitive_id> primitive_ids;
	std::list<Object_and_primitive_id> intersections;
	while(time.elapsed() < duration)
	{
		Segment segment = random_segment();
		switch(function)
		{
		case DO_INTERSECT:
			tree.do_intersect(segment);
			break;
		case ANY_INTERSECTION:
			tree.any_intersection(segment);
			break;
		case NB_INTERSECTIONS:
			tree.number_of_intersected_primitives(segment);
			break;
		case ALL_INTERSECTIONS:
			tree.all_intersections(segment,std::back_inserter(intersections));
			break;
		case ALL_INTERSECTED_PRIMITIVES:
			tree.all_intersected_primitives(segment,std::back_inserter(primitive_ids));
			break;
		}
		nb++;
	}
	double speed = 1000.0 * nb / time.elapsed();
	std::cout << speed << " queries/s with segment" << std::endl;
}

void Scene::bench_intersection_planes(Facet_tree& tree,
									const int function,
									const int duration)
{
	QTime time;
	time.start();
	unsigned int nb = 0;
	std::list<Primitive_id> primitive_ids;
	std::list<Object_and_primitive_id> intersections;
	while(time.elapsed() < duration)
	{
		Plane plane = random_plane();
		switch(function)
		{
		case DO_INTERSECT:
			tree.do_intersect(plane);
			break;
		case ANY_INTERSECTION:
			tree.any_intersection(plane);
			break;
		case NB_INTERSECTIONS:
			tree.number_of_intersected_primitives(plane);
			break;
		case ALL_INTERSECTIONS:
			tree.all_intersections(plane,std::back_inserter(intersections));
			break;
		case ALL_INTERSECTED_PRIMITIVES:
			tree.all_intersected_primitives(plane,std::back_inserter(primitive_ids));
			break;
		}
		nb++;
	}
	double speed = 1000.0 * nb / time.elapsed();
	std::cout << speed << " queries/s with plane" << std::endl;
}

void Scene::bench_do_intersect(Facet_tree& tree,
							   const int duration)
{
	std::cout << "Benchmark do_intersect" << std::endl;
	bench_intersection_segments(tree,DO_INTERSECT,duration);
	bench_intersection_rays(tree,DO_INTERSECT,duration);
	bench_intersection_lines(tree,DO_INTERSECT,duration);
	bench_intersection_planes(tree,DO_INTERSECT,duration);
}

void Scene::bench_any_intersection(Facet_tree& tree,
								   const int duration)
{
	std::cout << "Benchmark any_intersection" << std::endl;
	bench_intersection_segments(tree,ANY_INTERSECTION,duration);
	bench_intersection_rays(tree,ANY_INTERSECTION,duration);
	bench_intersection_lines(tree,ANY_INTERSECTION,duration);
	bench_intersection_planes(tree,ANY_INTERSECTION,duration);
}

void Scene::bench_nb_intersections(Facet_tree& tree,
								   const int duration)
{
	std::cout << "Benchmark nb_intersections" << std::endl;
	bench_intersection_segments(tree,DO_INTERSECT,duration);
	bench_intersection_rays(tree,DO_INTERSECT,duration);
	bench_intersection_lines(tree,DO_INTERSECT,duration);
	bench_intersection_planes(tree,DO_INTERSECT,duration);
}

void Scene::bench_all_intersected_primitives(Facet_tree& tree,
											 const int duration)
{
	std::cout << "Benchmark all_intersected_primitives" << std::endl;
	bench_intersection_segments(tree,ALL_INTERSECTED_PRIMITIVES,duration);
	bench_intersection_rays(tree,ALL_INTERSECTED_PRIMITIVES,duration);
	bench_intersection_lines(tree,ALL_INTERSECTED_PRIMITIVES,duration);
	bench_intersection_planes(tree,ALL_INTERSECTED_PRIMITIVES,duration);
}

void Scene::bench_all_intersections(Facet_tree& tree,
									const int duration)
{
	std::cout << "Benchmark all_intersections" << std::endl;
	bench_intersection_segments(tree,ALL_INTERSECTIONS,duration);
	bench_intersection_rays(tree,ALL_INTERSECTIONS,duration);
	bench_intersection_lines(tree,ALL_INTERSECTIONS,duration);
	bench_intersection_planes(tree,ALL_INTERSECTIONS,duration);
}

//////////////////////////////////////////////////////////////
// BENCH DISTANCES
//////////////////////////////////////////////////////////////

void Scene::bench_distance(Facet_tree& tree,
						   const int function,
						   const int duration)
{
	QTime time;
	time.start();
	unsigned int nb = 0;
	while(time.elapsed() < duration)
	{
		Point query = random_point();
		switch(function)
		{
		case SQ_DISTANCE:
			tree.squared_distance(query);
			break;
		case CLOSEST_POINT:
			tree.closest_point(query);
			break;
		case CLOSEST_POINT_AND_PRIMITIVE_ID:
			tree.closest_point_and_primitive(query);
		}
		nb++;
	}
	double speed = 1000.0 * nb / time.elapsed();
	std::cout << speed << " queries/s" << std::endl;
}

void Scene::bench_squared_distance(Facet_tree& tree,
								   const int duration)
{
	std::cout << "Benchmark squared distance" << std::endl;
	bench_distance(tree,SQ_DISTANCE,duration);
}

void Scene::bench_closest_point(Facet_tree& tree,
								const int duration)
{
	std::cout << "Benchmark closest point" << std::endl;
	bench_distance(tree,CLOSEST_POINT,duration);
}

void Scene::bench_closest_point_and_primitive(Facet_tree& tree,
											  const int duration)
{
	std::cout << "Benchmark closest point and primitive" << std::endl;
	bench_distance(tree,CLOSEST_POINT_AND_PRIMITIVE_ID,duration);
}
