//----------------------------------------------------------------------------
/// @file create_index.cpp
/// @brief
///
/// @author Copyright (c) 2010 2012 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <iterator>
#include <cassert>

#include <boost/sort/parallel/sort.hpp>

using namespace std ;
namespace bs_sort = boost::sort::parallel ;
using bs_sort::less_ptr_no_null ;

struct member
{	uint32_t num ;
	std::string name;
	float weight;
};
typedef typename vector<member>::iterator iter_t ;

struct cmp_num
{	bool operator() ( const member &m1, const member &m2) const
	{	return ( m1.num < m2.num); };
};

struct cmp_name
{	bool operator() ( const member &m1, const member &m2)const
	{	return ( m1.name < m2.name); };
};

struct cmp_weight
{	bool operator() ( const member &m1, const member &m2) const
	{	return ( m1.weight < m2.weight); };
};

ostream & operator << ( ostream & out, const member &m)
{	out<<m.num<<" - "<<m.name<<" - "<<m.weight<<endl;
	return out ;
};

int main( int, char*[] )
{   //------------------------ begin -----------------------
	// The data are sorted by number
	vector<member> VM = { {1,"Peter",85.6},   {2,"Hanna", 63.4 },
						{3,"John", 83.6}, 	{4,"Elsa",56.6} };
	vector<iter_t> Ix_name, Ix_weight ;

	for ( iter_t it= VM.begin() ; it != VM.end() ; ++it)
	{	Ix_name.push_back ( it);
		Ix_weight.push_back( it);
	};

	typedef less_ptr_no_null <iter_t, cmp_name> 	compare_name ;
	typedef less_ptr_no_null <iter_t, cmp_weight> 	compare_weight ;

	bs_sort::sort (Ix_name.begin()  , Ix_name.end()  , compare_name  ());
	bs_sort::sort (Ix_weight.begin(), Ix_weight.end(), compare_weight());

    cout<<"Printing sorted by number \n";
	for (auto it = VM.begin(); it != VM.end() ; ++it) cout<<(*it);

    cout<<"Printing sorted by name \n";
	for (auto it = Ix_name.begin(); it != Ix_name.end() ; ++it) cout<<(*(*it));

    cout<<"Printing sorted by weight \n";
	for (auto it = Ix_weight.begin(); it != Ix_weight.end() ; ++it) cout<<(*(*it));

    return 0;
};
