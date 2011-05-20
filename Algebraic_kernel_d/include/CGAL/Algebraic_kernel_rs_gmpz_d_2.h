// Copyright (c) 2011 INRIA Nancy-Grand Est (France).
// Copyright (c) 2011 National and Kapodistrian University of Athens (Greece).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
// Authors: Yacine Bouzidi <yacine.bouzidi@inria.fr>
//          Luis Peñaranda <luis.penaranda@gmx.com>
//          Marc Pouget <marc.pouget@inria.fr>
//          Fabrice Rouillier <fabrice.rouillier@inria.fr>

#ifndef CGAL_ALGEBRAIC_KERNEL_RS_GMPZ_2
#define CGAL_ALGEBRAIC_KERNEL_RS_GMPZ_2

#include <CGAL/Gmpz.h>
#include <CGAL/Polynomial.h>
#include <CGAL/Polynomial_type_generator.h>
#include <CGAL/Gmpfr.h>
#include <CGAL/RS/Algebraic_kernel_rs_2.h>

namespace CGAL{

typedef CGAL::Polynomial_type_generator<Gmpz,2>::Type
                                                __rspoly2;

typedef Algebraic_kernel_rs_2<__rspoly2,Gmpfr>  Algebraic_kernel_rs_gmpz_d_2;

}

#endif  // CGAL_ALGEBRAIC_KERNEL_RS_GMPZ_2
