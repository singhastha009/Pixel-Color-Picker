# 🎨 Pixel Color Picker – GTK 4 (C)

A lightweight desktop application written in C using GTK 4, allowing users to pick pixel colors from an image by clicking directly or entering coordinates. This tool demonstrates real-time pixel inspection, gesture handling, RGB color extraction, and GUI element updates in GTK.

---

## 🧠 Features

- Click on an image to get the RGB color of a pixel  
- Enter X and Y coordinates manually to retrieve pixel color  
- Preview the selected color inside a styled display frame  
- Uses GTK 4, GDK Pixbuf, and CSS for visual styling  
- Simple, clean, and responsive interface

---

## 🛠 Technologies Used

- **Language:** C  
- **GUI Toolkit:** GTK 4  
- **Image Processing:** GDK Pixbuf  
- **CSS Styling:** GTK CSS  
- **Build Tool:** gcc (or make)

---

## 🚀 How to Build and Run

### 1. Install GTK 4

#### Ubuntu/Debian:
```bash
sudo apt install libgtk-4-dev

Compile - gcc pixel_picker.c -o colorpicker $(pkg-config --cflags --libs gtk4)
Run - ./colorpicker
