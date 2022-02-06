

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "CaloGraphy.h"
#include "CaloGraphyIO.h"


void dump_graph(std::ofstream & strm, cg::node * nd, int parent){

  // dump information about the node
  // get the type
  auto tp = nd->type();
  auto id = nd->id();
  strm << "  " << id;
  if ( tp == cg::trackNode ) { 
    strm << " [ label=\"" << ((cg::track*)nd)->G4TrackID() << " pdg(" 
      << ((cg::track*)nd)->pdg() 
      << ")\" " << " shape=\"diamond\" ];";
  } else if ( tp == cg::processNode ) {
    strm << " [label=\"" << ((cg::process*)nd)->name() << "\" shape=\"ellipse\" ];";
  } else {
    strm << " label=\"" << id << "\" shape=\"box\" ];";
  }

  strm << std::endl;

  // connection from parent
  if ( parent != -1 )
    strm << "  " << parent << " -> " << id << std::endl;

  // go through the children
  const std::vector<cg::node *> & children = nd->children();
  const unsigned nkids = children.size();
  for ( unsigned i=0; i != nkids; i++ ){
    dump_graph(strm,children[i],id);
  }

}


int main(int argc, char **argv) {

  if ( argc != 3 ) {
    std::cout << "cgvis <file-name> <event-number>" << std::endl;
    return 0; 
  }

  std::string fileName(argv[1]);
  int evnum = atoi(argv[2]);

  // read the collection
  cg::node_collection col = cg::ReadCollection(fileName);

  assert(evnum < col.size());

  // load the graph
  cg::track * grph = (cg::track*)col[evnum];

  // start the graphviz image
  // open the file
  std::ofstream gv;
  gv.open(fileName + std::string(".gv"));
  
  // write the header stuff
  gv << "digraph shower {" << std::endl;

  // dump the graph
  dump_graph(gv,grph,-1);

  // end the graph
  gv << "}" << std::endl;

  // close the file 
  gv.close();
   

  return 0;
}
