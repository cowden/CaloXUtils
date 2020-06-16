#ifndef TRACK_H
#define TRACK_H

/**
* @file track.h
* @author C S Cowden
* @brief Declare the track class (inherit from node).
*/

// --- includes ---
#include "node.h"

#include <string>

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
public:

  /**
  * @brief default constructor
  */
  track()
    :node(trackNode)
  { }

  /**
  * @brief copy constructor
  */
  track(const track &trk)
    :node(trk)
    ,pdgid_(trk.pdgid_)
    ,g4trackid_(trk.g4trackid_)
    ,momentum_(trk.momentum_)
  { }

  /**
  * @brief construct with some data.
  */
  track(int pdg, unsigned g4trackid, const relvec &mom, double E, const relvec& rc)
    :node(trackNode,E,rc)
    ,pdgid_(pdg)
    ,g4trackid_(g4trackid)
    ,momentum_(mom)
  { }


  /**
  * @brief print some basic information about this node.
  */
  virtual void print(int lvl=0) const;

  /**
  * @brief serialize the track node
  */
  virtual void serialize(std::ostream &) const;

  /**
  * @brief deserialize the track node
  */
  virtual void deserialize(std::istream &);

  // --- new setters ---
  /**
  * @brief set the PDG particle id code.
  */
  virtual void set_pdg(int pdg) { pdgid_ = pdg; }

  /**
  * @brief set thew G4 track id.
  */
  virtual void set_g4trackid(unsigned id) { g4trackid_ = id; }

  /**
  * @brief set the 4-momentum of the track.
  */
  virtual void set_momentum(const relvec & mom) { momentum_ = mom; }

  // --- new getters ---
  /**
  * @brief get the PDG particle id code.
  */
  virtual int pdg() const { return pdgid_; }

  /**
  * @brief get the G4 track id
  */
  virtual unsigned G4TrackID() const { return g4trackid_; }

  /**
  * @brief get the 4-momentum of the particle
  */
  virtual const relvec & momentum() const { return momentum_; }


protected:

  // identify the particle type by pdg code
  int pdgid_;

  // identify a G4 track
  unsigned g4trackid_;

  // particle momentum
  relvec momentum_;

  
};

}

#endif
