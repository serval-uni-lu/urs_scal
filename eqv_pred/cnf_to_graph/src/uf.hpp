//
// Created by oz on 1/11/22.
//

#ifndef BN_UF_HPP
#define BN_UF_HPP

#include <vector>
// https://github.com/kartikkukreja/blog-codes/blob/master/src/Union%20Find%20%28Disjoint%20Set%29%20Data%20Structure.cpp

class UF    {
    int cnt;

    std::vector<int> id;
    std::vector<int> sz;
public:
    // Create an empty union find data structure with N isolated sets.
    UF(int N);
    ~UF();
    // Return the id of component corresponding to object p.
    int find(int p);
    // Replace sets containing x and y with their union.
    void merge(int x, int y);
    // Are objects x and y in the same set?
    bool connected(int x, int y);
    // Return the number of disjoint sets.
    int count();
};


#endif //BN_UF_HPP
