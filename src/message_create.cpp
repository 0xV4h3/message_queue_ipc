#include "message_queue.hpp"

#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

bool parse_size_t(const std::string& s, size_t& value) {
    try {
        size_t idx;
        value = std::stoull(s, &idx, 0);
        return idx == s.size();
    } catch (...) {
        return false;
    }
}

bool parse_permissions(const std::string& s, unsigned short& perms) {
    try {
        size_t idx;
        perms = static_cast<unsigned short>(std::stoul(s, &idx, 8));
        return idx == s.size();
    } catch (...) {
        return false;
    }
}

bool parse_key_t(const std::string& s, key_t& key) {
    try {
        size_t idx;
        long k = std::stol(s, &idx, 0);
        if (idx != s.size() || k < 0) return false;
        key = static_cast<key_t>(k);
        return true;
    } catch (...) {
        return false;
    }
}

std::string get_username(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : std::to_string(uid);
}

std::string get_groupname(gid_t gid) {
    struct group *gr = getgrgid(gid);
    return gr ? gr->gr_name : std::to_string(gid);
}

void print_usage() {
    std::cout << "Usage: message_create <key> <max_bytes> [permissions]\n"
              << "  <key>         : integer key for the queue\n"
              << "  <max_bytes>   : maximum allowed bytes in queue\n"
              << "  [permissions] : optional; octal permissions (default: 0600)\n";
}

int main(int argc, char* argv[]) {
    key_t key = -1;
    size_t max_bytes = 0;
    unsigned short permissions = 0600;

    if (argc >= 3) {
        // -- key
        if (!parse_key_t(argv[1], key)) {
            std::cerr << "Error: Invalid key value.\n";
            print_usage();
            return 1;
        }
        // -- max_bytes
        if (!parse_size_t(argv[2], max_bytes) || max_bytes == 0) {
            std::cerr << "Error: Invalid max_bytes value.\n";
            print_usage();
            return 1;
        }
        // -- permissions (optional)
        if (argc >= 4) {
            if (!parse_permissions(argv[3], permissions)) {
                std::cerr << "Error: Invalid permissions value (should be octal, e.g. 0600).\n";
                print_usage();
                return 1;
            }
        }
    } else {
        std::cout << "Enter queue key (integer): ";
        std::string key_str;
        std::getline(std::cin, key_str);
        if (!parse_key_t(key_str, key)) {
            std::cerr << "Error: Invalid key value.\n";
            return 1;
        }

        std::cout << "Enter max bytes for queue: ";
        std::string max_bytes_str;
        std::getline(std::cin, max_bytes_str);
        if (!parse_size_t(max_bytes_str, max_bytes) || max_bytes == 0) {
            std::cerr << "Error: Invalid max_bytes value.\n";
            return 1;
        }

        std::cout << "Enter permissions (octal, default 0600, e.g. 600, 0600, 666): ";
        std::string perms_str;
        std::getline(std::cin, perms_str);
        if (!perms_str.empty()) {
            if (!parse_permissions(perms_str, permissions)) {
                std::cerr << "Error: Invalid permissions value.\n";
                return 1;
            }
        }
    }

    try {
        MessageQueue mq = MessageQueue::create(key, max_bytes, permissions);

        try {
            QueueInfo info = mq.getInfo();
            std::cout << "Message queue created successfully!\n";
            std::cout << "  msqid       : " << info.msqid << "\n";
            std::cout << "  key         : " << info.key << "\n";
            std::cout << "  owner       : " << get_username(info.owner_uid) << " (UID " << info.owner_uid << ")\n";
            std::cout << "  group       : " << get_groupname(info.owner_gid) << " (GID " << info.owner_gid << ")\n";
            std::cout << "  permissions : 0" << std::oct << info.permissions << std::dec << "\n";
            std::cout << "  max bytes   : " << info.max_bytes << "\n";
        } catch (const std::exception& e) {
            std::cout << "Message queue created successfully!\n";
            std::cout << "  msqid       : " << mq.getMsqid() << "\n";
            std::cout << "  key         : " << key << "\n";
            std::cout << "  permissions : 0" << std::oct << permissions << std::dec << "\n";
            std::cout << "  max bytes   : " << max_bytes << "\n";
            std::cout << "  Info unavailable due to permission restrictions.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to create message queue: " << e.what() << "\n";
        return 1;
    }

    return 0;
}