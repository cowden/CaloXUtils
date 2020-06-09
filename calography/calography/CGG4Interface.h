#ifndef CGG4STEPINTERFACE_H
#define CGG4STEPINTERFACE_H

/**
* @file CGG4Interface.h
* @author C S Cowden
* @brief An interface to build a graph from G4Steps.
*/

// ---- includes -----
#include <vector>
#include <string>

#include "CaloGraphy.h"

// forward declare Geant4 classes
class G4Step;

namespace cg {

/**
* @brief Geant4 Interface.
* @details This interface builds a shower graph from processing many
* G4Steps.  It is expected that secondaries are processed on the stack.
* Simulations involving optical physics processes may violate this assumption
* since tracks are suspended when enough optical photons are generated.
* This class can be called from a user derived G4SteppingAction.
*/
class CGG4Interface {
public:

  /**
  * @brief constructor
  */
  CGG4Interface() { }

  /**
  * @brief construct with a name
  */


  // --- run level actions ---

  /**
  * @brief add event
  */

  /**
  * @brief write out run
  */


  /**
  * @brief merge runs (merge data from threads).
  */


  // --- step level actions --- 

  /**
  * @brief process a step
  */


  // --- event level actions ---

  /**
  * @brief start a new event
  */
 


  // --- getters ---
  /**
  * @brief get base name
  */

  /**
  * @brief get the number of events/graphs
  */

  /**
  * @brief get the node_collection of showers/events
  */

  /**
  * @brief get the shower graph of a particular event.
  */

  // --- setters ---
  /**
  * @brief set the base name
  */


private:
 

  // --------------------------------
  // thread local storage
  node_collection local_data_;

  // output file base name
  std::string base_name_;

  // ----------------------------------
  // static master collection
  static node_collection event_graphs_;

};

}

#endif
