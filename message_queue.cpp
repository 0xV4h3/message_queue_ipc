#include "message_queue.hpp"

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <algorithm>

// Internal message buffer struct (for System V)
struct MsgBuffer {
    long mtype;
    char mtext[4096];
};

// --- Static section ---
MessageQueue MessageQueue::create(key_t key, size_t max_bytes, unsigned short permissions) {
    int msqid = msgget(key, IPC_CREAT | IPC_EXCL | permissions);
    if (msqid == -1) {
        throw std::runtime_error("Failed to create message queue: " + std::string(strerror(errno)));
    }
    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        throw std::runtime_error("Failed to get queue info after creation: " + std::string(strerror(errno)));
    }
    buf.msg_qbytes = max_bytes;
    if (msgctl(msqid, IPC_SET, &buf) == -1) {
        throw std::runtime_error("Failed to set queue max bytes: " + std::string(strerror(errno)));
    }
    return MessageQueue(msqid);
}

MessageQueue MessageQueue::attach(int msqid) {
    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        throw std::runtime_error("Failed to attach to message queue: " + std::string(strerror(errno)));
    }
    return MessageQueue(msqid);
}

void MessageQueue::remove(int msqid) {
    if (msgctl(msqid, IPC_RMID, nullptr) == -1) {
        throw std::runtime_error("Failed to remove queue: " + std::string(strerror(errno)));
    }
}

void MessageQueue::sendMessage(int msqid, long type, const std::string &message) {
    if (type <= 0) throw std::invalid_argument("Message type must be positive");
    if (message.empty()) throw std::invalid_argument("Message cannot be empty");

    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        throw std::runtime_error("Failed to get queue info before sending: " + std::string(strerror(errno)));
    }
    size_t max_bytes = buf.msg_qbytes;
    size_t msg_len = message.size();
    if (msg_len > max_bytes) {
        throw std::length_error("Message length exceeds queue maximum (" + std::to_string(max_bytes) + ")");
    }
    if (msg_len > sizeof(MsgBuffer::mtext)) {
        throw std::length_error("Message length exceeds max buffer size (" + std::to_string(sizeof(MsgBuffer::mtext)) + ")");
    }

    MsgBuffer bufmsg;
    bufmsg.mtype = type;
    std::memset(bufmsg.mtext, 0, sizeof(bufmsg.mtext));
    std::memcpy(bufmsg.mtext, message.data(), msg_len);

    if (msgsnd(msqid, &bufmsg, msg_len, IPC_NOWAIT) == -1) {
        throw std::runtime_error("Failed to send message: " + std::string(strerror(errno)));
    }
}

std::string MessageQueue::receiveMessage(int msqid, long type, bool nowait) {
    if (type <= 0) throw std::invalid_argument("Message type must be positive");

    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        throw std::runtime_error("Failed to get queue info before receiving: " + std::string(strerror(errno)));
    }
    size_t max_bytes = buf.msg_qbytes;
    size_t bufsize = std::min(max_bytes, sizeof(MsgBuffer::mtext));

    MsgBuffer bufmsg;
    std::memset(&bufmsg, 0, sizeof(bufmsg));

    int flags = nowait ? IPC_NOWAIT : 0;
    ssize_t received = msgrcv(msqid, &bufmsg, bufsize, type, flags);
    if (received == -1) {
        if (errno == ENOMSG && nowait)
            throw std::runtime_error("No message of the requested type in the queue.");
        throw std::runtime_error("Failed to receive message: " + std::string(strerror(errno)));
    }
    return std::string(bufmsg.mtext, received);
}

void MessageQueue::setMaxBytes(int msqid, size_t max_bytes) {
    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        throw std::runtime_error("Failed to get queue info for setMaxBytes: " + std::string(strerror(errno)));
    }
    buf.msg_qbytes = max_bytes;
    if (msgctl(msqid, IPC_SET, &buf) == -1) {
        throw std::runtime_error("Failed to set queue max bytes: " + std::string(strerror(errno)));
    }
}

QueueInfo MessageQueue::getInfo(int msqid) {
    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        throw std::runtime_error("Failed to get queue info: " + std::string(strerror(errno)));
    }

    QueueInfo info;
    info.msqid = msqid;
    info.key = buf.msg_perm.__key;
    info.owner_uid = buf.msg_perm.uid;
    info.owner_gid = buf.msg_perm.gid;
    info.permissions = buf.msg_perm.mode & 0777;
    info.max_bytes = buf.msg_qbytes;
    info.used_bytes = buf.msg_cbytes;
    info.num_messages = buf.msg_qnum;
    info.last_send_time = buf.msg_stime;
    info.last_recv_time = buf.msg_rtime;
    info.last_change_time = buf.msg_ctime;
    return info;
}

// --- Non-static (object) section ---
MessageQueue::MessageQueue(int msqid)
    : msqid_(msqid)
{}

void MessageQueue::sendMessage(long type, const std::string &message) {
    sendMessage(msqid_, type, message);
}

std::string MessageQueue::receiveMessage(long type, bool nowait) {
    return receiveMessage(msqid_, type, nowait);
}

void MessageQueue::setMaxBytes(size_t max_bytes) {
    setMaxBytes(msqid_, max_bytes);
}

void MessageQueue::remove() {
    remove(msqid_);
}

QueueInfo MessageQueue::getInfo() const {
    return getInfo(msqid_);
}