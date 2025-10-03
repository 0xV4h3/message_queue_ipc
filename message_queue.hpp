#pragma once

#include <string>
#include <stdexcept>
#include <sys/types.h>

// Structure to hold detailed information about a message queue
struct QueueInfo {
    int msqid;                   // Message queue ID
    key_t key;                   // Key used to create the queue
    uid_t owner_uid;             // Owner's UID
    gid_t owner_gid;             // Owner's GID
    unsigned short permissions;  // Permissions (octal)
    size_t max_bytes;            // Maximum bytes allowed in the queue
    size_t used_bytes;           // Bytes currently used
    unsigned int num_messages;   // Number of messages in queue
    time_t last_send_time;       // Time of last msgsnd
    time_t last_recv_time;       // Time of last msgrcv
    time_t last_change_time;     // Time of last change
};

class MessageQueue {
public:
    // Static factory method: create a new queue
    static MessageQueue create(key_t key, size_t max_bytes, unsigned short permissions = 0600);

    // Static factory method: attach to an existing queue
    static MessageQueue attach(int msqid);

    // --- Static variants for all core operations ---

    // Send a message to the queue
    // Throws std::runtime_error on failure
    static void sendMessage(int msqid, long type, const std::string &message);

    // Receive a message from the queue.
    // By default, blocks until a message is available. If nowait is true, returns immediately with an exception if no message is present.
    // Throws std::runtime_error on failure or if no message is present in non-blocking mode.
    static std::string receiveMessage(int msqid, long type, bool nowait = false);

    // Change maximum allowed bytes for the queue
    // Throws std::runtime_error on failure
    static void setMaxBytes(int msqid, size_t max_bytes);

    // Remove (delete) the message queue with this msqid from the system.
    // Throws std::runtime_error on failure
    static void remove(int msqid);

    // Get detailed queue info
    // Throws std::runtime_error on failure
    static QueueInfo getInfo(int msqid);

    // --- Non-static (object) variants ---

    // Send a message to the queue
    // Throws std::runtime_error on failure
    void sendMessage(long type, const std::string &message);

    // Receive a message from the queue.
    // By default, blocks until a message is available. If nowait is true, returns immediately with an exception if no message is present.
    // Throws std::runtime_error on failure or if no message is present in non-blocking mode.
    std::string receiveMessage(long type, bool nowait = false);

    // Change maximum allowed bytes for the queue
    // Throws std::runtime_error on failure
    void setMaxBytes(size_t max_bytes);

    // Remove (delete) the message queue from the system (object method).
    // Throws std::runtime_error on failure
    void remove();

    // Get detailed queue info
    // Throws std::runtime_error on failure
    QueueInfo getInfo() const;

    // Get underlying msqid
    int getMsqid() const { return msqid_; }

    // Destructor
    ~MessageQueue() = default;

    // Deleted copy operations
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    // Allowed move operations
    MessageQueue(MessageQueue&&) = default;
    MessageQueue& operator=(MessageQueue&&) = default;

private:
    explicit MessageQueue(int msqid);

    int msqid_;
};