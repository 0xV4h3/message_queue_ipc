# Contributing to message_queue_ipc

Thank you for your interest in contributing!  
We welcome issues, feature requests, bugfixes, tests, and documentation improvements.

## How to contribute

1. **Fork the repository**  
   Click "Fork" on GitHub to create your own copy.

2. **Clone your fork**
   ```bash
   git clone https://github.com/<your-username>/message_queue_ipc.git
   cd message_queue_ipc
   ```

3. **Create a new branch**
   ```bash
   git checkout -b my-feature-branch
   ```

4. **Make your changes**
   - Add code to `src/` for C++ sources and CLI tools.
   - Update documentation in `README.md` as needed.
   - Follow existing code style and conventions.

5. **Test your changes**
   - Build with CMake:  
     ```bash
     mkdir build
     cd build
     cmake ..
     make
     ```
   - Run and test the CLI tools as appropriate.

6. **Commit and push**
   ```bash
   git add <files>
   git commit -m "Describe your changes"
   git push origin my-feature-branch
   ```

7. **Open a Pull Request**
   - Go to your fork on GitHub.
   - Click "Compare & pull request".
   - Describe your changes clearly.

## Code Guidelines

- Follow C++17 or newer standards.
- Use clear, descriptive comments for non-obvious logic.
- Keep CLI tools simple and robust.
- Prefer standard library for threading/synchronization if relevant.

## Issues and Discussions

- Use [GitHub Issues](../../issues) for bugs, ideas, or questions.
- Please check for duplicates before submitting.
- You may also use [GitHub Discussions](../../discussions) for general topics.

## License

All contributions are under the MIT License.

---

Thank you for making message_queue_ipc better!