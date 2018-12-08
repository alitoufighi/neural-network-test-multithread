#include <stdint.h>
#include <cstdio>
#include <vector>
#include <iostream>
#include <sstream> //this header file is needed when using stringstream
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include "utils.hpp"
#include "globals.hpp"
using namespace std;


/**
 * @details Set cursor position to given coordinates in the terminal window
 */

void locateCursor(const int row, const int col){
    printf("%c[%d;%dH",27,row,col);
}

/**
 * @details Clear terminal screen by printing an escape sequence
 */

void clearScreen(){
    printf("\e[1;1H\e[2J");
}

/**
 * @details Outputs a 28x28 MNIST image as charachters ("."s and "X"s)
 */

void displayImage(MNIST_Image *img, int row, int col){

    char imgStr[(MNIST_IMG_HEIGHT * MNIST_IMG_WIDTH)+((col+1)*MNIST_IMG_HEIGHT)+1];
    strcpy(imgStr, "");

    for (int y=0; y<MNIST_IMG_HEIGHT; y++){

        for (int o=0; o<col-2; o++) strcat(imgStr," ");
        strcat(imgStr,"|");

        for (int x=0; x<MNIST_IMG_WIDTH; x++){
            strcat(imgStr, img->pixel[y*MNIST_IMG_HEIGHT+x] ? "X" : "." );
        }
        strcat(imgStr,"\n");
    }

    if (col!=0 && row!=0) locateCursor(row, 0);
    printf("%s",imgStr);
}

/**
 * @details Outputs a 28x28 text frame at a defined screen position
 */

void displayImageFrame(int row, int col){

    if (col!=0 && row!=0) locateCursor(row, col);

    printf("------------------------------\n");

    for (int i=0; i<MNIST_IMG_HEIGHT; i++){
        for (int o=0; o<col-1; o++) printf(" ");
        printf("|                            |\n");
    }

    for (int o=0; o<col-1; o++) printf(" ");
    printf("------------------------------");

}

/**
 * @details Outputs reading progress while processing MNIST testing images
 */

void displayLoadingProgressTesting(int imgCount, int y, int x){

    float progress = (float)(imgCount+1)/(float)(MNIST_MAX_TESTING_IMAGES)*100;

    if (x!=0 && y!=0) locateCursor(y, x);

    printf("Testing image No. %5d of %5d images [%d%%]\n                                  ",(imgCount+1),MNIST_MAX_TESTING_IMAGES,(int)progress);

}

/**
 * @details Outputs image recognition progress and error count
 */

void displayProgress(int imgCount, int errCount, int y, int x){

    double successRate = 1 - ((double)errCount/(double)(imgCount+1));

    if (x!=0 && y!=0) locateCursor(y, x);

    printf("Result: Correct=%5d  Incorrect=%5d  Success-Rate= %5.2f%% \n",imgCount+1-errCount, errCount, successRate*100);


}

/**
 * @details Reverse byte order in 32bit numbers
 * MNIST files contain all numbers in reversed byte order,
 * and hence must be reversed before using
 */

uint32_t flipBytes(uint32_t n){

    uint32_t b0,b1,b2,b3;

    b0 = (n & 0x000000ff) <<  24u;
    b1 = (n & 0x0000ff00) <<   8u;
    b2 = (n & 0x00ff0000) >>   8u;
    b3 = (n & 0xff000000) >>  24u;

    return (b0 | b1 | b2 | b3);

}

/**
 * @details Read MNIST image file header
 * @see http://yann.lecun.com/exdb/mnist/ for definition details
 */

void readImageFileHeader(FILE *imageFile, MNIST_ImageFileHeader *ifh){

    ifh->magicNumber =0;
    ifh->maxImages   =0;
    ifh->imgWidth    =0;
    ifh->imgHeight   =0;

    fread(&ifh->magicNumber, 4, 1, imageFile);
    ifh->magicNumber = flipBytes(ifh->magicNumber);

    fread(&ifh->maxImages, 4, 1, imageFile);
    ifh->maxImages = flipBytes(ifh->maxImages);

    fread(&ifh->imgWidth, 4, 1, imageFile);
    ifh->imgWidth = flipBytes(ifh->imgWidth);

    fread(&ifh->imgHeight, 4, 1, imageFile);
    ifh->imgHeight = flipBytes(ifh->imgHeight);
}

/**
 * @details Read MNIST label file header
 * @see http://yann.lecun.com/exdb/mnist/ for definition details
 */

void readLabelFileHeader(FILE *imageFile, MNIST_LabelFileHeader *lfh){

    lfh->magicNumber =0;
    lfh->maxImages   =0;

    fread(&lfh->magicNumber, 4, 1, imageFile);
    lfh->magicNumber = flipBytes(lfh->magicNumber);

    fread(&lfh->maxImages, 4, 1, imageFile);
    lfh->maxImages = flipBytes(lfh->maxImages);

}

/**
 * @details Open MNIST image file and read header info
 * by reading the header info, the read pointer
 * is moved to the position of the 1st IMAGE
 */

FILE *openMNISTImageFile(string fileName){

    FILE *imageFile;
    imageFile = fopen (fileName.c_str(), "rb");
    if (imageFile == NULL) {
        printf("Abort! Could not fine MNIST IMAGE file: %s\n", fileName.c_str());
        exit(0);
    }

    MNIST_ImageFileHeader imageFileHeader;
    readImageFileHeader(imageFile, &imageFileHeader);

    return imageFile;
}


/**
 * @details Open MNIST label file and read header info
 * by reading the header info, the read pointer
 * is moved to the position of the 1st LABEL
 */

FILE *openMNISTLabelFile(string fileName){

    FILE *labelFile;
    labelFile = fopen (fileName.c_str(), "rb");
    if (labelFile == NULL) {
        printf("Abort! Could not find MNIST LABEL file: %s\n",fileName.c_str());
        exit(0);
    }

    MNIST_LabelFileHeader labelFileHeader;
    readLabelFileHeader(labelFile, &labelFileHeader);

    return labelFile;
}

/**
 * @details Returns the next image in the given MNIST image file
 */

MNIST_Image getImage(FILE *imageFile){

    MNIST_Image img;
    size_t result;
    result = fread(&img, sizeof(img), 1, imageFile);
    if (result!=1) {
        printf("\nError when reading IMAGE file! Abort!\n");
        exit(1);
    }

    return img;
}

/**
 * @details Returns the next label in the given MNIST label file
 */

MNIST_Label getLabel(FILE *labelFile){

    MNIST_Label lbl;
    size_t result;
    result = fread(&lbl, sizeof(lbl), 1, labelFile);
    if (result!=1) {
        printf("\nError when reading LABEL file! Abort!\n");
        exit(1);
    }

    return lbl;
}

/**
 * @brief allocate weights and bias to respective hidden cells
 */

void allocateHiddenParameters(){
    int idx = 0;
    int bidx = 0;
    ifstream weights(HIDDEN_WEIGHTS_FILE);
    for(string line; getline(weights, line); idx++)   //read stream line by line
    {
        stringstream in(line);
        for (int i = 0; i < 28*28; ++i){
            in >> hidden_nodes[idx].weights[i];
      }
    }
    weights.close();

    ifstream biases(OUTPUT_BIASES_FILE);
    for(string line; getline(biases, line); bidx++)   //read stream line by line
    {
        stringstream in(line);
        in >> hidden_nodes[bidx].bias;
    }
    biases.close();

}

/**
 * @brief allocate weights and bias to respective output cells
 */

void allocateOutputParameters(){
    int idx = 0;
    int bidx = 0;
    ifstream weights(OUTPUT_WEIGHTS_FILE); //"layersinfo.txt"
    for(string line; getline(weights, line); ++idx)   //read stream line by line
    {
        stringstream in(line);
        for (int i = 0; i < 256; ++i){
            in >> output_nodes[idx].weights[i];
      }
    }
    weights.close();

    ifstream biases(OUTPUT_BIASES_FILE);
    for(string line; getline(biases, line); ++bidx)   //read stream line by line
    {
        stringstream in(line);
        in >> output_nodes[bidx].bias;
    }
    biases.close();

}

/**
 * @details The output prediction is derived by finding the maxmimum output value
 * and returning its index (=0-9 number) as the prediction.
 */

int getNNPrediction(){

    double maxOut = 0;
    int maxInd = 0;

    for (int i=0; i<NUMBER_OF_OUTPUT_CELLS; i++){

        if (output_nodes[i].output > maxOut){
            maxOut = output_nodes[i].output;
            maxInd = i;
        }
    }

    return maxInd;

}