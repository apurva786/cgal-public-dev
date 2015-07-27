// Copyright (c) 2009 INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/trunk/Mesh_3/include/CGAL/Mesh_3/implicit_to_labeled_function_wrapper.h $
// $Id: implicit_to_labeled_function_wrapper.h 56231 2010-05-14 09:46:02Z afabri $
//
//
// Author(s)     : Mikhail Bogdanov
//
//******************************************************************************
// File Description :
// Implicit_to_labeled_subdomains_function_wrapper
//
// See classes description to have more information.
//******************************************************************************

#ifndef CGAL_MESH_3_IMPLICIT_TO_LABELED_SUBDOMAINS_FUNCTION_WRAPPER_H
#define CGAL_MESH_3_IMPLICIT_TO_LABELED_SUBDOMAINS_FUNCTION_WRAPPER_H


namespace CGAL {

/**
 * @class Implicit_to_labeled_function_wrapper
 *
 * This class is designed to wrap an implicit function which describes a domain
 * by [p is inside if f(p)<0] to a function which takes its values into {1, 2}.
 * f(p)=0 means that p is outside the domain.
 */
template<class Function_, class BGT>
class Implicit_to_labeled_subdomains_function_wrapper
{
public:
  // Types
  typedef int                     return_type;
  typedef typename BGT::Point_3   Point_3;

  /// Constructor
  Implicit_to_labeled_subdomains_function_wrapper(Function_& f)
    : r_f_(f) {}

  // Default copy constructor and assignment operator are ok

  /// Destructor
  ~Implicit_to_labeled_subdomains_function_wrapper() {}

  /// Operator ()
  return_type operator()(const Point_3& p, const bool = true) const
  {
    return ( (r_f_(p)<0) ? 1 : 2 );
  }

private:
  /// Function to wrap
  Function_& r_f_;

};  // end class Implicit_to_labeled_subdomains_function_wrapper

}  // end namespace CGAL


#endif // CGAL_MESH_3_IMPLICIT_TO_LABELED_SUBDOMAINS_FUNCTION_WRAPPER_H
