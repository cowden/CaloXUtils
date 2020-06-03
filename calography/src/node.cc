
#include "node.h"

#include <cassert>
#include <iostream>

// COCHISE
// need these for now to deserialize the graph.
// remove if a separate factory is implemented.
#include "process.h"
#include "track.h"



// instantiate the static node count
unsigned cg::node::nNodes_ = 0U;

// destructor
cg::node::~node() {
  const unsigned nkids = children_.size();
  for ( unsigned i=0; i != nkids; i++ )
    delete children_[i];
}


// serialize
void cg::node::serialize(std::ostream & stream) const {
  stream << id_ << " " << (unsigned)type_ << " " << energy_ << " " << pos_ << " ";

  const unsigned nkids = children_.size();
  stream << nkids << " ";
  for ( unsigned i=0; i != nkids; i++ ) {
    children_[i]->serialize(stream);
  }
}

// deserialize
void cg::node::deserialize(std::istream & stream) {
  unsigned tmptype;
  stream >> id_ >> tmptype >> energy_ >> pos_;
  type_ = static_cast<node_type>(tmptype);

  deserialize_children(stream);

}

// deserialze the child nodes
void cg::node::deserialize_children(std::istream & stream) {

  size_t num;
  stream >> num;
  for ( unsigned i=0; i != num; i++ ) {

    // peek ahead to determine the type of the next node
    unsigned stream_pos = stream.tellg();    
    unsigned id;
    unsigned tmptype;
    stream >> id >> tmptype;
    node_type type = static_cast<node_type>(tmptype); 

    // go back to deserialize
    stream.seekg(stream_pos);

    cg::node * nd;
  
    if ( type ==  cg::genericNode ) {
      nd = new cg::node;
      nd->deserialize(stream);
    } else if ( type == cg::processNode ) {
      nd = new cg::process;
      nd->deserialize(stream);
    } else if ( type == cg::trackNode ) {
      nd = new cg::track;
      nd->deserialize(stream);
    } else {
      assert(false);
    }

    children_.push_back(nd);
  }




}


// print
void cg::node::print(int lvl) const {
  std::cout << std::string(lvl,' ') << "Node "
    << id_ << " " << energy_ << " Pos(" << pos_ << ")" << std::endl;
  
  const unsigned kids = children_.size();
  for ( unsigned i=0; i != kids; i++ ) {
    children_[i]->print(lvl+1);
  }
}

// provenance
std::vector<const cg::node*> cg::node::provenance() const {
  return std::vector<const cg::node*>(0);
}

// total energy
double cg::node::totalenergy() const {
  return 0;
}

// shower
std::vector<const cg::node*> cg::node::shower() const {

  // create a vector
  std::vector<const cg::node *> nodes;

  // explore the graph
  this->shower(nodes);

  // return the vector
  return nodes;
}

// shower (append to data)
void cg::node::shower(std::vector<const node *> & nodes) const {

  // append this node
  nodes.push_back(this);

  // explore children
  const unsigned nkids = children_.size();
  for ( unsigned i=0; i != nkids; i++ )
    children_[i]->shower(nodes);

}

// get node
cg::node * cg::node::find(const unsigned id) {

  // check this node
  if ( id_ == id )
    return this;

  else {
    // explore children
    const unsigned nkids = children_.size();
    for ( unsigned i=0; i != nkids; i++ ) {
      cg::node * nd = children_[i]->find(id);
      if ( nd ) 
        return nd;
    }
  }

  // return NULL
  return NULL;
}


// set the position
void cg::node::set_pos(const relvec & pos) {
  pos_ = pos;
}

// set the energy
void cg::node::set_energy(const float E) {
  energy_ = E;
}


