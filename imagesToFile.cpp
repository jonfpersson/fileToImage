#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace std;

uint32_t subPixelsToValue(Mat* image, uint32_t row, uint32_t col){
    Vec3b firstPixel = image->at<Vec3b>(row, col);
    uint8_t part1 = firstPixel.val[0];
    uint8_t part2 = firstPixel.val[1];
    uint8_t part3 = firstPixel.val[2];
    uint32_t total = (static_cast<uint32_t>(part1) << 16) | 
                    (static_cast<uint32_t>(part2) << 8) |
                    static_cast<uint32_t>(part3);
    return total;
}

int main() {
    Mat image = imread("pics/0.png");
    uint32_t size = subPixelsToValue(&image, 0,0);
    uint32_t rows = subPixelsToValue(&image, 0,1);
    uint32_t cols = subPixelsToValue(&image, 0,2);
    uint32_t surplusBits = image.at<Vec3b>(0,3).val[0];

    // Check if the image is loaded successfully
    if(image.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }
    ofstream out("result.jpg", ios::binary);

    uint32_t index = 0;
    // Loop through each pixel in the encoded image
    for(uint32_t row = 0; row < image.rows; row++) {
        for(uint32_t col = 0; col < image.cols; col++) {
            if(index == size/3){
                goto end;
            }
            if(row == 0 && col == 0){
                col = 4;
            }

            // Get the RGB values of the current pixel
            Vec3b pixel = image.at<Vec3b>(row, col);
            uint32_t blue = pixel.val[0];
            uint32_t green = pixel.val[1];
            uint32_t red = pixel.val[2];

            // Write some bytes to the output file
            // Only write correct amount bytes
            out.put(blue);
            
            if(surplusBits == 1 && index == size-1)
                return 0;
            out.put(green);
            
            if(surplusBits == 2 && index == size-1)
                return 0;

            out.put(red);
            
            index++;
        }
    }

end:
printf("%i\n", index);
    out.close();
    return 0;
}
