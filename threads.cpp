#include <cstdio>
#include "utils.hpp"
#include "threads.hpp"
#include "globals.hpp"

vector<Hidden_Node> hidden_nodes(NUMBER_OF_HIDDEN_CELLS);
vector<Output_Node> output_nodes(NUMBER_OF_OUTPUT_CELLS);

MNIST_Image img;
MNIST_Label lbl;

void getImageAndLableMT(){
    // open MNIST files
    FILE *imageFile, *labelFile;
    imageFile = openMNISTImageFile(MNIST_TESTING_SET_IMAGE_FILE_NAME);
    labelFile = openMNISTLabelFile(MNIST_TESTING_SET_LABEL_FILE_NAME);

    for(int i = 0; i < MNIST_MAX_TESTING_IMAGES; ++i){
        // sem_wait(&file_to_input_layer);
        // ye lock vase wait o signal
        // ye flag vase in ke begam alan nobate mane ya layere badi
        img = getImage(imageFile);
        lbl = getLabel(labelFile);
        // sem_post(&file_to_input_layer);
        // lock.unlock();
    }

    // Close files
    fclose(imageFile);
    fclose(labelFile);
}