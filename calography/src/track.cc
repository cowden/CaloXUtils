
#include "track.h"

#include <iostream>

// print
void cg::track::print(int lvl) const {
  std::cout << std::string(lvl,' ') << "Track "
    << id_ << " " << g4trackid_ << " pdg(" << pdgid_ << ") E = " << energy_
    << " X(" << pos_ << ")  P(" << momentum_ << ")" << std::endl;

  const unsigned nkids = children_.size();
  for ( unsigned i=0; i != nkids; i++ ) {
    children_[i]->print(lvl+1);
  }
}


// serialize
void cg::track::serialize(std::ostream & stream) const { 
  stream << id_ << " " << type_ << " " << pdgid_ << " " << g4trackid_ 
    << " " << energy_ << " " << pos_ << " " << momentum_ << " ";

  const unsigned nkids = children_.size();
  stream << nkids << " ";
  for ( unsigned i=0; i != nkids; i++ ) {
    children_[i]->serialize(stream);
  }
}


// deserialize
void cg::track::deserialize(std::istream & stream) {
  unsigned tmptype;
  stream >> id_ >> tmptype >> pdgid_ >> g4trackid_ >> energy_ >> pos_ >> momentum_;
  type_ = static_cast<cg::node_type>(tmptype);

  deserialize_children(stream);
}
