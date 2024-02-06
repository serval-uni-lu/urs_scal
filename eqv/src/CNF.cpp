//
// Created by oz on 1/12/22.
//

#include "CNF.hpp"
#include "UF.hpp"

#include<fstream>
#include<iostream>
#include<sstream>
#include<algorithm>

bool is_unit(Clause const& c) {
    return c.size() == 1;
}

bool is_empty(Clause const& c) {
    return c.empty();
}

CNF::CNF(char const* path) {
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

std::map<Literal, int> CNF::count_var_occurrences() const {
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

Literal CNF::find_most_occurring_variable() const {
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

std::map<Literal, int> CNF::count_literal_occurrences() const {
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

Literal CNF::find_most_occurring_literal() const {
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

    //vars.erase(std::abs(backbone_lit));

    cnf.erase(it, cnf.end());
    units.insert(backbone_lit);

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
            units.insert(l);
            bool tt = simplify(l);
            res = res || tt;
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

Literal CNF::get_sign_of_const(Literal l) const {
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
                for(Literal l : i) {
                    tmp.vars.insert(std::abs(l));
                }
                return true;
            }
            return false;
        });
        clauses.erase(it, clauses.end());

        res.push_back(tmp);
    }

    return res;
}

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
    std::set<Clause> tmp(cnf.begin(), cnf.end());
    out << "p cnf " << cnf.get_nb_vars() << " " << (tmp.size() + cnf.get_nb_units()) << "\n";

    for(Clause const& c : tmp) {
        for(Literal l : c) {
            out << l << " ";
        }
        out << "0\n";
    }

    for(Literal l : cnf.units) {
        out << l << " 0\n";
    }

    for(Literal l : cnf.free) {
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

std::set<Literal> get_conflicting_variables(Clause const& c1, Clause const& c2) {
    std::set<Literal> res;

    for(Literal l1 : c1) {
        for(Literal l2 : c2) {
            if(l1 == -l2) {
                res.insert(l1);
            }
        }
    }

    return res;
}

bool is_trivial(Clause const& c) {
    for(Literal l1 : c) {
        for(Literal l2 : c) {
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

void print_cl_sub(Clause const& lh, Clause const& rh) {
    for(Literal l : lh) {
        std::cout << l << " ";
    }
    std::cout << "0 -> ";
    for(Literal l : rh) {
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

void CNF::propagate(std::vector<int> const& backbone) {
#pragma omp parallel for
    for(std::size_t i = 0; i < cnf.size(); i++) {
        for(std::size_t j = 0; j < backbone.size(); j++) {
            cnf[i].erase(-backbone[j]);
        }
    }

    auto it = std::remove_if(cnf.begin(), cnf.end(), [&](auto const& c) {
            for(int l : c) {
                int i = std::abs(l) - 1;
                if(l == backbone[i]) {
                    return true;
                }
            }
            return false;
    });

    cnf.erase(it, cnf.end());
}
