#include<iostream>

#include "cnf.hpp"

int main(int argc, char** argv) {
    CNF cnf(argv[1]);
    //cnf.simplify();

    std::cout << "max: ";
    std::set<int> vars;

    for(std::size_t ci = 0; ci < cnf.get_nb_clauses(); ci++) {
        std::cout << " c" << ci;
        if(ci + 1 < cnf.get_nb_clauses()) {
            std::cout << " +";
        }
        for(int v : cnf[ci]) {
            vars.insert(std::abs(v));
        }
    }
    for(auto const v : vars) {
        std::cout << " - v" << v;
    }
    std::cout << ";\n\n";

    for(std::size_t ci = 0; ci < cnf.get_nb_clauses(); ci++) {
        for(int v : cnf[ci]) {
            v = std::abs(v);
            //std::cout << "c" << ci << " = v" << v << ";\n";
            std::cout << "v" << v << " = c" << ci << ";\n";
        }
    }
    std::cout << "\n";

    for(std::size_t ci = 0; ci < cnf.get_nb_clauses(); ci++) {
        std::cout << "c" << ci << " <= 1;\n";
    }
    std::cout << "\n";

    for(int v : vars) {
        std::cout << "v" << v << " <= 1;\n";
    }
    std::cout << "\n";

    for(std::size_t ci = 0; ci < cnf.get_nb_clauses(); ci++) {
        std::cout << "int c" << ci << ";\n";
    }
    std::cout << "\n";

    for(int v : vars) {
        std::cout << "int v" << v << ";\n";
    }
    std::cout << "\n";
    return 0;
}
