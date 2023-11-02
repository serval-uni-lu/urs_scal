#include<iostream>

#include<z3++.h>

z3::expr rm(z3::context & c, z3::model const& m) {
    z3::expr_vector v(c);

    for(std::size_t i = 0; i < m.size(); i++) {
        if(m.get_const_interp(m[i]).is_true()) {
            v.push_back(!m[i]());
        }
        else {
            v.push_back(m[i]());
        }
    }

    return mk_or(v);
}

void print_model(z3::model const& m) {
    for(std::size_t i = 0; i < m.size(); i++) {
        if(m.get_const_interp(m[i]).is_true()) {
            std::cout << m[i]() << " ";
        }
        else {
            std::cout << !m[i]() << " ";
        }
    }
    std::cout << '\n';
}

int main(int argc, char** argv) {
    z3::context context;
    z3::solver solver(context);

    solver.from_file(argv[2]);
    const std::size_t max = std::stoll(argv[1]);

    std::size_t i;
    for(i = 0; i < max && z3::sat == solver.check(); i++) {
        auto m = solver.get_model();
        // std::cout << m << "\n";
        print_model(m);

        solver.add(rm(context, m));
    }

    std::cout << "c nb: " << i << "\n";

    return 0;
}

