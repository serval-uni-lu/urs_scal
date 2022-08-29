//
// Created by oz on 1/12/22.
//

#ifndef BN_CNF_HPP
#define BN_CNF_HPP

#include<set>
#include<vector>
#include<map>
#include<ostream>

using Literal = int;
using Clause = std::set<Literal>;

bool is_unit(Clause const& c);
bool is_empty(Clause const& c);

class CNF {
private:
    std::vector<Clause> cnf;
    std::set<Literal> units;
    std::set<Literal> free;
    std::set<Literal> vars;

    std::set<Literal> ind;

public:
    CNF() = default;
    CNF(char const* path);
    CNF(CNF const& c) = default;
    CNF(CNF && c) = default;

    CNF& operator=(CNF const& c) = default;
    CNF& operator=(CNF && c) = default;

    std::size_t get_nb_vars() const;
    std::size_t get_nb_clauses() const;
    std::size_t get_nb_units() const;

    std::size_t get_nb_binary() const;

    void resolution_extend();
    void subsumption();

    std::set<Literal> const& get_vars() const {
        return vars;
    }

    std::set<Literal> const& get_units() const {
        return units;
    }

    std::set<Literal> const& get_free_vars() const {
        return free;
    }

    std::set<Literal> const& get_ind() const {
        return ind;
    }

    bool unsat() const;

    std::map<Literal, int> count_var_occurrences() const;
    Literal find_most_occurring_variable() const;

    std::map<Literal, int> count_literal_occurrences() const;
    Literal find_most_occurring_literal() const;

    void compute_free_vars();

    bool simplify(Literal backbone_lit);
    void propagate(std::vector<int> const& backbone);
    bool simplify();
    CNF reduce(std::vector<Literal> & map) const;

    bool replace(Literal s, Literal r);
    bool simplify_binary();

    bool is_free(Literal l) const;
    bool is_const(Literal l) const;
    Literal get_sign_of_const(Literal l) const;

    void merge(CNF const& c);

    std::vector<CNF> split() const;

    void add_clause(Clause const& c);
    int add_var() {
        int n = get_nb_vars() + 1;
        vars.insert(n);
        return n;
    }

    std::vector<Clause>::const_iterator begin() const {
        return cnf.begin();
    }
    std::vector<Clause>::const_iterator end() const {
        return cnf.end();
    }

    std::vector<Clause>::iterator begin() {
        return cnf.begin();
    }
    std::vector<Clause>::iterator end() {
        return cnf.end();
    }

    Clause const& operator[](std::size_t i) const {
        return cnf[i];
    }

    friend std::ostream & operator<<(std::ostream & out, CNF const& cnf);
};

std::ostream & operator<<(std::ostream & out, CNF const& cnf);


#endif //BN_CNF_HPP
