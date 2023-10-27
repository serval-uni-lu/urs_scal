//
// Created by oz on 1/12/22.
//

#include "cnf.hpp"
#include "uf.hpp"

#include<fstream>
#include<iostream>
#include<sstream>
#include<algorithm>
#include<random>

bool is_unit(CNF::Clause const& c) {
    return c.size() == 1;
}

bool is_empty(CNF::Clause const& c) {
    return c.empty();
}

bool is_trivially_true(CNF::Clause const& c) {
    return std::any_of(c.begin(), c.end(), [&](CNF::Literal const& l){
        return c.find(-l) != c.end();
    });
}

CNF::CNF(std::string const& path) {
    std::ifstream f(path);

    if(!f) {
        std::cerr << "error opening file " << path << "\n";
        return;
    }

    int nb_vars;
    std::string line;
    while(getline(f, line)) {
        if(line.rfind("p cnf ", 0) == 0) {
            std::stringstream iss(line);
            std::string tmp;
            iss >> tmp >> tmp;
            iss >> nb_vars;

            for(int i = 1; i <= nb_vars; i++) {
                vars.insert(i);
            }
        }
        else if(line.rfind("c ind ", 0) == 0) {
            std::stringstream iss(line);
            std::string tmp;

            iss >> tmp >> tmp;
            while(iss) {
                int v;
                iss >> v;
                if(v != 0) {
                    ind.insert(v);
                }
            }
        }
        else if(line[0] != 'c' && line[0] != 'p') {
            Clause clause;
            std::stringstream iss(line);

            while(iss) {
                int v;
                iss >> v;
                if(v != 0) {
                    clause.insert(v);
                }
            }

            cnf.push_back(clause);
        }
    }

    //simplify();
    compute_free_vars();
}

std::size_t CNF::get_nb_vars() const {
    return vars.size();
}

std::size_t CNF::get_nb_clauses() const {
    return cnf.size();
}

std::size_t CNF::get_nb_units() const {
    return units.size();
}

std::size_t CNF::get_nb_binary() const {
    std::size_t nb = 0;
    for(Clause const& c : cnf) {
        if(c.size() == 2) {
            nb++;
        }
    }

    return nb;
}

bool CNF::unsat() const {
    return std::any_of(cnf.begin(), cnf.end(), is_empty);
}

std::map<CNF::Literal, int> CNF::count_var_occurrences() const {
    std::map<Literal, int> res;

    for(Clause const& c : cnf) {
        for(Literal l : c) {
            l = std::abs(l);

            auto it = res.find(l);
            if(it != res.end()) {
                it->second++;
            }
            else {
                res.insert({l, 1});
            }
        }
    }

    return res;
}

CNF::Literal CNF::find_most_occurring_variable() const {
    std::map<Literal, int> map = count_var_occurrences();

    Literal l = 0;
    int nb = -1;

    for(auto const& i : map) {
        if(nb < i.second) {
            nb = i.second;
            l = i.first;
        }
    }

    return l;
}

std::map<CNF::Literal, int> CNF::count_ind_occurrences() const {
    std::map<Literal, int> res;

    for(int i : ind) {
        res[i] = 0;
    }

    for(Clause const& c : cnf) {
        for(Literal l : c) {
            l = std::abs(l);

            auto it = res.find(l);
            if(it != res.end()) {
                it->second++;
            }
        }
    }

    return res;
}

CNF::Literal CNF::find_most_occurring_ind() const {
    std::map<Literal, int> map = count_ind_occurrences();

    Literal l = 0;
    int nb = -1;

    for(auto const& i : map) {
        if(nb < i.second) {
            nb = i.second;
            l = i.first;
        }
    }

    return l;
}

std::map<CNF::Literal, int> CNF::count_literal_occurrences() const {
    std::map<Literal, int> res;

    for(Clause const& c : cnf) {
        for(Literal l : c) {
            auto it = res.find(l);
            if(it != res.end()) {
                it->second++;
            }
            else {
                res.insert({l, 1});
            }
        }
    }

    return res;
}

CNF::Literal CNF::find_most_occurring_literal() const {
    std::map<Literal, int> map = count_literal_occurrences();

    Literal l = 0;
    int nb = -1;

    for(auto const& i : map) {
        if(nb < i.second) {
            nb = i.second;
            l = i.first;
        }
    }

    return l;
}

std::map<CNF::Literal, int> CNF::count_ind_literal_occurrences() const {
    std::map<Literal, int> res;

    for(int i : ind) {
        res[i] = 0;
        res[-i] = 0;
    }

    for(Clause const& c : cnf) {
        for(Literal l : c) {
            auto it = res.find(l);
            if(it != res.end()) {
                it->second++;
            }
        }
    }

    return res;
}

CNF::Literal CNF::find_most_occurring_ind_literal() const {
    std::map<Literal, int> map = count_ind_literal_occurrences();

    Literal l = 0;
    int nb = -1;

    for(auto const& i : map) {
        if(nb < i.second) {
            nb = i.second;
            l = i.first;
        }
    }

    return l;
}

void CNF::compute_free_vars() {
    free.clear();
    free = vars;

    for(Clause const& c : cnf) {
        for(Literal l : c) {
            free.erase(std::abs(l));
        }
    }

    for(Literal l : units) {
        free.erase(std::abs(l));
    }
}

bool CNF::simplify(Literal backbone_lit) {
    bool res = false;
    auto it = std::remove_if(cnf.begin(), cnf.end(), [&](Clause & c) {
        if(c.find(backbone_lit) != c.end()) {
            res = true;
            return true;
        }
        else {
            bool tmp = c.erase(-backbone_lit) >= 1;
            res = res || tmp;
            return false;
        }
    });

    cnf.erase(it, cnf.end());
    units.insert(backbone_lit);
    ind.erase(std::abs(backbone_lit));

    return res;
}

bool CNF::simplify() {
    bool res = false;
    std::vector<Literal> tmp;

    auto it = std::remove_if(cnf.begin(), cnf.end(), [&](Clause const& c) {
        if(is_unit(c)) {
            tmp.push_back(*(c.begin()));
            res = true;
            return true;
        }
        return false;
    });

    while (!tmp.empty()) {
        cnf.erase(it, cnf.end());
        for(Literal l : tmp) {
            if(units.find(-l) == units.end()) {
                units.insert(l);
                bool tt = simplify(l);
                res = res || tt;
            }
            else {
                cnf.push_back({});
            }
        }
        tmp.clear();
        it = std::remove_if(cnf.begin(), cnf.end(), [&](Clause const& c) {
            if(is_unit(c)) {
                tmp.push_back(*(c.begin()));
                res = true;
                return true;
            }
            return false;
        });
    }

    return res;
}

CNF CNF::reduce(std::vector<Literal> & map) const {
    map.clear();
    map.push_back(-1);

    for(Clause const& c : cnf) {
        for(Literal l : c) {
            l = std::abs(l);

            while(static_cast<int>(map.size()) <= l) {
                map.push_back(0);
            }
            map[l] = l;
        }
    }

    auto it = std::remove_if(map.begin(), map.end(), [](Literal l) {
        return l == 0;
    });
    map.erase(it, map.end());

    std::map<Literal, Literal> tmap;
    for(Literal i = 1; i < static_cast<int>(map.size()); i++) {
        tmap[map[i]] = i;
    }

    CNF res;

    for(auto const& c : cnf) {
        Clause tmp;
        for(Literal l : c) {
            Literal r = l > 0 ? tmap[l] : -tmap[std::abs(l)];
            tmp.insert(r);
            res.vars.insert(std::abs(r));
        }
        res.add_clause(tmp);
    }

    return res;
}

bool CNF::is_free(Literal l) const {
    return free.find(std::abs(l)) != free.end();
}

bool CNF::is_const(Literal l) const {
    return (units.find(l) != units.end()) || (units.find(-l) != units.end());
}

CNF::Literal CNF::get_sign_of_const(Literal l) const {
    auto it = units.find(l);
    if(it != units.end()) {
        return l;
    }
    it = units.find(-l);
    if(it != units.end()) {
        return -l;
    }
    return 0;
}

bool intersects(std::set<int> const& a, std::set<int> const& b) {
    return std::any_of(a.begin(), a.end(), [&](int i) {
        return b.find(i) != b.end();
    });
}

void merge(std::set<int> & a, std::set<int> const& b) {
    for(int i : b) {
        a.insert(i);
    }
}

std::vector<CNF> CNF::split() const {
    int max_var = 0;

    for(Clause const& c : cnf) {
        for(Literal l : c) {
            max_var = std::max(max_var, std::abs(l));
        }
    }

    UF uf(max_var + 1);

    for(Clause const& c : cnf) {
        for(Literal l1 : c) {
            for(Literal l2 : c) {
                uf.merge(std::abs(l1), std::abs(l2));
            }
        }
    }

    std::vector<Clause> clauses = cnf;
    std::vector<CNF> res;
    while(!clauses.empty()) {
        Clause c = clauses[clauses.size() - 1];
        clauses.pop_back();

        CNF tmp;
        tmp.cnf.push_back(c);

        auto it = std::remove_if(clauses.begin(), clauses.end(), [&](Clause i) {
            if(uf.connected(std::abs(*(c.begin())), std::abs(*(i.begin())))) {
                tmp.cnf.push_back(i);
                return true;
            }
            return false;
        });
        clauses.erase(it, clauses.end());

        for(Clause c : tmp) {
            for(int i : c) {
                i = std::abs(i);
                if (ind.find(i) != ind.end()) {
                    tmp.ind.insert(i);
                }
                tmp.vars.insert(i);
            }
        }

        res.push_back(tmp);
    }

    return res;
}
/*
std::vector<CNF> CNF::split() const {
    if(cnf.empty()) {
        return {};
    }
    int max_var = 0;

    for(Clause const& c : cnf) {
        for(Literal l : c) {
            max_var = std::max(max_var, std::abs(l));
        }
    }

    UF uf(max_var + 1);
    UF uf_clauses(get_nb_clauses());

    for(Clause const& c : cnf) {
        for(Literal l1 : c) {
            for(Literal l2 : c) {
                uf.merge(std::abs(l1), std::abs(l2));
            }
        }
    }

    for(std::size_t i = 0; i < cnf.size() - 1; i++) {
        int v1 = std::abs(*cnf[i].begin());
        for(std::size_t j = i + 1; j < cnf.size(); j++) {
            int v2 = std::abs(*cnf[j].begin());
            if(uf.connected(v1, v2)) {
                uf_clauses.merge(i, j);
            }
        }
    }

    std::map<int, CNF> map;
    for(std::size_t i = 0; i < cnf.size(); i++) {
        int id = uf_clauses.find(i);
        auto it = map.find(id);

        if(it == map.end()) {
            map[id] = CNF();
            it = map.find(id);
        }
        it->second.cnf.push_back(cnf[i]);
        for(Literal l : cnf[i]) {
            it->second.vars.insert(std::abs(l));
        }
    }

    std::vector<CNF> res;

    for(auto & e : map) {
        //if(!e.second.cnf.empty()) {
            e.second.ind = ind;
            res.push_back(e.second);
        //}
    }

    return res;
}*/

void CNF::merge(CNF const& c) {
    for(Clause const& cl : c.cnf) {
        cnf.push_back(cl);
    }

    for(Literal l : c.units) {
        units.insert(l);
    }

    for(Literal l : c.free) {
        free.insert(l);
    }

    for(Literal l : c.vars) {
        vars.insert(l);
    }
}

std::ostream & operator<<(std::ostream & out, CNF const& cnf) {
    out << "p cnf " << cnf.get_nb_vars() << " " << (cnf.get_nb_clauses() + cnf.get_nb_units()) << "\n";

    for(CNF::Clause const& c : cnf) {
        for(CNF::Literal l : c) {
            out << l << " ";
        }
        out << "0\n";
    }

    out << "\n";

    for(CNF::Literal l : cnf.units) {
        out << l << " 0\n";
    }

    out << "\n";

    for(CNF::Literal l : cnf.free) {
        out << "c free " << l << "\n";
    }

    return out;
}

bool CNF::simplify_binary() {
    bool res = false;

    for(int i = 1; i <= static_cast<int>(get_nb_vars()) - 1; i++) {
        //std::cout << i << std::endl;
        for(int j = i + 1; j <= static_cast<int>(get_nb_vars()); j++) {
            std::set<Clause> binary;

            auto it = std::remove_if(cnf.begin(), cnf.end(), [&](Clause c) {
                if (c.size() == 2 && (c.find(i) != c.end() || c.find(-i) != c.end()) && (c.find(j) != c.end() || c.find(-j) != c.end())) {
                    binary.insert(c);
                    return true;
                }
                return false;
            });
            cnf.erase(it, cnf.end());

            if(binary.size() == 1) {
                //nothing to do
                for(Clause const& c : binary) {
                    cnf.push_back(c);
                }
            }
            else if(binary.size() == 2) {
                std::map<Literal, int> occ;
                occ[i] = occ[-i] = occ[j] = occ[-j] = 0;
                for(Clause const& c : binary) {
                    for(Literal l : c) {
                        occ[l]++;
                    }
                }

                if(occ[i] == 2) {
                    res = res || simplify(i);
                }
                else if(occ[-1] == 2) {
                    res = res || simplify(-i);
                }
                else if(occ[j] == 2) {
                    res = res || simplify(j);
                }
                else if(occ[-j] == 2) {
                    res = res || simplify(-j);
                }
                else {
                    Clause t = {i, j};
                    if(binary.find(t) != binary.end()) {
                        // (i xor j) <=> (-i <=> j)
                        // replace j by -i
                        res = res || replace(j, -i);
                    }
                    else {
                        // i <=> j
                        // replace j by i
                        res = res || replace(j, i);
                    }

                    for(Clause const& c : binary) {
                        cnf.push_back(c);
                    }
                }
            }
            else if(binary.size() == 3) {
                res = true;

                std::map<Literal, int> occ;
                occ[i] = occ[-i] = occ[j] = occ[-j] = 0;
                for(Clause const& c : binary) {
                    for(Literal l : c) {
                        occ[l]++;
                    }
                }

                Literal l1 = occ[i] > occ[-i] ? i : -i;
                Literal l2 = occ[j] > occ[-j] ? j : -j;

                res = res || simplify(l1);
                res = res || simplify(l2);
            }
        }
    }
    return res;
}

bool CNF::replace(Literal s, Literal r) {
    bool res = false;
    for(Clause & c : cnf) {
        if(c.find(s) != c.end()) {
            res = true;
            c.erase(s);
            c.insert(r);
        }
        else if(c.find(-s) != c.end()) {
            res = true;
            c.erase(-s);
            c.insert(-r);
        }
    }
    return res;
}

void CNF::add_clause(Clause const& c) {
    cnf.push_back(c);
}

std::set<CNF::Literal> get_conflicting_variables(CNF::Clause const& c1, CNF::Clause const& c2) {
    std::set<CNF::Literal> res;

    for(CNF::Literal l1 : c1) {
        for(CNF::Literal l2 : c2) {
            if(l1 == -l2) {
                res.insert(l1);
            }
        }
    }

    return res;
}

bool is_trivial(CNF::Clause const& c) {
    for(CNF::Literal l1 : c) {
        for(CNF::Literal l2 : c) {
            if(l1 == -l2) {
                return true;
            }
        }
    }
    return false;
}

void CNF::resolution_extend() {
    for(std::size_t i = 0; i < cnf.size() - 1; i++) {
        std::cout << i << " / " << cnf.size() << std::endl;
        for(std::size_t j = i + 1; j < cnf.size(); j++) {
            auto conflict = get_conflicting_variables(cnf[i], cnf[j]);

            if(!conflict.empty()) {
                Clause tmp = cnf[i];
                tmp.insert(cnf[j].begin(), cnf[j].end());
                for (Literal l: conflict) {
                    tmp.erase(l);
                    tmp.erase(-l);
                    if(!is_trivial(tmp)) {
                        cnf.push_back(tmp);
                    }
                }

                subsumption();
            }
        }
    }
}

bool is_strict_subset(std::set<int> const& a, std::set<int> const& b) {
    return a.size() < b.size() && std::all_of(a.begin(), a.end(), [&](int i) {
        return b.find(i) != b.end();
    });
}

void print_cl_sub(CNF::Clause const& lh, CNF::Clause const& rh) {
    for(CNF::Literal l : lh) {
        std::cout << l << " ";
    }
    std::cout << "0 -> ";
    for(CNF::Literal l : rh) {
        std::cout << l << " ";
    }
    std::cout << "0\n";
}

void CNF::subsumption() {
    auto it = std::remove_if(cnf.begin(), cnf.end(), [&](Clause const& a) {
        return std::any_of(cnf.begin(), cnf.end(), [&](Clause const& b) {
            bool r = !b.empty() && is_strict_subset(b, a);
            if(r) {
                print_cl_sub(b, a);
            }
            return r;
        });
    });
    cnf.erase(it, cnf.end());
}

CNF::Literal CNF::random_ind() const {
    std::random_device rd;
    std::uniform_int_distribution<int> ud(0,static_cast<int>(ind.size()) - 1);
    int pos = ud(rd);

    for(int i : ind) {
        if(pos == 0) {
            return i;
        }
        pos--;
    }
    return -1;
}

CNF CNF::forget(CNF::Literal l) const {
    if(units.find(l) != units.end()) {
        CNF res(*this);
        res.simplify(l);
        res.simplify();
        res.units.erase(l);
        res.vars.erase(l);
        return res;
    }
    else if(units.find(-l) != units.end()) {
        CNF res(*this);
        res.simplify(-l);
        res.simplify();
        res.units.erase(-l);
        res.vars.erase(l);
        return res;
    }

    CNF t(*this);
    CNF f(*this);

    t.simplify(l);
    t.simplify();
    t.units.erase(l);

    f.simplify(-l);
    f.simplify();
    f.units.erase(-l);

    CNF res(*this);
    res.cnf.clear();
    res.units.clear();
    res.free.erase(l);
    res.vars.erase(l);
    res.ind.erase(l);

    std::set<Clause> clauses;
    for(Clause const& ct : t) {
        for(Clause const& cf : f) {
            Clause tmp(ct);
            for(Literal const& lit : cf) {
                tmp.insert(lit);
            }

            if(!is_trivially_true(tmp)) {
                clauses.insert(tmp);
            }
        }

        for(Literal lit : f.units) {
            Clause tmp(ct);

            if(tmp.find(-lit) == tmp.end()) {
                tmp.insert(lit);
                clauses.insert(tmp);
            }
        }
    }

    for(Literal lit : t.units) {
        for(Clause const& cf : f) {
            Clause tmp(cf);
            if(tmp.find(-lit) == tmp.end()) {
                tmp.insert(lit);
                clauses.insert(tmp);
            }
        }

        for(Literal l2 : f.units) {
            if(lit != -l2) {
                Clause tmp;
                tmp.insert(l2);
                tmp.insert(lit);

                clauses.insert(tmp);
            }
        }
    }

    for(Clause const& c : clauses) {
        res.cnf.push_back(c);
    }

    res.simplify();

    return res;
}

std::vector<int> CNF::rename_vars() {
    std::vector<int> map;
    std::map<int, int> rmap;

    map.push_back(0);
    for(int i : vars) {
        map.push_back(i);
        int v = static_cast<int>(map.size()) - 1;
        rmap[i] = v;
        rmap[-i] = -v;
    }

    std::vector<Clause> tmp;
    for(Clause const& c : cnf) {
        Clause t;
        for(Literal l : c) {
            t.insert(rmap[l]);
        }
        tmp.push_back(t);
    }
    cnf = tmp;

    std::set<int> tu;
    for(Literal l : units) {
        tu.insert(rmap[l]);
    }
    units = tu;

    tu.clear();
    for(Literal l : free) {
        tu.insert(rmap[l]);
    }
    free = tu;

    tu.clear();
    for(int i : ind) {
        tu.insert(rmap[i]);
    }
    ind = tu;

    vars.clear();
    for(int i = 1; i < map.size(); i++) {
        vars.insert(i);
    }

    return map;
}

