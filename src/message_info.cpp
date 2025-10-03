#include "message_queue.hpp"

#include <iostream>
#include <string>
#include <pwd.h>
#include <grp.h>

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

std::string format_time(time_t t) {
    if (t == 0) return "N/A";
    char buf[64];
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
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
    std::cout << "Usage: message_info <msqid>\n"
              << "  <msqid>: message queue ID to show info\n";
}

int main(int argc, char* argv[]) {
    int msqid = -1;

    if (argc >= 2) {
        if (!parse_int(argv[1], msqid)) {
            std::cerr << "Error: Invalid msqid value.\n";
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
    }

    try {
        QueueInfo info = MessageQueue::getInfo(msqid);

        std::cout << "Queue Info:\n";
        std::cout << "  msqid         : " << info.msqid << "\n";
        std::cout << "  key           : " << info.key << "\n";
        std::cout << "  owner         : " << get_username(info.owner_uid) << " (UID " << info.owner_uid << ")\n";
        std::cout << "  group         : " << get_groupname(info.owner_gid) << " (GID " << info.owner_gid << ")\n";
        std::cout << "  permissions   : 0" << std::oct << info.permissions << std::dec << "\n";
        std::cout << "  max bytes     : " << info.max_bytes << "\n";
        std::cout << "  used bytes    : " << info.used_bytes << "\n";
        std::cout << "  num messages  : " << info.num_messages << "\n";
        std::cout << "  last send     : " << format_time(info.last_send_time) << "\n";
        std::cout << "  last receive  : " << format_time(info.last_recv_time) << "\n";
        std::cout << "  last change   : " << format_time(info.last_change_time) << "\n";
    } catch (const std::exception& e) {
        std::cout << "Queue Info unavailable: " << e.what() << "\n";
        std::cout << "  msqid         : " << msqid << "\n";
    }

    return 0;
}