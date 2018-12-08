#include <vector>
#include <future>
#include "semaphore.hpp"
#include "utils.hpp"
#include "threads.hpp"
using namespace std;

// Results of testing with various number of threads for hidden layer:
// 1   thread:  256 nodes per thread: 14s
// 2   threads: 128 nodes per thread: 8s
// 4   threads: 64  nodes per thread: 7s
// 8   threads: 32  nodes per thread: 6s // Best choice!
// 16  threads: 16  nodes per thread: 8s
// 32  threads: 8   nodes per thread: 8s
// 64  threads: 4   nodes per thread: 10s
// 256 threads: 1   nodes per thread: 24s

Semaphore image_file(HIDDEN_THREADS);
Semaphore image_data(0);
vector<Semaphore> hidden_layer_output(HIDDEN_THREADS, Semaphore(OUTPUT_THREADS)); 
vector<Semaphore> output_layer_input(OUTPUT_THREADS, Semaphore(0));
Semaphore output_layer_output(0);
vector<Semaphore> prediction_layer_input(OUTPUT_THREADS, Semaphore(1));

/**
 * @details test the neural networks to obtain its accuracy when classifying
 * 10k images.
 */

void testNN(){
    // screen output for monitoring progress
    displayImageFrame(7,5);

    vector<std::future<void> > hidden_layer_threads;
    vector<std::future<void> > output_layer_threads;

    std::future<void> get_image_thread = std::async(get_image_MT);
    for(int i = 0; i < HIDDEN_THREADS; ++i)
        hidden_layer_threads.push_back(std::async(hidden_layer_MT, i));
    for(int i = 0; i < OUTPUT_THREADS; ++i)
        output_layer_threads.push_back(std::async(output_layer_MT, i));
    std::future<void> predictNumMT = std::async(predict_num_MT);
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
