// Copyright (c) 1997-2012  ETH Zurich (Switzerland).
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
// $URL$
// $Id$
// 
//
// Author(s)     : Sven Schoenherr
//                 Bernd Gaertner <gaertner@inf.ethz.ch>
//                 Franz Wessendorp
//                 Kaspar Fischer
//                 Yves Brise

#ifndef CGAL_QP__FILTERED_BASE_H
#define CGAL_QP__FILTERED_BASE_H

// includes
#include <CGAL/QP_solver/QP_pricing_strategy.h>
#include <CGAL/QP_solver/QP_solver.h>
#include <cmath>


namespace CGAL {

// ==================
// class declarations
// ==================
template < typename Q, typename ET, typename Tags, class NT_ = double, class ET2NT_ =
    To_double<ET> >
class QP__filtered_base;

// ===============
// class interface
// ===============
template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >
class QP__filtered_base : virtual public QP_pricing_strategy<Q, ET, Tags> {

    // self
    typedef  QP__filtered_base<Q, ET, Tags>    Self;
    typedef  QP_pricing_strategy<Q, ET, Tags>  Base;

  public:

    // number type
    typedef  NT_                        NT;
    typedef  ET2NT_                     ET2NT;

  protected:

    // construction
    QP__filtered_base( ET2NT et2nt = ET2NT());
    
    // initialization
    virtual  void  set ( );
    virtual  void  init( );

    // access
    const ET2NT&  et2nt_object( ) const { return et2nt_obj; }

    // operations
    void  init_NT( );
    NT mu_j_NT( int j) const
    {
      return mu_j_NT(j, Is_nonnegative());
    } 

    void  update_maxima( );

    // this function returns true if j is not a candidate for the
    // entering variable. This can be deduced if the inexact mu_j 
    // is sufficiently far away from 0. This test uses the error 
    // bounds from Sven's thesis (p.99), see also the C-file. Only
    // if these bounds are insufficient, exact arithmetic is used
    bool  certify_mu_j_NT( int j) const {
      return certify_mu_j_NT( j, Is_nonnegative());
    }

    virtual  void  transition( );

    // constants (protected)
    const NT                 nt0, nt1;  // small constants of NT

  private:
    // types from Tags
    typedef  typename Tags::Is_linear    Is_linear;
    typedef  typename Tags::Is_nonnegative Is_nonnegative;

    // private member functions
    void  set( int l, Tag_true  is_linear);
    void  set( int l, Tag_false is_linear);

    void  init( Tag_true  is_linear);
    void  init( Tag_false is_linear);

    void  init_NT( Tag_true  is_linear);
    void  init_NT( Tag_false is_linear);

    void  update_maxima( Tag_true  is_linear);
    void  update_maxima( Tag_false is_linear);

    void  transition( int n, Tag_true  is_linear);
    void  transition( int n, Tag_false is_linear);    
    
    NT    mu_j_NT( int j, Tag_true is_in_standard_form) const; 
    NT    mu_j_NT( int j, Tag_false is_in_standard_form) const { 
      return  mu_j_NT(j, is_in_standard_form, Is_linear());
    }
    NT    mu_j_NT( int j, Tag_false, Tag_true) const; // variable bounds, LP
    NT    mu_j_NT( int j, Tag_false, Tag_false) const; // variable bounds, QP
    bool  certify_mu_j_NT( int j, Tag_true) const; // standard form
    bool  certify_mu_j_NT( int j, Tag_false) const; // variable bounds

    // the q-parameter in the error bound
    void set_q(int c, int b) {
      set_q(c, b, Is_nonnegative());
    }
    void set_q(int c, int b, Tag_true);
    void set_q(int c, int b, Tag_false);

    // some more types
    typedef  typename Q::A_iterator   A_iterator;
    // TAG: 0SWITCH
    typedef  typename QP_model_detail::Sparse_iterator_adaptor<Q, typename Q::Is_sparse>::A_sparse_iterator A_sparse_iterator;
    //typedef  typename Q::A_sparse_iterator A_sparse_iterator;
    // TAG: 0SWITCH
    typedef  typename QP_model_detail::Sparse_iterator_adaptor<Q, typename Q::Is_sparse>::A_sparse_column_iterator A_sparse_column_iterator;
    //typedef  typename Q::A_sparse_column_iterator A_sparse_column_iterator;
    typedef  typename Q::C_iterator   C_iterator;   
    // TAG: 0SWITCH
    typedef  typename QP_model_detail::Sparse_iterator_adaptor<Q, typename Q::Is_sparse>::D_sparse_column_iterator D_sparse_column_iterator;
    //typedef  typename std::iterator_traits<typename Q::D_iterator>::value_type D_row_iterator;
    
    typedef  typename Q::R_iterator R_iterator;
					
    typedef typename Base::QP_solver::C_auxiliary_iterator C_auxiliary_iterator;

    typedef  typename Base::QP_solver::Basic_variable_index_iterator
                                        Basic_variable_index_iterator;
    typedef  typename Base::QP_solver::Basic_constraint_index_iterator
                                        Basic_constraint_index_iterator;

    typedef  std::vector<NT>            Values_NT;
    typedef  typename Values_NT::const_iterator  Values_NT_iterator;

    // data members
    int n;                              // number of solver variables
    NT q;                               // for the error bounds 
    mutable
    NT w_j_NT;                          // inexact version of w[j]
  
    ET2NT                    et2nt_obj; // conversion from ET to NT

    Values_NT                lambda_NT; // NT version of lambda (from KKT)
    Values_NT                x_B_O_NT;  // NT version of basic vars (orig.)
    NT                       d_NT;      // NT version of common denominator

    NT                       row_max_c; // row maximum of vector 'c'
    Values_NT                row_max_A; // row maxima of matrix 'A'
    Values_NT                row_max_D; // row maxima of matrix 'D'
    Values_NT                col_max;   // column maxima of 'c', 'A', and 'D'

    std::vector<bool>        handled_A; // flags: rows of A already handled
    std::vector<bool>        handled_D; // flags: rows of D already handled

    NT                       bound1;    // first bound for certification
    NT                       bound2_wq; // common part of second bounds
};

// ----------------------------------------------------------------------------

// =============================
// class implementation (inline)
// =============================

// construction
template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline
QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
QP__filtered_base( ET2NT et2nt)
    : nt0( 0), nt1( 1), w_j_NT( 0), et2nt_obj( et2nt)
{ }

// set-up
template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ > inline         // QP case
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
set( int l, Tag_false)
{
    x_B_O_NT.resize( l, nt0);
}

template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ > inline         // LP case
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
set( int l, Tag_true)
{
     x_B_O_NT.resize( l, nt0);   
}

// initialization
template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline        // QP case
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
init( Tag_false)
{
    if ( ! row_max_D.empty()) row_max_D.clear();
    if ( ! handled_D.empty()) handled_D.clear();
}

template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline        // LP case
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
init( Tag_true)
{
    // nop
}

// operations
template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline        // QP case
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
init_NT( Tag_false)
{
    if ( this->solver().number_of_basic_original_variables()
	 > static_cast< int>( x_B_O_NT.size())) x_B_O_NT.push_back( nt0);

    std::transform( this->solver().basic_original_variables_numerator_begin(),
		    this->solver().basic_original_variables_numerator_end(),
		    x_B_O_NT.begin(), et2nt_obj);
}

template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline        // LP case
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
init_NT( Tag_true)
{
    // nop
}


template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline
NT_  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
mu_j_NT( int j, Tag_true) const // standard form
{
    return this->solver().mu_j( j, lambda_NT.begin(), x_B_O_NT.begin(), d_NT);
}

template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline
NT_  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
mu_j_NT( int j, Tag_false, Tag_true) const // variable bounds, LP case
{
  return this->solver().mu_j( j, lambda_NT.begin(), x_B_O_NT.begin(), d_NT);
}

template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline
NT_  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
mu_j_NT( int j, Tag_false, Tag_false) const // variable bounds, QP case
{
  w_j_NT = ( (j < n && this->solver().phase() == 2) ? 
		et2nt_obj( this->solver().w_j_numerator(j)) : nt0 );
  return this->solver().mu_j( j, lambda_NT.begin(), 
				x_B_O_NT.begin(), w_j_NT, d_NT);
}


// transition
template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline        // QP case
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
transition( int n, Tag_false)
{
    handled_D.insert( handled_D.end(), n, false);
    row_max_D.insert( row_max_D.end(), n, nt0);
}

template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline        // LP case
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
transition( int, Tag_true)
{
    // nop
}

template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline // standard form
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
set_q(int c, int b, Tag_true) 
{
   q = std::ldexp( 1.015625 * ( c+b+1) * ( c+b+2), -53);
}

template < typename Q, typename ET, typename Tags, class NT_, class ET2NT_ >  inline // variable bounds
void  QP__filtered_base<Q,ET,Tags,NT_,ET2NT_>::
set_q(int c, int b, Tag_false) 
{
   q = std::ldexp( 1.015625 * ( c+b+2) * ( c+b+3), -53);
}

} //namespace CGAL

#include <CGAL/QP_solver/QP__filtered_base_impl.h>

#endif // CGAL_QP__FILTERED_BASE_H

// ===== EOF ==================================================================
