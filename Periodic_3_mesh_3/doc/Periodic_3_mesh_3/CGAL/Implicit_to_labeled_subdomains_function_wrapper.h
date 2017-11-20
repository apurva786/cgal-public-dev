namespace CGAL {

/*!
\ingroup PkgPeriodic_3_mesh_3Domains

The class `Implicit_to_labeled_subdomains_function_wrapper` is an helper class
designed to wrap an implicit function which describes a domain by
[p is inside if f(p)<0] to a function that takes its values into {1, 2} and
thus describes a multidomain.

Note that for the 3D mesh generator [f(p)=0] means that p is outside the domain.
Since this wrapper has values into {1, 2}, both the interior and the exterior of
the periodic domain domain described by the input implicit function are meshed,
thus yielding a periodic mesh of the entire fundamental domain.

\tparam Function provides the definition of the function.
        This parameter stands for a model of the concept `ImplicitFunction`
        described in the surface mesh generation package.
        The number types Function::FT and BGT::FT are required to match.

\sa `Implicit_multi_domain_to_labeling_function_wrapper`.

*/
template<class Function, class BGT>
class Implicit_to_labeled_subdomains_function_wrapper
{
public:
  /// \name Types
  /// @{
  //!
  typedef typename BGT::Point_3   Point_3;
  /// @}

  /// \name Creation
  /// @{
  /*!
   * \brief Construction from an implicit function.
   */
  Implicit_to_labeled_subdomains_function_wrapper (const Function_& f);
/// @}

/// \name Operations
/// @{

  /*!
   * Returns 1 or 2, depending on wether f(p) is negative or not.
   */
  int operator()(const Point_3& p, const bool = true) const;

/// @}


}; /* end Implicit_to_labeled_subdomains_function_wrapper */
/// @}
} /* end namespace CGAL */
