#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <vector>
#include "utils.hpp"
#include "semaphore.hpp"

extern MNIST_Image img;
extern MNIST_Label lbl;
extern std::vector<Hidden_Node> hidden_nodes;
extern std::vector<Output_Node> output_nodes;

// toye jayi ke file ro read mikonam, 8 bar wait mikonam k motmaen shim masraf shode
// toye jayi ke hidden layer ro misazam, signal mikonam 1 bar.
extern Semaphore image_file; // masraf shodane kamele img[] (init_hidden)
// access to image file after all hidden layers have read from it.
// Initially, it's okay to have access to it

// toye jayi ke image ro misazam, signal mikonam 8 bar
// toye jayi ke hidden ro misazam, wait mikonam 1 bar
extern Semaphore image_data; // amade bodane img[] (hidden_init)
// at first, data is not available.
// getImage will increase it by 8, each hidden layer thread will wait() for it (decrement).

// aya har 10 ta output_layer, dataye man ro khonde? (hidden_output)
// avale onjayi ke hidden_layer ro misazam, 10 bar wait mikonam
// akhare onjayi ke output_layer ro sakhtam, 1 bar signal mikonam baraye har hidden layer thread
extern std::vector<Semaphore> hidden_layer_output; 
// for each hidden_layer thread
// does all of output_layer neurons have got my outputs? initially, yes. 

// amade bodane dataye har hidden_layer thread (output_hidden)
// be ettelae har output_layer thread mirese
extern std::vector<Semaphore> output_layer_input;
// for each output_layer
// are outputs of all of hidden_layer neurons ready? initially, no.

// datae har output layer amadast?
// toye jayi k mikham predict konam, 10 bar wait mikonam (predict_output)
extern Semaphore output_layer_output;
// is data of all output_layer neurons ready? Initially, no.

// datae man (output_layer) masraf shode? (output_predict)
// toye jayi k mikham predict konam, signal mikonam baraye har output layer
// toye jayi k output_layero misazam, wait mikonam roye layere khodam
extern std::vector<Semaphore> prediction_layer_input;
// for each output_layer
// does prediction_layer has got my outputs? initially, no.
#endif
