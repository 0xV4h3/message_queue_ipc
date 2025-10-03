#include "message_queue.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

bool parse_int(const std::string& s, int& value) {
    try {
        size_t idx;
        long v = std::stol(s, &idx, 0);
        if (idx != s.size() || v < 0) return false;
        value = static_cast<int>(v);
        return true;
    } catch (...) {
        return false;
    }
}

void print_usage() {
    std::cout << "Usage: message_rm <msqid> [msqid ...]\n"
              << "  <msqid>: message queue ID(s) to remove\n"
              << "  You can specify multiple IDs separated by spaces.\n";
}

int main(int argc, char* argv[]) {
    std::vector<int> msqids;

    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            int msqid = -1;
            if (!parse_int(argv[i], msqid)) {
                std::cerr << "Error: Invalid msqid value '" << argv[i] << "'.\n";
                print_usage();
                return 1;
            }
            msqids.push_back(msqid);
        }
    } else {
        std::cout << "Enter message queue ID(s) to remove (separated by spaces): ";
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            int msqid = -1;
            if (!parse_int(token, msqid)) {
                std::cerr << "Error: Invalid msqid value '" << token << "'.\n";
                return 1;
            }
            msqids.push_back(msqid);
        }
        if (msqids.empty()) {
            std::cerr << "No valid msqid provided.\n";
            return 1;
        }
    }

    bool any_failed = false;
    for (int msqid : msqids) {
        try {
            MessageQueue::remove(msqid);
            std::cout << "Message queue removed successfully!\n";
            std::cout << "  msqid : " << msqid << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Failed to remove message queue " << msqid << ": " << e.what() << "\n";
            any_failed = true;
        }
    }

    return any_failed ? 1 : 0;
}