
#include "node.h"

#include <iostream>


// instantiate the static node count
unsigned cg::node::nNodes_ = 0U;


// serialize
void cg::node::serialize(std::ostream & stream) const {
  stream << id_ << " " << energy_ << " " << pos_ << " ";

  const unsigned nkids = children_.size();
  stream << nkids << " ";
  for ( unsigned i=0; i != nkids; i++ ) {
    children_[i].serialize(stream);
  }
}

// deserialize
void cg::node::deserialize(std::istream & stream) {
  stream >> id_ >> energy_ >> pos_;

  size_t num;
  stream >> num;
  for ( unsigned i=0; i != num; i++ ) {
    node nd;
    nd.deserialize(stream);
    children_.push_back(nd);
  }
}


// print
void cg::node::print(int lvl) const {
  std::cout << std::string(lvl,' ') << "Node "
    << id_ << " " << energy_ << " Pos(" << pos_ << ")" << std::endl;
  
  const unsigned kids = children_.size();
  for ( unsigned i=0; i != kids; i++ ) {
    children_[i].print(lvl+1);
  }
}

// provenance
std::vector<const cg::node*> cg::node::provenance() const {
  return std::vector<const cg::node*>(0);
}

// total energy
double cg::node::totalenergy() const{
  return 0;
}

// shower
std::vector<const cg::node*> cg::node::shower() const {
  return std::vector<const cg::node*>(0);
}

// get node
cg::node * cg::node::find(const unsigned id) {
}


// set the position
void cg::node::set_pos(const relvec & pos) {
  pos_ = pos;
}

// set the energy
void cg::node::set_energy(const float E) {
  energy_ = E;
}


