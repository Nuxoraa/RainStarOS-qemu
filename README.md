RainStarOS is a lightweight, x86-based educational operating system kernel developed to explore low-level system architecture and modular OS design.

Currently codenamed Obsidian, the project features a custom shell, basic memory management, and a built-in scripting environment.

🚀 Key Features

    Custom Kernel: Written in C and Assembly, booting via Multiboot compliant bootloaders like GRUB.

VBasic Interpreter: A built-in simplified Basic interpreter for running scripts (e.g., calc.bas, matrix.bas).

Fastool: A diagnostic utility providing system info, CPU vendor detection (via CPUID), and memory stats.

Package Management (DNF-like): A basic simulation of a package manager to "install" scripts from a virtual repository into the system.

Simple File System: Supports up to 16 active files in memory with basic ls and run capabilities.

Hardware Support:

    VGA Text Mode (80x25) with scrolling support.

PS/2 Keyboard driver with Shift-key support.

Low-level I/O port communication.

🛠 Tech Stack

    Language: C (Freestanding), NASM Assembly.

Architecture: x86 (32-bit).

Build System: Shell scripts with GCC and LD.

Target Environment: QEMU or physical hardware via ISO.

📂 Project Structure
Plaintext

RainStarOS/
├── boot.s       # Multiboot header and kernel entry point [cite: 43]
├── kernel.c     # Core logic, drivers, and shell [cite: 2, 31]
├── linker.ld    # Memory layout definition [cite: 42]
├── build.sh     # Compilation and ISO generation script 
└── LICENSE      # MIT License 

🏗 Building and Running

To build the OS and run it in QEMU, ensure you have nasm, gcc-multilib, grub-common, and xorriso installed.

    Grant execution permissions:
    Bash

chmod +x build.sh

Run the build script:
Bash

    ./build.sh

This will compile the kernel, create a bootable ISO image (myos.iso), and launch it using QEMU.

📜 License

This project is licensed under the MIT License - see the LICENSE file for details.

Created by nuxora.
