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
  node() { }

  /**
  * @brief copy constructor
  */
  node(const node& nd) { }

  /**
  * @brief destructor
  */
  virtual ~node() { }

  /**
  * @brief Add child node.
  */

  // --- serialization methods ---
  /**
  * @brief serialize
  * @param[in] stream The output stream into which to write this node.
  */
  virtual void serialize(std::ostream & stream);

  /**
  * @brief deserialize
  * @param[in] stream The input stream from which to read this node.
  */
  virtual void deserialize(std::istream & stream);

  /**
  * @brief insertion operator
  * This method can be used to insert the sub-graph below this node into a stream.
  */
  friend std::ostream& operator<<(std::ostream&, node &);

  /**
  * @brief extraction operator
  * @details This method can be used to extract the sub-graph below this node from a stream.
  */
  friend std::istream& operator>>(std::istream&, node &); 


  // ---  accessor methods ---

  /**
  * @brief print some basic information about this node/sub-graph.
  * @details print the details of this node, and its children to stdout.
  * @param[in] lvl The depth in the graph of this node. 
  */
  virtual void print(int lvl=0 ) const;


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
  virtual double totalenergy();

  /**
  * @brief Get steps (tracks) that deposit energy for further analysis
  * in the sub-graph.
  * @return The collection of energy depositing steps/tracks in the sub-graph.
  */
  virtual std::vector<const node*> shower() const; 


protected:

private:

}; 

}

#endif
