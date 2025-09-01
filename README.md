# 🚀 OmarOS

🖥️ A minimalist operating system built from scratch using C++ and Assembly!

## ✨ Features:

- 💻 Command-line interface with history and tab completion
- 📁 Virtual file system with system file protection
- 🎨 Colorful terminal output with custom VGA driver
- 🤖 Interactive chatbot to keep you company
- 🔄 Tab autocompletion for commands and files
- 🔐 System file protection to prevent accidental deletion

## 🛠️ In Development:

- 🐍 Snake game (currently debugging keyboard input issues)
- 📝 Text editor (work in progress)

🔧 This educational project demonstrates low-level programming concepts including bootloader implementation, kernel development, and device interaction.

👨‍💻 Created with a 50/50 blend of human coding and AI assistance

🔥 Try it in QEMU or boot it on real hardware to experience the thrill of building an OS from the ground up! Perfect for OS development enthusiasts and learners! 🌟

## 📋 How to Run OmarOS

### Prerequisites

- Linux environment (Ubuntu/Debian recommended) or WSL on Windows
- QEMU emulator
- NASM assembler
- GCC/G++ compiler
- GNU Make
- GRUB bootloader utilities

### Installation of Dependencies

```bash
# For Ubuntu/Debian
sudo apt update
sudo apt install build-essential nasm gcc-multilib g++-multilib xorriso grub-pc-bin qemu-system-x86
```

### Building OmarOS

1. Clone the repository:
```bash
git clone https://github.com/eblon137/OmarOS
cd OmarOS
```

2. Compile the operating system:
```bash
make clean
make
```

### Running in QEMU

```bash
# Run with standard settings
make run

# Or run with debug information
make debug
```

### Running on Real Hardware

Create a bootable USB drive:
```bash
sudo dd if=myos.iso of=/dev/sdX bs=4M status=progress
```
Replace `/dev/sdX` with your USB device (be careful to select the correct device!).

### Basic Commands

Once OmarOS is running, you can use these commands:
- `help` - Display available commands
- `clear` - Clear the screen
- `ls` - List files in current directory
- `cd [dir]` - Change directory
- `mkdir [dir]` - Create a new directory
- `touch [file]` - Create a new file
- `cat [file]` - Display file contents
- `rm [path]` - Remove a file or directory
- `info` - Show system information
- `chat` - Start the chatbot
