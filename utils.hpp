#ifndef _UTILS_H
#define _UTILS_H

#include <string>

#define MNIST_TESTING_SET_IMAGE_FILE_NAME "data/t10k-images-idx3-ubyte"  ///< MNIST image testing file in the data folder
#define MNIST_TESTING_SET_LABEL_FILE_NAME "data/t10k-labels-idx1-ubyte"  ///< MNIST label testing file in the data folder

#define HIDDEN_WEIGHTS_FILE "net_params/hidden_weights.txt"
#define HIDDEN_BIASES_FILE  "net_params/hidden_biases.txt"
#define OUTPUT_WEIGHTS_FILE "net_params/out_weights.txt"
#define OUTPUT_BIASES_FILE  "net_params/out_biases.txt"

#define NUMBER_OF_INPUT_CELLS  784   ///< use 28*28 input cells (= number of pixels per MNIST image)
#define NUMBER_OF_HIDDEN_CELLS 256  ///< use 256 hidden cells in one hidden layer
#define NUMBER_OF_OUTPUT_CELLS 10   ///< use 10 output cells to model 10 digits (0-9)

#define MNIST_MAX_TESTING_IMAGES 10000   ///< number of images+labels in the TEST file/s
#define MNIST_IMG_WIDTH  28               ///< image width in pixel
#define MNIST_IMG_HEIGHT 28              ///< image height in pixel

#define HIDDEN_THREADS 8  // Change it in order to test with other number of threads for hidden layer
#define NEURONS_PER_THREAD NUMBER_OF_HIDDEN_CELLS/HIDDEN_THREADS
#define OUTPUT_THREADS 10

typedef struct MNIST_ImageFileHeader MNIST_ImageFileHeader;
typedef struct MNIST_LabelFileHeader MNIST_LabelFileHeader;

typedef struct MNIST_Image MNIST_Image;
typedef uint8_t MNIST_Label;
typedef struct Hidden_Node Hidden_Node;
typedef struct Output_Node Output_Node;

/**
 * @brief Data block defining a hidden cell
 */

struct Hidden_Node{
    double weights[28*28];
    double bias;
    double output;
};

/**
 * @brief Data block 
 * defining an output cell
 */

struct Output_Node{
    double weights[256];
    double bias;
    double output;
};

/**
 * @brief Data block defining a MNIST image
 * @see http://yann.lecun.com/exdb/mnist/ for details
 */

struct MNIST_Image{
    uint8_t pixel[28*28];
};

/**
 * @brief Data block defining a MNIST image file header
 * @attention The fields in this structure are not used.
 * What matters is their byte size to move the file pointer
 * to the first image.
 * @see http://yann.lecun.com/exdb/mnist/ for details
 */

struct MNIST_ImageFileHeader{
    uint32_t magicNumber;
    uint32_t maxImages;
    uint32_t imgWidth;
    uint32_t imgHeight;
};

/**
 * @brief Data block defining a MNIST label file header
 * @attention The fields in this structure are not used.
 * What matters is their byte size to move the file pointer
 * to the first label.
 * @see http://yann.lecun.com/exdb/mnist/ for details
 */

struct MNIST_LabelFileHeader{
    uint32_t magicNumber;
    uint32_t maxImages;
};


/**
 * @details Set cursor position to given coordinates in the terminal window
 */

void locateCursor(const int row, const int col);

/**
 * @details Clear terminal screen by printing an escape sequence
 */

void clearScreen();

/**
 * @details Outputs a 28x28 MNIST image as charachters ("."s and "X"s)
 */

void displayImage(MNIST_Image *img, int row, int col);

/**
 * @details Outputs a 28x28 text frame at a defined screen position
 */

void displayImageFrame(int row, int col);

/**
 * @details Outputs reading progress while processing MNIST testing images
 */

void displayLoadingProgressTesting(int imgCount, int y, int x);

/**
 * @details Outputs image recognition progress and error count
 */

void displayProgress(int imgCount, int errCount, int y, int x);

/**
 * @details Reverse byte order in 32bit numbers
 * MNIST files contain all numbers in reversed byte order,
 * and hence must be reversed before using
 */

uint32_t flipBytes(uint32_t n);

/**
 * @details Read MNIST image file header
 * @see http://yann.lecun.com/exdb/mnist/ for definition details
 */

void readImageFileHeader(FILE *imageFile, MNIST_ImageFileHeader *ifh);

/**
 * @details Read MNIST label file header
 * @see http://yann.lecun.com/exdb/mnist/ for definition details
 */

void readLabelFileHeader(FILE *imageFile, MNIST_LabelFileHeader *lfh);

/**
 * @details Open MNIST image file and read header info
 * by reading the header info, the read pointer
 * is moved to the position of the 1st IMAGE
 */

FILE *openMNISTImageFile(std::string fileName);


/**
 * @details Open MNIST label file and read header info
 * by reading the header info, the read pointer
 * is moved to the position of the 1st LABEL
 */

FILE *openMNISTLabelFile(std::string fileName);

/**
 * @details Returns the next image in the given MNIST image file
 */

MNIST_Image getImage(FILE *imageFile);

/**
 * @details Returns the next label in the given MNIST label file
 */

MNIST_Label getLabel(FILE *labelFile);

/**
 * @brief allocate weights and bias to respective hidden cells
 */

void allocateHiddenParameters();

/**
 * @brief allocate weights and bias to respective output cells
 */

void allocateOutputParameters();

/**
 * @details The output prediction is derived by finding the maxmimum output value
 * and returning its index (=0-9 number) as the prediction.
 */

int getNNPrediction();

#endif