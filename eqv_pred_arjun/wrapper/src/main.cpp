#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<chrono>
#include<sstream>

#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>

std::size_t get_mem_usage(pid_t p) {
    std::string path = "/proc/" + std::to_string(p) + "/stat";
    std::ifstream in(path);

    if(in) {
        std::size_t r;
        std::string line;
        for(int i = 1; i < 23 && std::getline(in, line, ' '); i++);
        in >> r;
        return r / 1000;
    }
    return 0;
}

std::vector<char*> get_array(int argc, char** argv) {
    std::vector<char*> r;

    for(int i = 3; i < argc; i++) {
        r.push_back(argv[i]);
    }
    r.push_back(NULL);

    return r;
}

std::string build_cmd(int argc, char** argv) {
    std::stringstream r;

    for(int i = 3; i < argc; i++) {
        r << argv[i];
        if(i + 1 < argc) {
            r << " ";
        }
    }

    return r.str();
}

/**
 * wrap <Virt mem in MB> <time in s> porgram + args
 */
int main(int argc, char** argv) {
    pid_t pid = fork();

    std::size_t max_mem = std::stoll(argv[1]) * 1000;
    std::size_t max_time = std::stoll(argv[2]);

    auto ts = std::chrono::steady_clock::now();

    if(pid == -1) {
        std::cerr << "fork failed\n";
        return 1;
    }

    if(pid == 0) {
        auto vec = get_array(argc, argv);
        int r = execv(argv[3], vec.data());

        std::cerr << "execv failed\n";
    }
    else {
        std::size_t mem = get_mem_usage(pid);

        int wstatus;
        pid_t w;
        do {
            mem = std::max(mem, get_mem_usage(pid));
            w = waitpid(pid, &wstatus, WNOHANG);

            std::chrono::duration<double> dur = std::chrono::steady_clock::now() - ts;

            if(w == pid) {
                if(WIFEXITED(wstatus))
                    std::cerr << build_cmd(argc, argv) << ", done, " << mem << ", " << dur.count() << "\n";
                else
                    std::cerr << build_cmd(argc, argv) << ", err, " << mem << ", " << dur.count() << "\n";
                return 0;
            }
            if(mem > max_mem) {
                kill(pid, SIGINT);
                w = waitpid(pid, &wstatus, 0);

                if(w == pid) {
                    std::cerr << build_cmd(argc, argv) << ", mem, " << mem << ", " << dur.count() << "\n";
                    return 0;
                }
                else {
                    std::cerr << build_cmd(argc, argv) << ", mem err, " << mem << ", " << dur.count() << "\n";
                    return 1;
                }
            }
            if(dur.count() > max_time) {
                kill(pid, SIGINT);
                w = waitpid(pid, &wstatus, 0);

                if(w == pid) {
                    std::cerr << build_cmd(argc, argv) << ", timeout, " << mem << ", " << dur.count() << "\n";
                    return 0;
                }
                else {
                    std::cerr << build_cmd(argc, argv) << ", timeout err, " << mem << ", " << dur.count() << "\n";
                    return 1;
                }
            }

            usleep(1000); // 0.01s
        } while(true);
    }
    return 0;
}
