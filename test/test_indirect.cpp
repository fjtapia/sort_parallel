//----------------------------------------------------------------------------
/// @file test_indirect_sort.hpp
/// @brief Intro Sort algorithm
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "boost/sort/parallel/algorithm/indirect.hpp"
#include "boost/sort/parallel/algorithm/intro_sort.hpp"
#include "boost/sort/parallel/algorithm/parallel_sort.hpp"
#include "boost/sort/parallel/algorithm/parallel_stable_sort.hpp"

#define NELEM 100
using namespace boost::sort::parallel::algorithm;
using namespace std;

struct member
{
    uint32_t num;
    std::string name;
    float weight;
};
typedef typename vector<member>::iterator iter_t;

struct cmp_num
{
    bool operator()(const member &m1, const member &m2) const
    {
        return (m1.num < m2.num);
    };
};

struct cmp_name
{
    bool operator()(const member &m1, const member &m2) const
    {
        return (m1.name < m2.name);
    };
};

struct cmp_weight
{
    bool operator()(const member &m1, const member &m2) const
    {
        return (m1.weight < m2.weight);
    };
};

ostream &operator<<(ostream &out, const member &m)
{
    out << m.num << " - " << m.name << " - " << m.weight << endl;
    return out;
};

int test1(void)
{ //---------------------------- Inicio --------------------------------
    std::vector<unsigned> A = {7,  4, 23, 15, 17, 2, 24, 13, 8,  3,  11,
                               16, 6, 14, 21, 5,  1, 12, 19, 22, 25, 8};

    typedef std::less<unsigned> compare;
    indirect_heap_sort(A.begin(), A.end(), compare());
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK(A[i] <= A[i + 1]);
    };

    unsigned B[] = {1,  2,  3,  5,  6,  7,  8,  9,  10, 11, 12,
                    13, 14, 15, 17, 18, 19, 20, 21, 23, 24, 25};
    indirect_heap_sort(&B[0], &B[22], compare());
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK(B[i] <= B[i + 1]);
    };

    unsigned C[] = {27, 26, 25, 23, 22, 21, 19, 18, 17, 16, 15,
                    14, 13, 11, 10, 9,  8,  7,  6,  5,  3,  2};
    indirect_spin_sort(&C[0], &C[22], compare());
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK(C[i] <= C[i + 1]);
    };

    unsigned D[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    indirect_intro_sort(&D[0], &D[22], compare());
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK(D[i] <= D[i + 1]);
    };

    unsigned F[100];
    for (unsigned i = 0; i < 100; i++) F[i] = rand() % 1000;
    indirect_sample_sort(&F[0], &F[100], compare());
    for (unsigned i = 0; i < 99; i++) {
        BOOST_CHECK(F[i] <= F[i + 1]);
    };

    return 0;
};

int test2(void)
{ //------------------------ begin -----------------------
    // The data are sorted by number
    vector<member> VM = {{1, "Peter", 85.6},
                         {2, "Hanna", 63.4},
                         {3, "John", 83.6},
                         {4, "Elsa", 56.6}};
    vector<iter_t> Ix_name, Ix_weight;

    for (iter_t it = VM.begin(); it != VM.end(); ++it) {
        Ix_name.push_back(it);
        Ix_weight.push_back(it);
    };

    typedef less_ptr_no_null<iter_t, cmp_name> compare_name;
    typedef less_ptr_no_null<iter_t, cmp_weight> compare_weight;
    intro_sort(Ix_name.begin(), Ix_name.end(), compare_name());

    intro_sort(Ix_weight.begin(), Ix_weight.end(), compare_weight());

    cout << "Printing sorted by number \n";
    for (auto it = VM.begin(); it != VM.end(); ++it) cout << (*it);

    cout << "Printing sorted by name \n";
    for (auto it = Ix_name.begin(); it != Ix_name.end(); ++it) cout << (*(*it));

    cout << "Printing sorted by weight \n";
    for (auto it = Ix_weight.begin(); it != Ix_weight.end(); ++it)
        cout << (*(*it));

    return 0;
};
int test_main(int, char *[])
{ //-------------- begin------------
    test1();
    test2();
    return 0;
};
