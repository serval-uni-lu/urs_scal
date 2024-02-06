#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<utility>
#include<omp.h>

#include<ctime>

#include "CNF.hpp"
#include "UF.hpp"
#include "Z3CNF.hpp"

int sign(int x) {
    return x / std::abs(x);
}

struct EquivClasses {
    std::vector<int> set_ass;
    std::vector<int> initial_model;
    UF uf;
    int next_set;

    EquivClasses(std::vector<int> m) : set_ass(m.size(), 1), initial_model(m), uf(m.size()), next_set(2) {
    }

    void add_model(std::vector<int> const& m) {
        struct pair {
            int change;
            int nochange;

            pair() : change(-1), nochange(-1) {}
        };
        std::map<int, pair> change;

        for(std::size_t i = 0; i < m.size(); i++) {
            auto& elem = change[set_ass[i]];
            if(m[i] != initial_model[i]) {
                int s = elem.change;
                if(s == -1) {
                    if(elem.nochange == -1) {
                        s = elem.change = set_ass[i];
                    }
                    else {
                        s = elem.change = next_set++;
                    }
                }
                set_ass[i] = s;
            }
            else {
                int s = elem.nochange;
                if(s == -1) {
                    if(elem.change == -1) {
                        s = elem.nochange = set_ass[i];
                    }
                    else {
                        s = elem.nochange = next_set++;
                    }
                }
                set_ass[i] = s;
            }
        }
    }

    int nb_sets() {
        return uf.count();
    }

    int nb_preliminary_sets() const {
        return next_set - 1;
    }

    void join(int x, int y) {
        x = std::abs(x) - 1;
        y = std::abs(y) - 1;
        uf.merge(x, y);
    }

    bool is_verified(int x, int y) {
        x = std::abs(x) - 1;
        y = std::abs(y) - 1;
        return uf.connected(x, y);
    }

    int preliminary_set(int x) const {
        x = std::abs(x) - 1;
        return set_ass[x];
    }

    int verified_set(int x) {
        x = std::abs(x) - 1;
        return uf.find(x);
    }

    int literal(int x) const {
        x = std::abs(x) - 1;
        return initial_model[x];
    }
};

std::ostream& operator<<(std::ostream & out, EquivClasses const& eqc) {
    for(int i = 1; i < eqc.next_set; i++) {
        out << i << ": ";
        for(int j = 0; j < eqc.initial_model.size(); j++) {
            if(eqc.set_ass[j] == i) {
                out << eqc.initial_model[j] << " ";
            }
        }
        out << "\n";
    }

    return out;
}

CNF apply_eqv(EquivClasses const& eqv, CNF const& cnf, int cnst) {
    CNF res;

    for(int i = 1; i <= eqv.nb_preliminary_sets(); i++) {
        int l = res.add_var();
        std::cout << "add var " << l << "\n";
    }

    std::vector<std::set<int> > cls;
    for(Clause const& c : cnf) {
        std::set<int> tmp;
        bool trivial = false;

        for(int l : c) {
            int se = eqv.preliminary_set(l);
            int li = eqv.literal(l);

            int rl = se * sign(l) * sign(li);

            trivial = trivial || tmp.find(-rl) != tmp.end();
            if(trivial) {
                break;
            }

            tmp.insert(rl);
        }

        if(!trivial) {
            cls.push_back(tmp);
        }
    }

    for(Clause const& c : cls) {
        res.add_clause(c);
    }

    cnst = eqv.preliminary_set(cnst);
    res.simplify(cnst);

    return res;
}

bool is_subset(std::set<int> const& lhs, std::set<int> const& rhs) {
    for(int i : lhs) {
        if(rhs.find(i) == rhs.end()) {
            return false;
        }
    }
    return true;
}

CNF par_apply_eqv(EquivClasses const& eqv, CNF const& cnf, int cnst) {
    CNF res;
    cnst = eqv.preliminary_set(cnst);

    for(int i = 1; i <= eqv.nb_preliminary_sets(); i++) {
        int l = res.add_var();
    }

    std::vector<std::set<int> > cls(cnf.get_nb_clauses());
    std::vector<bool> rmv(cnf.get_nb_clauses(), false);
    std::vector<bool> sub(cnf.get_nb_clauses(), false);

//#pragma omp parallel for
    for(std::size_t i = 0; i < cnf.get_nb_clauses(); i++) {
        bool trivial = false;

        for(int l : cnf[i]) {
            int se = eqv.preliminary_set(l);
            int li = eqv.literal(l);

            int rl = se * sign(l) * sign(li);

            trivial = trivial || rl == cnst || cls[i].find(-rl) != cls[i].end();
            if(trivial) {
                rmv[i] = true;
                break;
            }

            if(rl != -cnst)
                cls[i].insert(rl);
        }
    }

#pragma omp parallel for collapse(2)
    for(std::size_t i = 0; i < cnf.get_nb_clauses(); i++) {
        for(int j = 0; j < cnf.get_nb_clauses(); j++) {
            bool e = i != j && !rmv[i] && !rmv[j];
            e = e && !sub[i];
            if(e && is_subset(cls[j], cls[i])) {
                sub[i] = true;
            }
        }
    }


    for(std::size_t i = 0; i < cls.size(); i++) {
        if(!rmv[i]) {
            res.add_clause(cls[i]);
        }
    }
    res.add_clause({cnst});

    return res;
}

int main(int argc, char const** argv) {
    std::string const path(argv[1]);
    CNF cnf(path.c_str());
    int const cnst = cnf.add_var();
    cnf.add_clause({cnst});
    Z3CNF z3cnf(cnf);

    z3cnf.check();
    EquivClasses eqv(model_to_set(z3cnf, z3cnf.get_model()));

    std::vector<Z3CNF> z3cnf_l;
    z3cnf_l.reserve(omp_get_max_threads());
    for(int i = 0; i < omp_get_max_threads(); i++) {
        z3cnf_l.emplace_back(cnf);
    }
    std::cout << "c nb threads: " << z3cnf_l.size() << "\n";

    int sat_calls = 0;

// #pragma omp parallel for collapse(2) schedule(dynamic, 100)
#pragma omp parallel for collapse(2)
    for(int i = 1; i <= cnf.get_nb_vars(); i++) {
        for(int j = 1; j <= cnf.get_nb_vars(); j++) {
            if(i < j) {
                bool e;
#pragma omp critical
                e = !eqv.is_verified(i, j) && eqv.preliminary_set(i) == eqv.preliminary_set(j);
                if(e) {
                    int li = eqv.literal(i);
                    int lj = eqv.literal(j);

                    Z3CNF& tmp = z3cnf_l[omp_get_thread_num()];

                    tmp.push();
                    tmp.add_clause({li, lj});
                    tmp.add_clause({-lj, -li});

#pragma omp atomic
                    sat_calls++;
                    if(tmp.check() == z3::sat) {
                        auto tm = model_to_set(tmp, tmp.get_model());
#pragma omp critical
                        eqv.add_model(tm);
                    }
                    else {
                        // li <=> lj
#pragma omp critical
                        eqv.join(i, j);
                    }
                    
                    tmp.pop();
                }
            }
        }
    }

    std::cout << eqv;

    std::cout << "c nb eqv classes: " << eqv.nb_sets() << "\n";
    std::cout << "c nb vars: " << cnf.get_nb_vars() << "\n";
    std::cout << "c nb sat calls: " << sat_calls << "\n\n";


    auto r = par_apply_eqv(eqv, cnf, cnst);
    std::ofstream out(path + ".eqv");
    out << r;

    return 0;
}
