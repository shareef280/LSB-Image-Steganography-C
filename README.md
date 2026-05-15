 # LSB Image Steganography using C

## 📌 Project Overview
This project implements **LSB (Least Significant Bit) Image Steganography** using the C programming language.  
The system securely hides confidential text messages inside BMP image files without visibly affecting the original image quality.

Steganography is a technique used in cybersecurity and information security to conceal secret data inside digital media such as images, audio, or video files.

This project demonstrates:
- Image-based data hiding
- Bit manipulation in C
- File handling concepts
- Secure message extraction

---

# ✨ Features

- Encode secret text inside BMP images
- Decode and retrieve hidden messages
- Uses Least Significant Bit (LSB) technique
- Magic string authentication for security
- BMP image validation
- Command-line based implementation
- Lightweight and efficient design

---

# 🛠️ Technologies Used

- C Programming
- File Handling
- Bit Manipulation
- BMP Image Processing
- Linux / GCC Compiler

---

# 📂 Project Structure

```text
LSB-Image-Steganography-C/
│
├── main.c
├── encode.c
├── decode.c
├── encode.h
├── decode.h
├── common.h
├── types.h
├── beautiful.bmp
├── stego.bmp
├── secret.txt
├── output.txt
└── README.md
