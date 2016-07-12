//----------------------------------------------------------------------------
/// @file test_sort.cpp
/// @brief Benchmark of several sort methods with different uint64_t numbers
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <algorithm>
#include <boost/sort/parallel/sort.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <vector>

#define NELEM 1000000
using namespace std;
namespace bps = boost::sort::parallel;

void test_two_parameters(void)
{
    std::mt19937 my_rand(0);
    vector<uint64_t> A, B, C;
    A.reserve(NELEM);

    for (uint32_t i = 0; i < NELEM; ++i) A.push_back(my_rand());
    B = A;
    std::sort(B.begin(), B.end());
    std::cout << "-----------------------------------\n";

    C = A;
    bps::sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::indirect_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::stable_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::indirect_stable_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::sample_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_stable_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n\n\n\n";
};
void test_three_parameters(void)
{
    std::mt19937 my_rand(0);
    vector<uint64_t> A, B, C;
    A.reserve(NELEM);
    std::less<uint64_t> comp;

    for (uint32_t i = 0; i < NELEM; ++i) A.push_back(my_rand());
    B = A;
    std::sort(B.begin(), B.end(), comp);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::sort(C.begin(), C.end(), comp);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";
    C = A;
    bps::indirect_sort(C.begin(), C.end(), comp);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::stable_sort(C.begin(), C.end(), comp);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::indirect_stable_sort(C.begin(), C.end(), comp);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_sort(C.begin(), C.end(), comp);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::sample_sort(C.begin(), C.end(), comp);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_stable_sort(C.begin(), C.end(), comp);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_sort(C.begin(), C.end(), 8);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::sample_sort(C.begin(), C.end(), 8);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_stable_sort(C.begin(), C.end(), 8);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n\n\n";
};

void test_four_parameters(void)
{
    std::mt19937 my_rand(0);
    vector<uint64_t> A, B, C;
    A.reserve(NELEM);
    std::less<uint64_t> comp;

    for (uint32_t i = 0; i < NELEM; ++i) A.push_back(my_rand());
    B = A;
    std::sort(B.begin(), B.end(), comp);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_sort(C.begin(), C.end(), comp, 100);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::sample_sort(C.begin(), C.end(), comp, 100);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_stable_sort(C.begin(), C.end(), comp, 100);
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i] == C[i]);
    std::cout << "-----------------------------------\n\n\n";
};
void test_stability(void)
{
    struct xk
    {
        uint64_t num;
        xk(uint64_t h) : num(h){};
        bool operator<(xk A) const { return ((num >> 3) < (A.num >> 3)); };
    };
    std::mt19937 my_rand(0);
    vector<xk> A, B, C;
    A.reserve(NELEM);

    for (uint32_t i = 0; i < NELEM; ++i) A.emplace_back(my_rand());
    B = A;
    std::stable_sort(B.begin(), B.end());
    std::cout << "-----------------------------------\n";

    C = A;
    bps::stable_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i].num == C[i].num);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::indirect_stable_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i].num == C[i].num);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::sample_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i].num == C[i].num);
    std::cout << "-----------------------------------\n";

    C = A;
    bps::parallel_stable_sort(C.begin(), C.end());
    BOOST_CHECK(C.size() == B.size());
    for (uint32_t i = 0; i < B.size(); ++i) BOOST_CHECK(B[i].num == C[i].num);
    std::cout << "-----------------------------------\n\n\n\n";
}
int test_main(int argc, char *argv[])
{
    test_two_parameters();
    test_three_parameters();
    test_four_parameters();
    test_stability();
    return 0;
}
