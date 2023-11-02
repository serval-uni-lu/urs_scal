//
// Created by oz on 1/11/22.
//

#include "uf.hpp"

UF::UF(int N) : id(N), sz(N)  {
    cnt = N;
    for(int i=0; i<N; i++)	{
        id[i] = i;
        sz[i] = 1;
    }
}
UF::~UF()	{
}

int UF::find(int p)	{
    int root = p;
    while (root != id[root])
        root = id[root];
    while (p != root) {
        int newp = id[p];
        id[p] = root;
        p = newp;
    }
    return root;
}

void UF::merge(int x, int y)	{
    int i = find(x);
    int j = find(y);
    if (i == j) return;

    // make smaller root point to larger one
    if   (sz[i] < sz[j])	{
        id[i] = j;
        sz[j] += sz[i];
    } else	{
        id[j] = i;
        sz[i] += sz[j];
    }
    cnt--;
}

bool UF::connected(int x, int y)    {
    return find(x) == find(y);
}

int UF::count() {
    return cnt;
}