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
  * @param[in] name base name to write out data
  */
  CGG4Interface(G4String & name)
    :base_name_(name)
  { }


  // --- run level actions ---

  /**
  * @brief write out run
  * @param[in] run_number the run number to append to base_name for the output file name.
  */
  virtual void write_collection(unsigned run_number);


  /**
  * @brief merge runs (merge data from threads).
  */
  virtual void merge();


  // --- step level actions --- 

  /**
  * @brief process a step
  * @param[in] step The G4Step to process and update the event graph
  */
  virtual void process_step(G4Step * step);


  // --- event level actions ---

  /**
  * @brief start a new event
  */
  virtual void start_event();
 

  // --- getters ---
  /**
  * @brief get base name
  */
  virtual const G4string & get_base_name() const { return base_name_; }

  /**
  * @brief get the number of events/graphs
  */
  virtual size_t size() const;

  /**
  * @brief get the node_collection of showers/events
  */
  virtual node_collection & collection();
  

  /**
  * @brief get the shower graph of a particular event.
  * @param\[in] i the event number 
  */
  virtual node * node(const unsigned i);


  // --- setters ---
  /**
  * @brief set the base name
  */
  virtual void set_base_name(const G4String &name ) { base_name_ = name; }


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
