#ifndef CTLIB_COMPLEX_BOUNDARY_H
#define CTLIB_COMPLEX_BOUNDARY_H
/*******************************************************************************
* -Academic Honesty-
* Plagarism: The unauthorized use or close imitation of the language and 
* thoughts of another author and the representation of them as one's own 
* original work, as by not crediting the author. 
* (Encyclopedia Britannica, 2008.)
*
* You are free to use the code according to the below liscence, but please
* do not commit acts of academic dishonesty. We encourage and request that 
* for any academic use of this source code one should cite the following 
* papers:
* 
* \cite{$bibtex_names_here}
* 
* See ct.bib for the corresponding bibtex entries. 
* !!! DO NOT CITE THE USER MANUAL !!!
*******************************************************************************
* Copyright (C) $NAMES_OF_AUTHORS $YEARS_FIRST_CREATED <$emails>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program in a file entitled COPYING; if not, write to the 
* Free Software Foundation, Inc., 
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*******************************************************************************
* NOTES
* For now this is an input_iterator, abstract_simplex is bidirectional so we 
* could easily store this
*******************************************************************************
*******************************************************************************/

//non exported functionality 
namespace {
template< typename Complex_, typename Term_, typename Cell_boundary_> 
class _const_boundary_iterator: 
public std::iterator< std::input_iterator_tag,
			      Term_,
			      std::ptrdiff_t,
			      const Term_*,
			      const Term_>{
  typedef _const_boundary_iterator< Complex_, Term_, Cell_boundary_> Self;
  typedef Complex_ Complex;
  typedef Term_ Term;
  typedef Cell_boundary_ Cell_boundary;

public:
	//default
	_const_boundary_iterator() {}
	
	//copy
	_const_boundary_iterator( const Self & i):
	complex ( i.complex),
	next_term( i.next_term),
	end_term( i.end_term),
	term( i.term) {}
	
	//move
	_const_boundary_iterator( Self && i):
	complex ( std::move( i.complex)),
	next_term ( std::move( i.next_term)),
	end_term ( std::move( i.end_term)),
	term( std::move( i.term)) { i.complex = NULL; }

	//begin constructor
	_const_boundary_iterator( Complex& _complex, 
				  Cell_boundary & _bd,
				  const typename Complex::Cell& cell):
	  complex( &_complex), 
	  next_term ( _bd.begin( cell)), end_term( _bd.end( cell)){ 
		_next_term();
	}
 
        //end constructor
	_const_boundary_iterator( Complex & _complex): complex( &_complex){ 
	  	_end_term(); 
	}
	Self& operator=( const Self& from){
		complex = from.complex;
		next_term = from.next_term;
		end_term = from.end_term;
		term = from.term;
		return *this;
	}
	bool operator==( const Self & i) const { return term == i.term;}		
	bool operator!=( const Self & i) const { return term != i.term;}		
	Self& operator++(){ 
		_next_term(); 
		return *this;
	}
	Self operator++(int){
		Self tmp = *this;
		_next_term(); 
		return tmp;
	}
	Term& operator*(){ return term; }
	Term* operator->(){ return &term; }
protected:
  void _next_term(){
	if( next_term != end_term){
		term.cell() = complex->find_cell( next_term->cell());
		term.coefficient( next_term->coefficient());
		//mirror the internal filtration position
		//TODO: see if it improves performace,
		//complex_iterators can store this position
		//to avoid the extra dereference
		term.pos_( term.cell()->second.pos_());
		++next_term;
		return;
	}
	_end_term();
  }

  void _end_term(){ term.cell() = complex->end(); }
  //we use a pointer since references are not default constructible
  Complex* complex;
  //if we want to define operator-- 
  //typename Cell_boundary::const_iterator begin_term; 
  typename Cell_boundary::const_iterator next_term;
  typename Cell_boundary::const_iterator end_term;
  Term term;	
}; //class _const_boundary_iterator
} //anonymous namespace

//exported functionality
namespace ctl{

template< typename Complex_,
	  typename Cell_boundary_ = typename Complex_::Boundary,
	  typename Iterator_ = typename Complex_::iterator >
class Complex_boundary{
	typedef Complex_boundary< Complex_> Self;
	typedef typename Cell_boundary_::Term Cell_term;
	typedef typename Cell_term::Coefficient Coefficient;
	public:
	typedef	Complex_ Complex;
	typedef	Cell_boundary_ Cell_boundary;
	typedef Iterator_ Iterator;
	typedef typename Complex::size_type size_type;
	//Complex boundary terms are iterators
	typedef typename Cell_term::template 
			rebind< Iterator, Coefficient>::Term Term;
	typedef _const_boundary_iterator< Complex, Term, Cell_boundary> 
							const_iterator;
	//copy constructor
	Complex_boundary( Complex_boundary & f): _complex( f._complex) {};
	//move constructor, we don't care since we store references
	Complex_boundary( Complex_boundary && f): _complex( f._complex) {};

	Complex_boundary( Complex & complex): _complex( complex) {};
	
	const_iterator begin( const typename Term::Cell & c) {
		return const_iterator( _complex, _complex.boundary(), c->first);
	}
	const_iterator begin( const typename Term::Cell & c, 
			              const std::size_t & pos) {
		c->second.pos_( pos);
		return const_iterator( _complex, _complex.boundary(), c->first);
	}
	const_iterator end( const typename Term::Cell & c) {
		return const_iterator( _complex);
	}
	inline const_iterator end( const typename Term::Cell & c, 
				   const std::size_t & pos) { return end( c); }
	size_type length( const typename Term::Cell & c){
		return _complex.boundary().length( c->first);
	}
		
	private:		
	Complex & _complex;

}; // class Complex_boundary

} //namespace ctl

#endif //CTLIB_COMPLEX_BOUNDARY_H
