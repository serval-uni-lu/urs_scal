//
// Created by oz on 2/3/22.
//

#include<iostream>

#include "cnf.hpp"

#include<fstream>
#include <cmath>
#include <set>

struct Edge {
    std::size_t i1;
    std::size_t i2;
};

bool operator<(Edge const& l, Edge const& r) {
    return (l.i1 < r.i1) || (l.i1 == r.i1 && l.i2 < r.i2);
}

int main(int argc, char** argv) {
    std::set<Edge > edges;

    std::string path(argv[1]);
    CNF cnf(path);
    //cnf.simplify();
    //std::cout << "old size: " << cnf.get_nb_clauses() << " + " << cnf.get_nb_units() << std::endl;

    //for(int i = 2; i < argc; i++) {
        //int lit = std::stoi(argv[i]);
        //std::cout << "forget: " << lit << std::endl;
        //cnf = cnf.forget(lit);
        //std::cout << "new size: " << cnf.get_nb_clauses() << " + " << cnf.get_nb_units() << std::endl;
    //}

    //auto map = cnf.rename_vars();

    //for(std::size_t i = 1; i < map.size(); i++) {
        //std::cout << i << " -> " << map[i] << std::endl;
    //}

    //std::ofstream gr(path + ".gr");
    std::ostream& gr = std::cout;
    //std::ofstream dot(path + ".dot");
    //std::ofstream red(path + ".red");

    //red << "c ind ";
    //for(int i : cnf.get_ind()) {
        //red << i << " ";
    //}
    //red << "0\n";

    //red << "p cnf " << cnf.get_nb_vars() << " " << (cnf.get_nb_clauses() + cnf.get_nb_units()) << std::endl;
    //for(auto const& c : cnf) {
        //for(int i : c) {
            //red << i << " ";
        //}
        //red << "0\n";
    //}
    //for(int i : cnf.get_units()) {
        //red << i << " 0\n";
    //}

    for(auto const& c : cnf) {
        for(int i1 : c) {
            i1 = std::abs(i1);
            for(int i2 : c) {
                i2 = std::abs(i2);

                if(i1 != i2) {
                    Edge e{};
                    e.i1 = std::min(i1, i2);
                    e.i2 = std::max(i1, i2);
                    edges.insert(e);
                }
            }
        }
    }

    std::vector<std::size_t> edge_count;
    for(std::size_t i = 0; i <= cnf.get_nb_vars(); i++) {
        edge_count.emplace_back(0);
    }

    for(auto const& e : edges) {
        edge_count[e.i1]++;
        edge_count[e.i2]++;
    }

    gr << "p tw " << cnf.get_nb_vars() << " " << edges.size() << std::endl;
    //dot << "graph G {\n";

    //for(std::size_t i = 1; i < edge_count.size(); i++) {
        //dot << i << "[label=\"" << i << " : " << edge_count[i] << "\"];\n";
    //}

    for(auto const& e : edges) {
        std::size_t f = e.i1;
        std::size_t l = e.i2;

        gr << f << " " << l << std::endl;
        //dot << f << " -- " << l << ";\n";
    }
    //dot << "}";

    //gr.close();
    //dot.close();
    //red.close();

    return 0;
}
