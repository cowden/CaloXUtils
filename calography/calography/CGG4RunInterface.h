#ifndef CGG4RUNINTERFACE_H
#define CGG4RUNINTERFACE_H

/**
* @file CGG4RunInterface.h
* @author C S Cowden
* @brief An interface to collect graphs in G4 Runs.
*/

// --- includes ---


namespace cg {

/**
* @brief Geant4 Run Interface.
* @details This interface collects shower graphs from
* events in a run.
*/
class CGG4RunInterface {
public:

  /**
  * @brief constructor
  */


  /**
  * @brief construct with a name
  */

  /**
  * @brief add event
  */

  /**
  * @brief write out run
  */


  // ---- getters ---
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

  // output file base name
  std::string base_name_;


  // collection of showers/events
  node_collection event_graphs_;

};


}
#endif
