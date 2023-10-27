#include <boost/multiprecision/gmp.hpp>

#include<iostream>

#include "cnf.hpp"

using boost::multiprecision::mpz_int;

int main(int argc, char** argv) {
    CNF cnf(argv[1]);
    // cnf.simplify();
    //mpz_int n = cnf.get_nb_vars();
    //mpz_int m = cnf.get_nb_clauses();

    //mpz_int deff = m - n;
    //mpz_int exp = 1;
    //for(mpz_int i = 0; i < deff; i++) {
    //    exp *= 2;
    //}
    std::size_t k = 0;
    for(auto const& c : cnf) {
        k = std::max(k, c.size());
    }

    auto occ = cnf.count_var_occurrences();
    std::size_t t = 0;
    size_t s = 0;
    for(auto const& p : occ) {
        if(p.second > k) {
            t++;
        }
        s = std::max(s, static_cast<std::size_t>(p.second));
    }
    std::cout << argv[1];
    std::cout << ", " << t;
    std::cout << ", " << k;
    std::cout << ", " << s;
    std::cout << "\n";
    return 0;
}
