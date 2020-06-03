#ifndef PROCESS_H
#define PROCESS_H

/**
* @file process.h
* @author C S Cowden
* @brief Declare a process node for the graph
*/

// --- includes ---
#include "node.h"

#include <string>

// #include "G4ProcessType.hh"

namespace cg {

/**
* @brief Process class - Node
* @details The process node captures a physics process implemented and limiting a step in a Geant4 simulation.
* Analyzing the graph and summing energy deposition and physical extents of the graph below
* a process can be used to understand the shape of sub-showers within a calorimeter.
*/
class process : public node {
public:

  /** 
  * @brief default constructor
  */
  process()
    :node(processNode)
  { }

  /**
  * @brief copy constructor
  */
  process(const process & proc)
    :node(proc)
  { }

  /**
  * @brief construct with some data.
  */
   process(const std::string name, double E, const relvec& rc)
    :node(processNode,E,rc)
    ,procName_(name)
  { }
  

  /**
  * @brief destructor
  */


  /** 
  * @brief print some basic information about this node.
  */
  virtual void print(int lvl=0) const; 



  /**
  * @brief serialize the process node
  */
  virtual void serialize(std::ostream &) const;

  /**
  * @brief deserialize the process node
  */
  virtual void deserialize(std::istream &);


  // --- new setters
  /**
  * @brief set the process name.
  */
  virtual void set_name(const std::string & name) { procName_ = name; }


  // --- new getters
  /**
  * @brief get the process name
  */
  const virtual std::string & name() const { return procName_; }


protected: 

  // process name
  std::string procName_;

}; 

}

#endif
