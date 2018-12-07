#include <stdint.h>
#include <cstdio>
#include <math.h>
#include <vector>
#include <time.h>
#include <iostream>
#include <sstream> //this header file is needed when using stringstream
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <semaphore.h>
#include "semaphore.hpp"
#include "utils.hpp"
#include "threads.hpp"
#include "globals.hpp"
using namespace std;

// sem_t file_to_input_layer;                              // file to MNIST_Image object
// sem_t input_to_hidden_layer[NUM_OF_HIDDEN_THREADS];     // image object to hidden_layer
// sem_t hidden_to_output_layer[NUM_OF_OUTPUT_THREADS];    // output of hidden_layers and input of output_layer 
// sem_t output_to_presentation_layer;                     // output of output_layer to presentation

// toye jayi ke file ro read mikonam, 8 bar wait mikonam k motmaen shim masraf shode
// toye jayi ke hidden layer ro misazam, signal mikonam 1 bar.
Semaphore image_file(8); // masraf shodane kamele img[] (init_hidden)
// access to image file after all hidden layers have read from it.
// Initially, it's okay to have access to it

// toye jayi ke image ro misazam, signal mikonam 8 bar
// toye jayi ke hidden ro misazam, wait mikonam 1 bar
Semaphore image_data(0); // amade bodane img[] (hidden_init)
// at first, data is not available.
// getImage will increase it by 8, each hidden layer thread will wait() for it (decrement).

// aya har 10 ta output_layer, dataye man ro khonde? (hidden_output)
// avale onjayi ke hidden_layer ro misazam, 10 bar wait mikonam
// akhare onjayi ke output_layer ro sakhtam, 1 bar signal mikonam baraye har hidden layer thread
vector<Semaphore> hidden_layer_output(8, Semaphore(10)); 
// for each hidden_layer thread
// does all of output_layer neurons have got my outputs? initially, yes. 

// amade bodane dataye har hidden_layer thread (output_hidden)
// be ettelae har output_layer thread mirese
vector<Semaphore> output_layer_input(10, Semaphore(0));
// for each output_layer
// are outputs of all of hidden_layer neurons ready? initially, no.

// datae har output layer amadast?
// toye jayi k mikham predict konam, 10 bar wait mikonam (predict_output)
Semaphore output_layer_output(0);
// is data of all output_layer neurons ready? Initially, no.

// datae man (output_layer) masraf shode? (output_predict)
// toye jayi k mikham predict konam, signal mikonam baraye har output layer
// toye jayi k output_layero misazam, wait mikonam roye layere khodam
vector<Semaphore> prediction_layer_input(10, Semaphore(1));
// for each output_layer
// does prediction_layer has got my outputs? initially, no.

// vector<Hidden_Node> hidden_nodes(NUMBER_OF_HIDDEN_CELLS);
// vector<Output_Node> output_nodes(NUMBER_OF_OUTPUT_CELLS);

// MNIST_Image img;
// MNIST_Label lbl;


/**
 * @details test the neural networks to obtain its accuracy when classifying
 * 10k images.
 */

void testNN(){
        // open MNIST files
    FILE *imageFile, *labelFile;
    imageFile = openMNISTImageFile(MNIST_TESTING_SET_IMAGE_FILE_NAME);
    labelFile = openMNISTLabelFile(MNIST_TESTING_SET_LABEL_FILE_NAME);


    // screen output for monitoring progress
    displayImageFrame(7,5);

    // number of incorrect predictions
    int errCount = 0;


    // Loop through all images in the file
    for (int imgCount=0; imgCount<MNIST_MAX_TESTING_IMAGES; imgCount++){
        // display progress
        displayLoadingProgressTesting(imgCount,5,5);

        // Reading next image and corresponding label
        // getImageAndLableMT();
        MNIST_Image img = getImage(imageFile);
        MNIST_Label lbl = getLabel(labelFile);

        displayImage(&img, 8,6);

        // loop through all output cells for the given image
        for (int i= 0; i < NUMBER_OF_OUTPUT_CELLS; i++){
            output_nodes[i].output = 0;
            for (int j = 0; j < NUMBER_OF_HIDDEN_CELLS; j++) {
                hidden_nodes[j].output = 0;
                for (int z = 0; z < NUMBER_OF_INPUT_CELLS; z++) {
                    hidden_nodes[j].output += img.pixel[z] * hidden_nodes[j].weights[z];
                }
                hidden_nodes[j].output += hidden_nodes[j].bias;
                hidden_nodes[j].output = (hidden_nodes[j].output >= 0) ?  hidden_nodes[j].output : 0;
                output_nodes[i].output += hidden_nodes[j].output * output_nodes[i].weights[j];
            }
            output_nodes[i].output += 1/(1+ exp(-1* output_nodes[i].output));
        }

        int predictedNum = getNNPrediction();
        if (predictedNum!=lbl) errCount++;

        printf("\n      Prediction: %d   Actual: %d ",predictedNum, lbl);

        displayProgress(imgCount, errCount, 5, 66);

    }

    // // Close files
    // fclose(imageFile);
    // fclose(labelFile);

}

int main(int argc, const char * argv[]) {

    // remember the time in order to calculate processing time at the end
    time_t startTime = time(NULL);

    // clear screen of terminal window
    clearScreen();
    printf("    MNIST-NN: a simple 2-layer neural network processing the MNIST handwriting images\n");

    // alocating respective parameters to hidden and output layer cells
    allocateHiddenParameters();
    allocateOutputParameters();

    //test the neural network
    testNN();

    locateCursor(38, 5);

    // calculate and print the program's total execution time
    time_t endTime = time(NULL);
    double executionTime = difftime(endTime, startTime);
    printf("\n    DONE! Total execution time: %.1f sec\n\n",executionTime);

    return 0;
}
