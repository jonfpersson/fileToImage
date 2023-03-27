#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
using namespace cv;

void create_images(int frames, int rows, int cols, long size, uint8_t* index, Mat** images){
    long loopIndex = 0;

    for (int i = 0; i < frames; i++) {
        images[i] = new cv::Mat(rows, cols, CV_8UC3); // create a new cv::Mat object
    }
    //Split 64 bit number into 3*8bit number to encode size in pixels
    uint64_t number = size;
    uint8_t part1 = (number >> 16) & 0xFF;  // Most significant 8 bits
    uint8_t part2 = (number >> 8) & 0xFF;   // Middle 8 bits
    uint8_t part3 = number & 0xFF;
    (images[0])->at<Vec3b>(Point(0,0)) = Vec3b (part1, part2, part3);
    (images[0])->at<Vec3b>(Point(0,1)) = Vec3b (rows, cols, cols);

    printf("%i   %i    %i\n", part1, part2, part3);
    for(int i = 0; i < frames; i++){
        for(int row = 0; row < rows; row++){
            for(int col = 2; col < cols; col++){
                //Check if we're at the last byte
                if(loopIndex == size){
                    return;
                }
                
                uint8_t byteOne = *index;
                index += 1;
                
                uint8_t byteTwo = *index;
                index += 1;
                
                uint8_t byteThree = *index;
                index += 1;

                loopIndex += 3;
                //Set the colors (bytes)
                (images[i])->at<Vec3b>(Point(row, col)) = Vec3b (byteOne, byteTwo, byteThree);;
            }
        }
    }
}

int main() {
    FILE *frog = fopen("frog3.jpg","rb");
    
    //Get file size
    fseek(frog, 0, SEEK_END);
    // Determine file size
    long size = ftell(frog);
    fseek(frog, 0, SEEK_SET);
    if(size % 3 != 0){
        fclose(frog);
        return 1;
    }

    //Allocate memory for picture
    uint8_t* buffer = (uint8_t*) malloc(size * sizeof(uint8_t));
    fread(buffer, sizeof(uint8_t), size, frog);
    uint8_t* index = buffer;

    printf("size %lu \n", size);
    int cols = 300;
    int rows = 300;
    int frames = size/(3*rows*cols);
    int leftOverPixels = size - frames * (cols * rows);
    //We do this to account for the last non-full frame
    frames++;

    //Create images
    cv::Mat** images = new cv::Mat*[frames]; // allocate array of pointers to cv::Mat objects

    create_images(frames, rows, cols, size, index, images);

    // free the memory
    for (int i = 0; i < frames; i++) {
        imwrite("pics/" + std::to_string(i) + ".png",*images[i]);
        // delete the cv::Mat object
        delete images[i];
    }
    // delete the array of pointers
    delete[] images;

    free(buffer);
    fclose(frog);

    return 0;
}
