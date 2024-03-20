//
// Created by oz on 12/10/21.
//

#include "Z3CNF.hpp"

#include <random>
#include <sstream>
#include <algorithm>

Z3CNF::Z3CNF() : context(), solver(context) {
}

Z3CNF::Z3CNF(CNF const& cnf) : context(), solver(context), cnf(cnf) {
    for(Clause const& c : cnf) {
        z3::expr_vector clause(context);
        for(Literal l : c) {
            clause.push_back(literal(l));

            vars.insert(std::abs(l));
        }

        solver.add(mk_or(clause));
    }
}

Z3CNF::Z3CNF(Z3CNF const& cnf) : context(), solver(context), cnf(cnf.cnf) {
    for(Clause const& c : this->cnf) {
        z3::expr_vector clause(context);
        for(Literal l : c) {
            clause.push_back(literal(l));

            vars.insert(std::abs(l));
        }

        solver.add(mk_or(clause));
    }
}

z3::expr Z3CNF::literal(int v) const {
    z3::expr t = context.constant(context.int_symbol(std::abs(v)), context.bool_sort());
    if(v < 0) {
        t = !t;
    }
    return t;
}

int Z3CNF::get_nb_vars() const {
    return cnf.get_nb_vars();
}

z3::check_result Z3CNF::check() {
    return solver.check();
}

z3::model Z3CNF::get_model() const {
    return solver.get_model();
}

std::ostream& operator<<(std::ostream& o, Z3CNF const& cnf) {
    z3::model const m = cnf.get_model();
    o << "cnf model\n";
    for(Literal i : cnf.get_vars()) {
        z3::func_decl d(cnf.literal(i).decl());
        z3::expr expr = m.get_const_interp(d);
        o << i << " : " << (expr.bool_value() == Z3_L_TRUE) << "\n";
    }

    o << "\nZ3 format:\n";
    o << cnf.solver << "\n";

    return o;
}

z3::model Z3CNF::max_sat_sample(bool & sat) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 1);

    std::map<int, bool> res;
    for(int i : vars) {
        res[i] = dist(mt);
    }

    return max_sat_sample(sat, res);
}

void Z3CNF::push() {
    solver.push();
}

void Z3CNF::pop() {
    solver.pop();
}

void Z3CNF::add_clause(std::set<int> const& c) {
    z3::expr_vector clause(context);

    for(int i : c) {
        clause.push_back(literal(i));
    }

    solver.add(mk_or(clause));
}

z3::model Z3CNF::max_sat_sample(bool &sat, const std::string &m) {
    solver.push();

    for(int i : vars) {
        if (m[i - 1] == '1') {
            solver.add_soft(literal(i), 1);
        } else {
            solver.add_soft(literal(-i), 1);
        }
    }

    sat = check() == z3::sat;
    z3::model res = get_model();
    solver.pop();
    return res;
}

z3::model Z3CNF::max_sat_sample(bool &sat, const std::map<int, bool> &m) {
    solver.push();

    for(int i : vars) {
        if(m.at(i)) {
            solver.add_soft(literal(i), 1);
        }
        else {
            solver.add_soft(literal(-i), 1);
        }
    }

    sat = check() == z3::sat;
    z3::model res = get_model();
    solver.pop();
    return res;
}

z3::model Z3CNF::max_sat_sample(bool & sat, std::set<int> const& m) {
    solver.push();

    for(int i : m) {
        solver.add_soft(literal(i), 1);
    }

    sat = check() == z3::sat;
    z3::model res = get_model();
    solver.pop();
    return res;
}

void Z3CNF::remove_model(const z3::model &m) {
    z3::expr_vector res(context);

    for(int i : vars) {
        z3::func_decl d(literal(i).decl());
        z3::expr e = m.get_const_interp(d);

        int v = i;
        if (e.bool_value() == Z3_L_TRUE) {
            v = -i;
        }

        res.push_back(literal(v));
    }

    solver.add(mk_or(res));
}

void Z3CNF::remove_model(std::map<int, bool> const& m) {
    z3::expr_vector  res(context);

    for(int i : vars) {
        int v = i;
        if(m.at(i)) {
            v = -i;
        }

        res.push_back(literal(v));
    }

    solver.add(mk_or(res));
}

std::string model_to_string(Z3CNF const& c) {
    return model_to_string(c, c.get_model());
}

std::string model_to_string(Z3CNF const& c, z3::model const& m, bool ignore_dep) {
    std::stringstream oss;

    for(int i = 1; i <= c.get_nb_vars(); i++) {
        if(!ignore_dep || c.get_vars().find(i) != c.get_vars().end()) {
            z3::func_decl d(c.literal(i).decl());
            z3::expr expr = m.get_const_interp(d);
            //oss << (expr.bool_value() == Z3_L_TRUE);
            oss << ((expr.bool_value() == Z3_L_TRUE) ? i : -i) << " ";
        }
    }

    return oss.str();
}

std::map<int, bool> model_to_map(Z3CNF const& c, z3::model const& m) {
    std::map<int, bool> res;
    for(int i : c.vars) {
        z3::func_decl d(c.literal(i).decl());
        z3::expr expr = m.get_const_interp(d);
        res[i] = expr.bool_value() == Z3_L_TRUE;
    }

    return res;
}

std::vector<bool> model_to_vector(Z3CNF const& c, z3::model const& m) {
    std::vector<bool> res;
    res.reserve(c.get_nb_vars());

    for(int i : c.get_vars()) {
        z3::func_decl d(c.literal(i).decl());
        z3::expr expr = m.get_const_interp(d);
        res.push_back(expr.bool_value() == Z3_L_TRUE);
    }

    return res;
}

std::vector<int> model_to_set(Z3CNF const& c, z3::model const& m) {
    std::vector<int> res;

    for(int i = 1; i <= c.get_nb_vars(); i++) {
        z3::func_decl d(c.literal(i).decl());
        z3::expr expr = m.get_const_interp(d);

        if(expr.bool_value() == Z3_L_TRUE) {
            res.push_back(i);
        }
        else {
            res.push_back(-i);
        }
    }

    return res;
}

std::string model_to_string(std::map<int, bool> const& m) {
    std::string res;

    for(auto i : m) {
        if(i.second) {
            res += "1";
        }
        else {
            res += "0";
        }
    }

    return res;
}

void shuffle_vec(std::vector<int> & v) {
    std::random_device rd;
    std::default_random_engine rng(rd());

    std::shuffle(v.begin(), v.end(), rng);
}

z3::model Z3CNF::partial_sat_sample(bool & sat, std::size_t nb) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 1);

    std::vector<int> v(vars.begin(), vars.end());
    shuffle_vec(v);

    solver.push();
    for(std::size_t i = 0; i < nb; i++) {
        if(dist(mt)) {
            solver.add(literal(v[i]));
        }
        else {
            solver.add(literal(-v[i]));
        }
    }

    sat = check() == z3::sat;
    z3::model res = get_model();
    solver.pop();
    return res;
}
