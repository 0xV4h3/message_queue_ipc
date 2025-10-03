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

bool parse_size_t(const std::string& s, size_t& value) {
    try {
        size_t idx;
        value = std::stoull(s, &idx, 0);
        return idx == s.size();
    } catch (...) {
        return false;
    }
}

void print_usage() {
    std::cout << "Usage: message_chqbytes <msqid> <max_bytes>\n"
              << "  <msqid>    : message queue ID\n"
              << "  <max_bytes>: new maximum allowed bytes in queue\n";
}

int main(int argc, char* argv[]) {
    int msqid = -1;
    size_t max_bytes = 0;

    if (argc >= 3) {
        if (!parse_int(argv[1], msqid)) {
            std::cerr << "Error: Invalid msqid value.\n";
            print_usage();
            return 1;
        }
        if (!parse_size_t(argv[2], max_bytes) || max_bytes == 0) {
            std::cerr << "Error: Invalid max_bytes value.\n";
            print_usage();
            return 1;
        }
    } else {
        std::cout << "Enter message queue ID (msqid): ";
        std::string msqid_str;
        std::getline(std::cin, msqid_str);
        if (!parse_int(msqid_str, msqid)) {
            std::cerr << "Error: Invalid msqid value.\n";
            return 1;
        }

        std::cout << "Enter new max bytes for queue: ";
        std::string max_bytes_str;
        std::getline(std::cin, max_bytes_str);
        if (!parse_size_t(max_bytes_str, max_bytes) || max_bytes == 0) {
            std::cerr << "Error: Invalid max_bytes value.\n";
            return 1;
        }
    }

    try {
        MessageQueue::setMaxBytes(msqid, max_bytes);

        QueueInfo info = MessageQueue::getInfo(msqid);
        std::cout << "Queue max bytes changed successfully!\n";
        std::cout << "  msqid     : " << info.msqid << "\n";
        std::cout << "  max bytes : " << info.max_bytes << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed to change queue size: " << e.what() << "\n";
        return 1;
    }

    return 0;
}