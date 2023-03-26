# fileToImage

# Table of Contents
1. [Description](#Description)
2. [Install and Run the Project](#Install-and-Run-the-Project)
3. [How to play](#How-to-play)
3. [Technologies](#Technologies)
3. [Features](#Features)

# FileToImage - Convert a file into a serie of images

## Description
FileToImage is a program written in C++ which loads an ordinary file and converts it into a series of images. 
This is done by reading individual bytes and transforming them into RGB pixels.

## Install and Run the Project
This project is built on top of the OpenCV library. You'll need to have it installed to compile the program.

### How to install OpenCV on debian based systems
```sudo apt-get update```

```sudo apt-get install build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev```

```git clone https://github.com/opencv/opencv.git```

```cd opencv```

```mkdir build```

```cd build```

```cmake ..```

```make -j$(nproc)```

```sudo make install```

### Compile the program
```g++ -o main main.cpp -I /usr/local/include/opencv4/ -L /usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs ```

## Technologies

- OpenCV 4.7.0
- g++ 11.3.0
