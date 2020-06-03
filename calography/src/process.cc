
#include "process.h"

#include <iostream>


// print
void cg::process::print(int lvl) const {
  std::cout << std::string(lvl,' ') << "Process "
    << id_ << " " << procName_ << " " << energy_ << " Pos(" << pos_ << ")" << std::endl;

  const unsigned nkids = children_.size();
  for ( unsigned i=0; i != nkids; i++ ) {
    children_[i]->print(lvl+1);
  }   
}


// serialize
void cg::process::serialize(std::ostream & stream) const {
  stream << id_ << " " << type_ << " " << procName_ << " " << energy_ << " " << pos_ << " ";

  const unsigned nkids = children_.size();
  stream << nkids << " ";
  for ( unsigned i=0; i != nkids; i++ ) {
    children_[i]->serialize(stream);
  }
}


// deserialize
void cg::process::deserialize(std::istream & stream) {
  unsigned tmptype;
  stream >> id_ >> tmptype >> procName_ >> energy_ >> pos_;
  type_ = static_cast<cg::node_type>(tmptype);

  deserialize_children(stream);
}

