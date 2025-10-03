# Message Queue IPC System

This project provides a robust and modern C++ interface for System V message queues on Linux.  
It includes a reusable class (`MessageQueue`) and a set of CLI utilities for creating, sending, receiving, resizing, removing, and monitoring message queues.

---

## Features

- **Safe and modern C++:** All low-level IPC logic wrapped in a class with RAII principles, exceptions, and `std::string` support.
- **Command-line tools:** Each action (create, send, receive, change size, remove, info) is a separate utility.
- **Strict argument validation:** All utilities check and validate user input, including type, range, and permissions.
- **Informative error handling:** Clear messages and exit codes.
- **Interactive fallback:** Utilities can prompt for missing arguments if needed.
- **Configurable permissions:** You can set queue permissions on creation.
- **Monitoring:** View detailed info about queues with the `message_info` utility.

---

## File Structure

```
src/
  message_queue.hpp       # C++ class interface for message queues
  message_queue.cpp       # Implementation of MessageQueue class
  message_create.cpp      # CLI utility: create a queue
  message_send.cpp        # CLI utility: send a message
  message_receive.cpp     # CLI utility: receive a message
  message_chqbytes.cpp    # CLI utility: change queue max bytes
  message_rm.cpp          # CLI utility: remove queue(s)
  message_info.cpp        # CLI utility: show info about a queue

CMakeLists.txt            # Build system configuration
README.md                 # Project documentation
LICENSE                   # MIT License
```

---

## How to Build (Recommended: CMake)

1. **Clone the repository from GitHub**
   ```bash
   git clone https://github.com/0xV4h3/message_queue_ipc.git
   cd message_queue_ipc
   ```

2. **Install CMake and build essentials (if not already installed)**
   ```bash
   sudo apt update
   sudo apt install cmake build-essential
   ```

3. **Build the project**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

---

## CLI Utilities Usage

**Create a queue:**
```bash
./message_create <key> <max_bytes> [permissions]
```
- `<key>`: Integer key for the queue (unique per queue).
- `<max_bytes>`: Maximum total size of the queue in bytes.
- `[permissions]`: Optional; permissions in octal (default: 0600).

**Send a message:**
```bash
./message_send <msqid> <type> [message]
```
- `<msqid>`: Message queue ID.
- `<type>`: Message type (integer, positive).
- `[message]`: Optional; if not provided, the utility will prompt for input.

**Receive a message:**
```bash
./message_receive <msqid> <type> [--nowait|-n]
```
- `<msqid>`: Message queue ID.
- `<type>`: Message type (integer, positive).
- `[--nowait|-n]`: Optional; do not block if no message is present.

**Change queue size:**
```bash
./message_chqbytes <msqid> <max_bytes>
```
- `<msqid>`: Message queue ID.
- `<max_bytes>`: New maximum size in bytes for the queue.

**Remove queue(s):**
```bash
./message_rm <msqid> [msqid ...]
```
- `<msqid>`: One or more message queue IDs (separated by spaces).
- You can specify multiple IDs to remove several queues at once.

**Show queue info:**
```bash
./message_info <msqid>
```
- `<msqid>`: Message queue ID.
- Displays the queue's owner, permissions, message count, bytes used, maximum size, and last operation times.

---

## Monitoring

To monitor and debug message queues, you can use:

- `ipcs -q` — List all active System V message queues, including msqid, owner, permissions, usage.
- `cat /proc/sysvipc/msg` — Detailed info about all queues (Linux only).
- `ps aux | grep message_` — Check running CLI utilities of this project.

### Built-in Monitoring

Use the `message_info` utility to display detailed information about a queue:
```bash
./message_info <msqid>
```
Output includes:
- Message count
- Bytes used
- Owner and group
- Permissions
- Maximum queue size
- Last operation times

### Logging (Recommendation)

You may extend the CLI utilities to log actions (create, send, receive, remove) to a file (e.g., `/var/log/message_queue_ipc.log`) or use system logging (`syslog`) for audit and debugging purposes.

### Resource Management (Recommendation)

For advanced resource management, you may implement signal handlers in your CLI utilities to automatically clean up message queues on termination (e.g., SIGINT/SIGTERM).

---

## Production & Global Usage

For production or system-wide use, you may want the CLI utilities to be available as regular Linux commands, accessible from anywhere in the terminal.

### System-wide Installation

1. **Build the project as described above.**
2. **Copy the binaries to a directory in your `$PATH` (e.g., `/usr/local/bin`):**

   ```bash
   sudo cp build/message_create /usr/local/bin/
   sudo cp build/message_send /usr/local/bin/
   sudo cp build/message_receive /usr/local/bin/
   sudo cp build/message_chqbytes /usr/local/bin/
   sudo cp build/message_rm /usr/local/bin/
   sudo cp build/message_info /usr/local/bin/
   ```

3. **Now you can use the tools from any location:**

   ```bash
   message_create ...
   message_send ...
   message_receive ...
   message_chqbytes ...
   message_rm ...
   message_info ...
   ```

### Additional Recommendations for Production

- Ensure appropriate permissions for binaries (`chmod +x /usr/local/bin/message_*`).
- Restrict usage to trusted users or groups if handling sensitive data.
- Regularly monitor and clean up unused queues.
- Consider enabling logging for operational tracking.
- Rebuild and reinstall binaries after updates.

---

## Environment

- **Operating System:** Linux
- **IPC Mechanism:** System V message queues (`sys/msg.h`)
- **Compiler:** Any C++17-compatible compiler (tested with g++)
- **Build System:** CMake (recommended)
- **Dependencies:** Standard Linux development tools

---

## Project Purpose

This project is designed for developers, students, and engineers who need a safe, convenient, and modern way to work with message queues in C++ for inter-process communication (IPC) in Linux environments.  
The `MessageQueue` class can be reused or extended in other C++ projects needing IPC functionality.
