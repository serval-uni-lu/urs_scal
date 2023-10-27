//
// Created by oz on 3/1/22.
//

#include<iostream>

#include<fstream>
#include <cmath>
#include <string>
#include <set>
#include <vector>
#include <sstream>

struct Edge {
    std::size_t i1;
    std::size_t i2;
};

bool operator<(Edge const& l, Edge const& r) {
    return (l.i1 < r.i1) || (l.i1 == r.i1 && l.i2 < r.i2);
}

struct Tree {
    std::vector<std::set<int> > bags;
    std::set<Edge> edges;
};

Tree load(std::string const& path) {
    std::ifstream f(path);

    if(!f) {
        std::cerr << "error opening file " << path << "\n";
        exit(1);
    }

    Tree res;

    std::string line;
    while(getline(f, line)) {
        if(line.rfind("s td", 0) == 0) {
            std::stringstream iss(line);
            std::string tmp;
            iss >> tmp >> tmp;
            int t;
            iss >> t;

            for(int i = 0; i <= t; i++) {
                res.bags.emplace_back();
            }
        }
        else if(line.rfind("b ", 0) == 0) {
            std::stringstream iss(line);
            std::string tmp;
            std::size_t i, pos;
            iss >> tmp >> pos;
            iss >> i;
            while (iss) {
                res.bags[pos].insert(i);
                iss >> i;
            }
        }
        else if(line[0] != 'c') {
            Edge tmp{};
            std::stringstream iss(line);

            iss >> tmp.i1 >> tmp.i2;
            res.edges.insert(tmp);
        }
    }

    return res;
}

void tree_to_dot(Tree const& t, std::string const& path) {
    std::ofstream out(path);
    out << "digraph G {\n";

    for(std::size_t i = 1; i < t.bags.size(); i++) {
        out << i << "[label=\"";
        for(int j : t.bags[i]) {
            out << j << " ";
        }
        out << "\"];\n";
    }

    for(Edge const& e : t.edges) {
        out << e.i1 << " -> " << e.i2 << ";\n";
    }

    out << "}";
    out.close();
}

int main(int argc, char** argv) {
    std::string path(argv[1]);
    auto tree = load(path);
    tree_to_dot(tree, path + ".dot");
    return 0;
}