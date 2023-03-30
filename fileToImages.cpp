#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
using namespace cv;
#define PLACE_HOLDER_VALUE (34)
void create_images(int frames, int rows, int cols, long size, uint8_t* index, Mat** images){
    long loopIndex = 0;
    int surplusBits = size % 3;
    printf("surplus %i\n", surplusBits);
    //We add an extra pixel where one or two bits are the surplus bytes
    //If surplus is zero, we add none extra
    int pixelsInResult = size/3 + (surplusBits == 0 ? 0 : 1);

    for (int i = 0; i < frames; i++) {
        images[i] = new cv::Mat(rows, cols, CV_8UC3);
    }
    //Split 64 bit number into 3*8bit number to encode size in pixels
    uint32_t number = pixelsInResult;
    uint8_t part1 = (number >> 16) & 0xFF;  // Most significant 8 bits
    uint8_t part2 = (number >> 8) & 0xFF;   // Middle 8 bits
    uint8_t part3 = number & 0xFF;
    (images[0])->at<Vec3b>(Point(0,0)) = Vec3b (part1, part2, part3);
    (images[0])->at<Vec3b>(Point(1,0)) = Vec3b (rows, cols, surplusBits);

    printf("%i   %i    %i\n", part1, part2, part3);
    for(int i = 0; i < frames; i++){
        for(int row = 0; row < rows; row++){
            for(int col = 0; col < cols; col++){
                //We are maunaly setting the pixel values for (0,0) and (1,0)
                if(row == 0 && col == 0){
                    col = 2;
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

    int cols = 255;
    int rows = 255;
    int frames = size/(3*rows*cols);
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
