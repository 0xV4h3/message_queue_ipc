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

// Print usage
void print_usage() {
    std::cout << "Usage: message_send <msqid> <type> [message]\n"
              << "  <msqid>  : message queue ID\n"
              << "  <type>   : message type (positive integer)\n"
              << "  [message]: optional; message content (if omitted, will prompt)\n";
}

int main(int argc, char* argv[]) {
    int msqid = -1;
    long type = 0;
    std::string message;

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
        // -- message (optional)
        if (argc >= 4) {
            message = argv[3];
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
    }

    if (message.empty()) {
        std::cout << "Enter message content: ";
        std::getline(std::cin, message);
    }

    if (message.empty()) {
        std::cerr << "Error: Message cannot be empty.\n";
        return 1;
    }

    try {
         QueueInfo info = MessageQueue::getInfo(msqid);
        if (message.size() > info.max_bytes) {
            std::cerr << "Error: Message size (" << message.size()
                      << ") exceeds queue max_bytes (" << info.max_bytes << ").\n";
            return 1;
        }

        MessageQueue::sendMessage(msqid, type, message);

        std::cout << "Message sent successfully!\n";
        std::cout << "  msqid      : " << info.msqid << "\n";
        std::cout << "  type       : " << type << "\n";
        std::cout << "  bytes sent : " << message.size() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed to send message: " << e.what() << "\n";
        return 1;
    }

    return 0;
}