#ifndef TRACK_H
#define TRACK_H

/**
* @file track.h
* @author C S Cowden
* @brief Declare the track class (inherit from node).
*/

// --- includes ---
#include "node.h"

namespace cg {

/**
* @brief Track class - node specialization
* @details The track class records particles
* between processes.  A track implemented in CaloGraphy
* is more closely related to the Geant4 track/particle from
* step to step since in many processes a single track 
* may interact and continue (e.g. an electron and the eBrem process).
*/
class track : public node {
};

}

#endif
