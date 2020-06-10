#ifndef CALOGRAPHYIO_H
#define CALOGRAPHYIO_H


/**
* @file CaloGraphyIO.h
* @author C S Cowden
* @brief Implement IO functions.
*/

// --- includes ---
#include <vector>
#include <string>
#include <fstream>
#include <cassert>

#include "node.h"
#include "process.h"
#include "track.h"
#include "nodetypes.h"


namespace cg {

/**
* @brief define collection of graphs
*/
typedef std::vector<node *> node_collection;

/**
* @brief Write a collection to a file.
*/
inline void WriteCollection(const node_collection & nc, const std::string & name) {
  // open a file
  std::ofstream out;
  out.open(name);

  // cycle over the collection
  const unsigned ncs = nc.size();
  for ( unsigned i=0; i != ncs; i++ ) {

    // write to the stream
    out << nc[i];

  }

  // close the file
  out.close();
}


/**
* @brief Write a graph to a file.
*/
inline void WriteGraph(const node * nd, const std::string & name) {
  // open a file
  std::ofstream out;
  out.open(name);

  // write to the stream
  out << nd;

  // close the file
  out.close();
}

/**
* @brief Extract a node from a stream.
*/
inline node * extract_node(std::istream & stream) {

  // peek ahead to determine the type of node
  unsigned pos = stream.tellg();
  unsigned id;
  unsigned tmptype;
  stream >> id >> tmptype;
  node_type type = static_cast<node_type>(tmptype);

  // step back in stream
  stream.seekg(pos);

  // instantiate and extract the node
  node *nd;

  if ( type == genericNode ) {
    nd = new node;
  } else if ( type == processNode ) {
    nd = new process;
  } else if ( type == trackNode ) {
    nd = new track;
  } else {
    assert(false);
  }

  nd->deserialize(stream);

  // return the node
  return nd;
}

/**
* @brief Read a collection from a file.
*/
inline node_collection ReadCollection(const std::string & name ) {
  // open the file
  std::ifstream in;
  in.open(name);

  // create a node collection
  node_collection nc;

  // keep reading graphs until we reach the end of the file
  while ( !in.eof() && in.good() ) {
    node * nd = extract_node(in);
    nc.push_back(nd);
  }

  // close the file
  in.close();

  // 
  return nc;
  
}

/**
* @brief Read a graph freom a file.
*/
inline node * ReadGraph(const std::string & name ) {
  // open the file
  std::ifstream in;
  in.open(name);

  node * nd = extract_node(in);

  // close the file
  in.close();

  // return the node
  return nd;

}

}

#endif
