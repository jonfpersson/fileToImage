#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
using namespace cv;

void create_images(int frames, int rows, int cols, int size, uint8_t* index, Mat** images){
    long loopIndex = 0;

    for (int i = 0; i < frames; i++) {
        images[i] = new cv::Mat(rows, cols, CV_8UC3); // create a new cv::Mat object
    }

    for(int i = 0; i < frames; i++){
        for(int y = 0; y < rows; y++){
            for(int x = 0; x < cols; x++){
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
                (images[i])->at<Vec3b>(Point(x,y)) = Vec3b (byteOne, byteTwo, byteThree);;
            }
        }
    }
}

int main() {
    FILE *frog = fopen("frogg.webp","rb");
    
    //Get file size
    fseek(frog, 0, SEEK_END);
    // Determine file size
    long size = ftell(frog) + 1;
    fseek(frog, 0, SEEK_SET);

    //Allocate memory for picture
    uint8_t* buffer = (uint8_t*) malloc(size * sizeof(uint8_t));
    fread(buffer, sizeof(uint8_t), size, frog);
    uint8_t* index = buffer;

    printf("size %lu \n", size);
    int cols = 50;
    int rows = 50;
    int frames = size/(3*rows*cols);
    int leftOverPixels = size - frames * (cols * rows);
    //We do this to account for the last non-full frame
    frames++;

    //Create images
    cv::Mat** images = new cv::Mat*[frames]; // allocate array of pointers to cv::Mat objects

    create_images(frames, rows, cols, size, index, images);

end:
    printf("frames %i\n", frames);

    // free the memory
    for (int i = 0; i < frames; i++) {
        imwrite("pics/" + std::to_string(i) + ".jpg",*images[i]);
        // delete the cv::Mat object
        delete images[i];
    }
    // delete the array of pointers
    delete[] images;

    fclose(frog);
    free(buffer);

    return 0;
}
