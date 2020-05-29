#ifndef NODE_H
#define NODE_H

/**
* @file node.h
* @author C S Cowden
* @brief Declare the `node` class.
*/

// ----- includes ------
#include <ostream>
#include <istream>
#include <vector>

#include "relvec.h"

namespace cg {


/**
* @brief Abstract node class
* 
*/
class node {
public:

  /**
  * @brief default constructor
  */
  node():id_(nNodes_++) { }

  /**
  * @brief construct with data.
  * @param[in] E energy of the node
  * @param[in] pos the position of the node.
  */
  node(double E, const relvec& rc)
    :id_(nNodes_++)
    ,energy_(E)
    ,pos_(rc)
  { }

  /**
  * @brief copy constructor
  */
  node(const node& nd)
    :id_(nd.id_)
    ,energy_(nd.energy_)
    ,pos_(nd.pos_) 
    ,children_(nd.children_)
  { }

  /**
  * @brief destructor
  */
  virtual ~node() { }

  /**
  * @brief Add child node.
  */
  virtual void add_child(node & nd) {
    children_.push_back(nd);
  }

  // --- serialization methods ---
  /**
  * @brief serialize
  * @param[in] stream The output stream into which to write this node.
  */
  virtual void serialize(std::ostream & stream) const;

  /**
  * @brief deserialize
  * @param[in] stream The input stream from which to read this node.
  */
  virtual void deserialize(std::istream & stream);

  /**
  * @brief insertion operator
  * This method can be used to insert the sub-graph below this node into a stream.
  */
  inline friend std::ostream& operator<<(std::ostream& stream, const node & nd) {
    nd.serialize(stream);
    return stream;
  }

  /**
  * @brief extraction operator
  * @details This method can be used to extract the sub-graph below this node from a stream.
  */
  inline friend std::istream& operator>>(std::istream& stream, node & nd) {
    nd.deserialize(stream);
    return stream;
  }


  // ---  accessor methods ---

  /**
  * @brief print some basic information about this node/sub-graph.
  * @details print the details of this node, and its children to stdout.
  * @param[in] lvl The depth in the graph of this node. 
  */
  virtual void print(int lvl=0 ) const;


  /**
  * @brief Get the id of this node.
  */
  virtual unsigned id() const { return id_; }

  /**
  * @brief Get the position of this node.
  */
  virtual const relvec & pos() const { return pos_; }

  /**
  * @brief Get the energy.
  */
  virtual float energy() const { return energy_; }


  // --- analysis methods ---

  /**
  * @brief Find the provenance of this node.
  * @return Vector of pointers to nodes above this current node. 
  * The vector is stored in the reverse order of the graph, so the 
  * root node is last.
  */
  virtual std::vector<const node*> provenance() const;


  /**
  * @brief Sum the deposited energy in this sub-graph.
  * @return The sum of the (readable) energy from the sub-graph.
  */
  virtual double totalenergy() const;

  /**
  * @brief Get steps (tracks) that deposit energy for further analysis
  * in the sub-graph.
  * @return The collection of energy depositing steps/tracks in the sub-graph.
  */
  virtual std::vector<const node*> shower() const; 


  /**
  * @brief look up a specific node.
  * @param[in] id The node id.
  * @return A pointer to the node (NULL if the node is not found).
  */
  virtual node * find(const unsigned id);


  // --- setter methods ---
  /**
  * @brief Set the position of the node.
  * @param[in] pos The new position (4-space) of the node.
  */
  virtual void set_pos(const relvec &);

  /**
  * @brief Set the energy of the node.
  * @param[in] E The new energy of the node.
  */
  virtual void set_energy(const float E);

protected:

  // node id
  unsigned id_;

  // energy
  float energy_;

  // location
  relvec pos_;

  // children of this node
  std::vector<node> children_;

private:

  /// 
  /// node count, use this to increment each time
  /// a new node is created.  This will ensure
  /// each node has a unique id.
  static unsigned nNodes_;

}; 

}



#endif
