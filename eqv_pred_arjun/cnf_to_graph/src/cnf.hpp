//
// Created by oz on 1/12/22.
//

#ifndef BN_CNF_HPP
#define BN_CNF_HPP

#include<set>
#include<vector>
#include<map>
#include<ostream>

class CNF {
public:
    using Literal = int;
    using Clause = std::set<Literal>;
private:
    std::vector<Clause> cnf;
    std::set<Literal> units;
    std::set<Literal> free;
    std::set<Literal> vars;

    std::set<Literal> ind;

public:
    CNF() = default;
    CNF(std::string const& path);
    CNF(CNF const& c) : cnf(c.cnf), units(c.units), free(c.free), vars(c.vars), ind(c.ind) {};
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

    void clear_units() {
        units.clear();
    }

    std::set<Literal> const& get_free_vars() const {
        return free;
    }

    std::set<Literal> const& get_ind() const {
        return ind;
    }

    void set_ind(std::set<Literal> const& s) {
        ind = s;
    }

    bool unsat() const;

    CNF forget(Literal l) const;

    std::map<Literal, int> count_var_occurrences() const;
    Literal find_most_occurring_variable() const;

    std::map<Literal, int> count_ind_occurrences() const;
    Literal find_most_occurring_ind() const;

    std::map<Literal, int> count_literal_occurrences() const;
    Literal find_most_occurring_literal() const;

    std::map<Literal, int> count_ind_literal_occurrences() const;
    Literal find_most_occurring_ind_literal() const;

    Literal random_ind() const;

    void compute_free_vars();

    bool simplify(Literal backbone_lit);
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

    std::vector<int> rename_vars();

    friend std::ostream & operator<<(std::ostream & out, CNF const& cnf);
};

std::ostream & operator<<(std::ostream & out, CNF const& cnf);

bool is_unit(CNF::Clause const& c);
bool is_empty(CNF::Clause const& c);
bool is_trivially_true(CNF::Clause const& c);

#endif //BN_CNF_HPP
