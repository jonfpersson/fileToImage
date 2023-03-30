#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
using namespace cv;
#define PLACE_HOLDER_VALUE (34)

void setPixels(Mat** images, uint32_t valueToEncode, uint32_t x, uint32_t y){
    uint8_t part1 = (valueToEncode >> 16) & 0xFF;  // Most significant 8 bits
    uint8_t part2 = (valueToEncode >> 8) & 0xFF;   // Middle 8 bits
    uint8_t part3 = valueToEncode & 0xFF;
    (images[0])->at<Vec3b>(Point(x,y)) = Vec3b (part1, part2, part3);
}
void create_images(uint32_t frames, uint32_t rows, uint32_t cols, long size, uint8_t* index, Mat** images){
    long loopIndex = 0;
    uint32_t surplusBits = size % 3;

    //We add an extra pixel where one or two bits are the surplus bytes
    //If surplus is zero, we add none extra
    uint32_t pixelsInResult = size/3 + (surplusBits == 0 ? 0 : 1);
    printf("%i\n", pixelsInResult);
    for (uint32_t i = 0; i < frames; i++) {
        images[i] = new cv::Mat(rows, cols, CV_8UC3);
    }

    //Split numbers into 3*8bit and encode in pixels
    setPixels(images, pixelsInResult, 0,0);
    setPixels(images, rows, 1,0);
    setPixels(images, cols, 2,0);
    (images[0])->at<Vec3b>(Point(3,0)) = Vec3b (surplusBits, surplusBits, surplusBits);

    for(uint32_t i = 0; i < frames; i++){
        for(uint32_t row = 0; row < rows; row++){
            for(uint32_t col = 0; col < cols; col++){
                //We are maunaly setting the pixel values for (0,0) and (1,0)
                if(row == 0 && col == 0){
                    col = 4;
                }

                //Check if we're at the last byte
                if(loopIndex == size-1 && surplusBits == 0){
                    return;
                }

                //Read byte 1
                uint8_t byteOne = *index;
                
                //Check if we're at the last byte and if that byte's a surplus
                if(loopIndex == size-1 && surplusBits == 1){
                    (images[i])->at<Vec3b>(Point(col, row)) = Vec3b (byteOne, PLACE_HOLDER_VALUE, PLACE_HOLDER_VALUE);
                    return;
                }
                
                //Increment counter and read byte
                index += 1;
                uint8_t byteTwo = *index;
                loopIndex++;

                if(loopIndex == size-1 && surplusBits == 2){
                    (images[i])->at<Vec3b>(Point(col, row)) = Vec3b (byteOne, byteTwo, PLACE_HOLDER_VALUE);
                    return;
                }

                //Increment counter and read byte
                index += 1;
                uint8_t byteThree = *index;
                loopIndex++;

                //Increment for next round if not at eof
                if(loopIndex != size-1){
                    index += 1;
                    loopIndex++;
                }
                
                //Set the colors (bytes)
                (images[i])->at<Vec3b>(Point(col, row)) = Vec3b (byteOne, byteTwo, byteThree);
            }
        }
    }
}

int main() {
    FILE *frog = fopen("frog.jpg","rb");
    
    //Get file size
    fseek(frog, 0, SEEK_END);
    // Determine file size
    long size = ftell(frog);
    fseek(frog, 0, SEEK_SET);
    if(size % 3 != 0){
        printf("size %lu \n", size);
    }

    //Allocate memory for picture
    uint8_t* buffer = (uint8_t*) malloc(size * sizeof(uint8_t));
    fread(buffer, sizeof(uint8_t), size, frog);
    uint8_t* index = buffer;

    uint32_t cols = 1920;
    uint32_t rows = 1080;
    uint32_t frames = size/(3*rows*cols);
    //We do this to account for the last non-full frame
    frames++;

    //Create images
    cv::Mat** images = new cv::Mat*[frames]; // allocate array of pointers to cv::Mat objects

    create_images(frames, rows, cols, size, index, images);

    // free the memory
    for (uint32_t i = 0; i < frames; i++) {
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
