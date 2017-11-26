#include <CGAL/vsa_approximation.h>
#include <CGAL/property_map.h>

template <typename TriangleMesh, typename GeomTraits>
class Mesh_approximation_wrapper {
  typedef typename GeomTraits::FT FT;
  typedef typename GeomTraits::Point_3 Point_3;
  typedef typename GeomTraits::Vector_3 Vector_3;

  typedef typename boost::graph_traits<TriangleMesh>::face_descriptor face_descriptor;
  typedef typename boost::graph_traits<TriangleMesh>::halfedge_descriptor halfedge_descriptor;

  typedef typename boost::property_map<TriangleMesh, boost::vertex_point_t>::type Vertex_point_map;
  typedef boost::associative_property_map<std::map<face_descriptor, FT> > Facet_area_map;
  typedef boost::associative_property_map<std::map<face_descriptor, Point_3> > Facet_center_map;

  typedef CGAL::VSA::Mesh_approximation<TriangleMesh, Vertex_point_map,
    CGAL::Default, CGAL::Default, GeomTraits> L21_approx;
  typedef typename L21_approx::Error_metric L21_metric;
  typedef typename L21_approx::Proxy_fitting L21_proxy_fitting;

  typedef CGAL::VSA::L2_metric<TriangleMesh> L2_metric;
  typedef CGAL::VSA::L2_proxy_fitting<TriangleMesh> L2_proxy_fitting;
  typedef CGAL::VSA::Mesh_approximation<TriangleMesh, Vertex_point_map,
    L2_metric, L2_proxy_fitting, GeomTraits> L2_approx;

  // user defined point-wise compact metric
  struct Compact_metric {
    typedef Point_3 Proxy;

    Compact_metric(const Facet_center_map &_center_pmap)
      : center_pmap(_center_pmap) {}

    FT operator()(const face_descriptor &f, const Proxy &px) const {
      return FT(std::sqrt(CGAL::to_double(
        CGAL::squared_distance(center_pmap[f], px))));
    }

    const Facet_center_map center_pmap;
  };

  struct Point_proxy_fitting {
    typedef Point_3 Proxy;

    Point_proxy_fitting(const Facet_center_map &_center_pmap,
      const Facet_area_map &_area_pmap)
      : center_pmap(_center_pmap),
      area_pmap(_area_pmap) {}

    template <typename FacetIterator>
    Proxy operator()(const FacetIterator beg, const FacetIterator end) const {
      CGAL_assertion(beg != end);

      // fitting center
      Vector_3 center = CGAL::NULL_VECTOR;
      FT area(0);
      for (FacetIterator fitr = beg; fitr != end; ++fitr) {
        center = center + (center_pmap[*fitr] - CGAL::ORIGIN) * area_pmap[*fitr];
        area += area_pmap[*fitr];
      }
      center = center / area;
      return CGAL::ORIGIN + center;
    }

    const Facet_center_map center_pmap;
    const Facet_area_map area_pmap;
  };
  typedef CGAL::VSA::Mesh_approximation<TriangleMesh, Vertex_point_map,
    Compact_metric, Point_proxy_fitting, GeomTraits> Compact_approx;

public:
  enum Metric { L21, L2, Compact };

#ifdef CGAL_SURFACE_MESH_APPROXIMATION_DEBUG
  typedef typename L21_approx::Proxy_wrapper L21_proxy_wrapper;
#endif

  Mesh_approximation_wrapper()
    : m_metric(L21),
    m_center_pmap(m_facet_centers),
    m_area_pmap(m_facet_areas),
    m_pl21_metric(NULL),
    m_pl21_proxy_fitting(NULL),
    m_pl2_metric(NULL),
    m_pl2_proxy_fitting(NULL),
    m_pcompact_metric(NULL),
    m_pcompact_proxy_fitting(NULL) {}

  ~Mesh_approximation_wrapper() {
    if (m_pl21_metric)
      delete m_pl21_metric;
    if (m_pl21_proxy_fitting)
      delete m_pl21_proxy_fitting;
    if (m_pl2_metric)
      delete m_pl2_metric;
    if (m_pl2_proxy_fitting)
      delete m_pl2_proxy_fitting;
    if (m_pcompact_metric)
      delete m_pcompact_metric;
    if (m_pcompact_proxy_fitting)
      delete m_pcompact_proxy_fitting;
  }

  void set_mesh(const TriangleMesh &mesh) {
    Vertex_point_map vpm = get(boost::vertex_point, const_cast<TriangleMesh &>(mesh));

    m_facet_centers.clear();
    m_facet_areas.clear();
    BOOST_FOREACH(face_descriptor f, faces(mesh)) {
      const halfedge_descriptor he = halfedge(f, mesh);
      const Point_3 &p0 = vpm[source(he, mesh)];
      const Point_3 &p1 = vpm[target(he, mesh)];
      const Point_3 &p2 = vpm[target(next(he, mesh), mesh)];

      m_facet_centers.insert(std::pair<face_descriptor, Point_3>(
        f, CGAL::centroid(p0, p1, p2)));

      FT area(std::sqrt(CGAL::to_double(CGAL::squared_area(p0, p1, p2))));
      m_facet_areas.insert(std::pair<face_descriptor, FT>(f, area));
    }

    if (m_pl21_metric)
      delete m_pl21_metric;
    if (m_pl21_proxy_fitting)
      delete m_pl21_proxy_fitting;
    if (m_pl2_metric)
      delete m_pl2_metric;
    if (m_pl2_proxy_fitting)
      delete m_pl2_proxy_fitting;
    if (m_pcompact_metric)
      delete m_pcompact_metric;
    if (m_pcompact_proxy_fitting)
      delete m_pcompact_proxy_fitting;

    m_pl21_metric = new L21_metric(mesh);
    m_pl21_proxy_fitting = new L21_proxy_fitting(mesh);
    m_pl2_metric = new L2_metric(mesh);
    m_pl2_proxy_fitting = new L2_proxy_fitting(mesh);
    m_pcompact_metric = new Compact_metric(m_center_pmap);
    m_pcompact_proxy_fitting = new Point_proxy_fitting(m_center_pmap, m_area_pmap);

    m_l21_approx.set_mesh(mesh, vpm);
    m_l21_approx.set_metric(*m_pl21_metric, *m_pl21_proxy_fitting);

    m_l2_approx.set_mesh(mesh, vpm);
    m_l2_approx.set_metric(*m_pl2_metric, *m_pl2_proxy_fitting);

    m_iso_approx.set_mesh(mesh, vpm);
    m_iso_approx.set_metric(*m_pcompact_metric, *m_pcompact_proxy_fitting);
  }

  void set_metric(const Metric m) {
    m_metric = m;
    switch (m_metric) {
      case L21: return m_l21_approx.rebuild();
      case L2: return m_l2_approx.rebuild();
      case Compact: return m_iso_approx.rebuild();
    }
  }

  std::size_t seeding(const CGAL::VSA::Seeding method,
    const boost::optional<std::size_t> max_nb_proxies,
    const boost::optional<FT> min_error_drop,
    const std::size_t nb_relaxations) {
    switch (m_metric) {
      case L21:
        return m_l21_approx.seeding(
          method, max_nb_proxies, min_error_drop, nb_relaxations);
      case L2:
        return m_l2_approx.seeding(
          method, max_nb_proxies, min_error_drop, nb_relaxations);
      case Compact:
        return m_iso_approx.seeding(
          method, max_nb_proxies, min_error_drop, nb_relaxations);
    }
    return 0;
  }

  std::size_t run_one_step() {
    switch (m_metric) {
      case L21:
        return m_l21_approx.run();
      case L2:
        return m_l2_approx.run();
      case Compact:
        return m_iso_approx.run();
    }
    return 0;
  }

  std::size_t add_one_proxy() {
    switch (m_metric) {
      case L21:
        return m_l21_approx.add_proxies_furthest(1, 0);
      case L2:
        return m_l2_approx.add_proxies_furthest(1, 0);
      case Compact:
        return m_iso_approx.add_proxies_furthest(1, 0);
    }
    return 0;
  }

  std::size_t teleport_one_proxy() {
    switch (m_metric) {
      case L21:
        return m_l21_approx.teleport_proxies(1, 0, true);
      case L2:
        return m_l2_approx.teleport_proxies(1, 0, true);
      case Compact:
        return m_iso_approx.teleport_proxies(1, 0, true);
    }
    return 0;
  }

  template <typename PolyhedronSurface>
  bool extract_mesh(PolyhedronSurface &mesh_out, const FT chord_error, bool pca_plane) {
    switch (m_metric) {
      case L21:
        return m_l21_approx.extract_mesh(mesh_out, chord_error, pca_plane);
      case L2:
        return m_l2_approx.extract_mesh(mesh_out, chord_error, pca_plane);
      case Compact:
        return m_iso_approx.extract_mesh(mesh_out, chord_error, pca_plane);
    }
    return false;
  }

  std::size_t get_proxies_size() {
    switch (m_metric) {
      case L21:
        return m_l21_approx.get_proxies_size();
      case L2:
        return m_l2_approx.get_proxies_size();
      case Compact:
        return m_iso_approx.get_proxies_size();
    }
    return 0;
  }

#ifdef CGAL_SURFACE_MESH_APPROXIMATION_DEBUG
  template <typename OutputIterator>
  void get_l21_proxies(OutputIterator outitr) {
    switch (m_metric) {
      case L21:
        return m_l21_approx.get_wrapped_proxies(outitr);
      default:
        return;
    }
  }
#endif

  template <typename FacetProxyMap>
  void get_proxy_map(FacetProxyMap &fpmap) {
    switch (m_metric) {
      case L21:
        return m_l21_approx.get_proxy_map(fpmap);
      case L2:
        return m_l2_approx.get_proxy_map(fpmap);
      case Compact:
        return m_iso_approx.get_proxy_map(fpmap);
    }
  }

  template <typename OutputIterator>
  void get_indexed_triangles(OutputIterator outitr) {
    switch (m_metric) {
      case L21:
        return m_l21_approx.get_indexed_triangles(outitr);
      case L2:
        return m_l2_approx.get_indexed_triangles(outitr);
      case Compact:
        return m_iso_approx.get_indexed_triangles(outitr);
    }
  }

  template <typename OutputIterator>
  void get_anchor_points(OutputIterator outitr) {
    switch (m_metric) {
      case L21:
        return m_l21_approx.get_anchor_points(outitr);
      case L2:
        return m_l2_approx.get_anchor_points(outitr);
      case Compact:
        return m_iso_approx.get_anchor_points(outitr);
    }
  }

  template <typename OutputIterator>
  void get_anchor_vertices(OutputIterator outitr) {
    switch (m_metric) {
      case L21:
        return m_l21_approx.get_anchor_vertices(outitr);
      case L2:
        return m_l2_approx.get_anchor_vertices(outitr);
      case Compact:
        return m_iso_approx.get_anchor_vertices(outitr);
    }
  }

  template <typename OutputIterator>
  void get_indexed_boundary_polygons(OutputIterator outitr) {
    switch (m_metric) {
      case L21:
        return m_l21_approx.get_indexed_boundary_polygons(outitr);
      case L2:
        return m_l2_approx.get_indexed_boundary_polygons(outitr);
      case Compact:
        return m_iso_approx.get_indexed_boundary_polygons(outitr);
    }
  }

private:
  Metric m_metric; // current metric

  // facet property maps
  std::map<face_descriptor, Point_3> m_facet_centers;
  Facet_center_map m_center_pmap;
  std::map<face_descriptor, FT> m_facet_areas;
  Facet_area_map m_area_pmap;

  L21_metric *m_pl21_metric;
  L21_proxy_fitting *m_pl21_proxy_fitting;
  L21_approx m_l21_approx;

  L2_metric *m_pl2_metric;
  L2_proxy_fitting *m_pl2_proxy_fitting;
  L2_approx m_l2_approx;

  Compact_metric *m_pcompact_metric;
  Point_proxy_fitting *m_pcompact_proxy_fitting;
  Compact_approx m_iso_approx;
};
