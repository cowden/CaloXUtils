#ifndef PROCESS_H
#define PROCESS_H

/**
* @file process.h
* @author C S Cowden
* @brief Declare a process node for the graph
*/

// --- includes ---
#include "node.h"

namespace cg {

/**
* @brief Process class - Node
* @details The process node captures a physics process implemented and limiting a step in a Geant4 simulation.
* Analyzing the graph and summing energy deposition and physical extents of the graph below
* a process can be used to understand the shape of sub-showers within a calorimeter.
*/
class process : public node {
}; 

}

#endif
