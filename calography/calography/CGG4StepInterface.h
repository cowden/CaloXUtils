#ifndef CGG4STEPINTERFACE_H
#define CGG4STEPINTERFACE_H

/**
* @file CGG4StepInterface.h
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
* @brief Geant4 Step Interface.
* @details This interface builds a shower graph from processing many
* G4Steps.  It is expected that secondaries are processed on the stack.
* Simulations involving optical physics processes may violate this assumption
* since tracks are suspended when enough optical photons are generated.
* This class can be called from a user derived G4SteppingAction.
*/
class CGG4StepInterface {
public:

  /**
  * @brief constructor
  */
  CGG4Interface() { }


  /**
  * @brief start a new event
  */
  

  /**
  * @brief process a step
  */



  // --- getters ---

  // --- setters ---


private:
 
  node * nd_;
  

};

}

#endif
