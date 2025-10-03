#include "message_queue.hpp"

#include <iostream>
#include <string>
#include <limits>
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

bool parse_long(const std::string& s, long& value) {
    try {
        size_t idx;
        long v = std::stol(s, &idx, 0);
        if (idx != s.size() || v <= 0) return false;
        value = v;
        return true;
    } catch (...) {
        return false;
    }
}

void print_usage() {
    std::cout << "Usage: message_receive <msqid> <type> [--nowait|-n]\n"
              << "  <msqid>: message queue ID\n"
              << "  <type> : message type (positive integer)\n"
              << "  [--nowait|-n] : optional; do not block if no message is present\n";
}

int main(int argc, char* argv[]) {
    int msqid = -1;
    long type = 0;
    bool nowait = false;

    if (argc >= 3) {
        // -- msqid
        if (!parse_int(argv[1], msqid)) {
            std::cerr << "Error: Invalid msqid value.\n";
            print_usage();
            return 1;
        }
        // -- type
        if (!parse_long(argv[2], type)) {
            std::cerr << "Error: Invalid type value.\n";
            print_usage();
            return 1;
        }
        // -- nowait flag (optional)
        if (argc >= 4) {
            if (std::string(argv[3]) == "--nowait" || std::string(argv[3]) == "-n")
                nowait = true;
        }
    } else {
        std::cout << "Enter message queue ID (msqid): ";
        std::string msqid_str;
        std::getline(std::cin, msqid_str);
        if (!parse_int(msqid_str, msqid)) {
            std::cerr << "Error: Invalid msqid value.\n";
            return 1;
        }

        std::cout << "Enter message type (positive integer): ";
        std::string type_str;
        std::getline(std::cin, type_str);
        if (!parse_long(type_str, type)) {
            std::cerr << "Error: Invalid type value.\n";
            return 1;
        }

        std::cout << "Non-blocking mode? (y/N): ";
        std::string nowait_str;
        std::getline(std::cin, nowait_str);
        nowait = !nowait_str.empty() && (nowait_str[0] == 'y' || nowait_str[0] == 'Y');
    }

    try {
        std::string received = MessageQueue::receiveMessage(msqid, type, nowait);

        std::cout << "Message received successfully!\n";
        std::cout << "  msqid          : " << msqid << "\n";
        std::cout << "  type           : " << type << "\n";
        std::cout << "  bytes received : " << received.size() << "\n";
        std::cout << "  message        : " << received << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed to receive message: " << e.what() << "\n";
        return 1;
    }

    return 0;
}