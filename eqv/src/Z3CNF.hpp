//
// Created by oz on 12/10/21.
//

#ifndef SAMPLER_CNF_HPP
#define SAMPLER_CNF_HPP

#include<fstream>
#include<string>
#include<set>
#include<vector>
#include<map>

#include<z3++.h>

#include "CNF.hpp"

class Z3CNF {
private:
    mutable z3::context context;
    z3::optimize solver;
    CNF cnf;
    std::set<Literal> vars;

public:
    Z3CNF();
    Z3CNF(CNF const& cnf);
    Z3CNF(Z3CNF const& c);
    //Z3CNF(Z3CNF && c) = default;

    //Z3CNF& operator=(Z3CNF const& c) = default;
    //Z3CNF& operator=(Z3CNF && c) = default;

    z3::expr literal(int v) const;
    int get_nb_vars() const;

    void push();
    void pop();
    void add_clause(std::set<int> const& c);

    CNF const& get_cnf() const {
        return cnf;
    }
    std::set<Literal> const& get_vars() const {
        return vars;
    }

    z3::check_result check();
    z3::model get_model() const;

    z3::model max_sat_sample(bool & sat);
    z3::model max_sat_sample(bool & sat, std::string const& m);
    z3::model max_sat_sample(bool & sat, std::set<int> const& m);
    z3::model max_sat_sample(bool & sat, std::map<int, bool> const& m);

    void remove_model(z3::model const& m);
    void remove_model(std::map<int, bool> const& m);

    z3::model partial_sat_sample(bool & sat, std::size_t nb);

    friend std::ostream& operator<<(std::ostream& o, Z3CNF const& cnf);
    friend std::map<int, bool> model_to_map(Z3CNF const& c, z3::model const& m);
};

std::ostream& operator<<(std::ostream& o, Z3CNF const& cnf);
std::string model_to_string(Z3CNF const& c, z3::model const& m, bool ignore_dep = false);
std::string model_to_string(Z3CNF const& c);
std::map<int, bool> model_to_map(Z3CNF const& c, z3::model const& m);
std::vector<bool> model_to_vector(Z3CNF const& c, z3::model const& m);
std::vector<int> model_to_set(Z3CNF const& c, z3::model const& m);
std::string model_to_string(std::map<int, bool> const& m);


#endif //SAMPLER_CNF_HPP
