#include <cmath>
#include "utils.hpp"
#include "globals.hpp"
using namespace std;

vector<Hidden_Node> hidden_nodes(NUMBER_OF_HIDDEN_CELLS);
vector<Output_Node> output_nodes(NUMBER_OF_OUTPUT_CELLS);

MNIST_Image img;

void get_image_MT(){
    // open MNIST files
    FILE* imageFile = openMNISTImageFile(MNIST_TESTING_SET_IMAGE_FILE_NAME);

    for(int imgCount = 0; imgCount < MNIST_MAX_TESTING_IMAGES; ++imgCount){
        
        for(int i = 0; i < HIDDEN_THREADS; ++i)
            image_file.wait();
        
        img = getImage(imageFile);

        for(int i = 0; i < HIDDEN_THREADS; ++i)
            image_data.signal();
    }

    // Close files
    fclose(imageFile);
}

// each thread has an `id`
// which creates boundary of neurons is should process
void hidden_layer_MT(int id){
    for(int imgCount = 0; imgCount < MNIST_MAX_TESTING_IMAGES; ++imgCount){

        for(int i = 0; i < OUTPUT_THREADS; ++i)
            hidden_layer_output[id].wait();
        image_data.wait();

        for (int j = id*NEURONS_PER_THREAD; j < (id+1)*NEURONS_PER_THREAD; j++) {
            hidden_nodes[j].output = 0;
            for (int z = 0; z < NUMBER_OF_INPUT_CELLS; z++)
                hidden_nodes[j].output += img.pixel[z] * hidden_nodes[j].weights[z];
            hidden_nodes[j].output += hidden_nodes[j].bias;
            hidden_nodes[j].output = (hidden_nodes[j].output >= 0) ?  hidden_nodes[j].output : 0;
        }

        for(int i = 0; i < OUTPUT_THREADS; ++i)
            output_layer_input[i].signal();
        image_file.signal();
    }
}

// 10 output layers, 10 threads.
// each call is per each output layer.
void output_layer_MT(int id){
    for(int imgCount = 0; imgCount < MNIST_MAX_TESTING_IMAGES; ++imgCount){
        
        for(int i = 0; i < HIDDEN_THREADS; ++i)
            output_layer_input[id].wait();
        prediction_layer_input[id].wait();

        output_nodes[id].output = 0;
        for (int j = 0; j < NUMBER_OF_HIDDEN_CELLS; j++)
            output_nodes[id].output += hidden_nodes[j].output * output_nodes[id].weights[j];
        output_nodes[id].output += 1/(1 + exp(-1* output_nodes[id].output));

        for(int i = 0; i < HIDDEN_THREADS; ++i)
            hidden_layer_output[i].signal();
        output_layer_output.signal();
    }
}

// one thread for predict num, according to output layers
void predict_num_MT(){
    // open MNIST files
    FILE* labelFile = openMNISTLabelFile(MNIST_TESTING_SET_LABEL_FILE_NAME);
    FILE* imageFile = openMNISTImageFile(MNIST_TESTING_SET_IMAGE_FILE_NAME);
    MNIST_Image img;
    MNIST_Label lbl;

    int errCount = 0;

    for(int imgCount = 0; imgCount < MNIST_MAX_TESTING_IMAGES; ++imgCount){
        
        for(int i = 0; i < OUTPUT_THREADS; ++i)
            output_layer_output.wait();
        int predictedNum = getNNPrediction();
        lbl = getLabel(labelFile);
        img = getImage(imageFile);
        if (predictedNum!=lbl) errCount++;
        
        displayLoadingProgressTesting(imgCount,5,5);
        displayImage(&img, 8,6);
        printf("\n      Prediction: %d   Actual: %d ",predictedNum, lbl);
        displayProgress(imgCount, errCount, 5, 66);

        for(int i = 0; i < OUTPUT_THREADS; ++i)
            prediction_layer_input[i].signal();
    }
    
    fclose(labelFile);
}