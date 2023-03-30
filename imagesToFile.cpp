#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace std;


int main() {
    Mat image = imread("pics/0.png");

    // Check if the image is loaded successfully
    if(image.empty())
    {
        cout << "Could not open or find the image" << endl;
        return -1;
    }
    // Get the number of rows and columns in the image
    int rows = image.rows;
    int cols = image.cols;

    ofstream out("result.jpg", ios::binary);

    Vec3b firstPixel = image.at<Vec3b>(0,0);
    uint8_t part1 = firstPixel.val[0];
    uint8_t part2 = firstPixel.val[1];
    uint8_t part3 = firstPixel.val[2];
    uint32_t size = (static_cast<uint32_t>(part1) << 16) | 
                    (static_cast<uint32_t>(part2) << 8) |
                    static_cast<uint32_t>(part3);

    Vec3b secondPixel = image.at<Vec3b>(0,1);
    int originalRows = secondPixel.val[0];
    int originalCols = secondPixel.val[1];
    int surplusBits = secondPixel.val[2];

    int index = 0;
    // Loop through each pixel in the encoded image
    for(int row = 0; row < rows; row++) {
        for(int col = 0; col < cols; col++) {
            if(index == size/3){
                goto end;
            }
            if(row == 0 && col == 0){
                col = 2;
            }

            // Get the RGB values of the current pixel
            Vec3b pixel = image.at<Vec3b>(row, col);
            int blue = pixel.val[0];
            int green = pixel.val[1];
            int red = pixel.val[2];

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
