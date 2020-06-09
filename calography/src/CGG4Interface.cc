
#include "CGG4Interface.h"

#include <sstream>

#include "G4Types.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4Threading.hh"



namespace { G4Mutex cgMutex = G4MUTEX_INITIALIZER; }


cg::node_collection CGG4Interface::event_graphs_  = cg::node_collection();


// write collection
void CGG4Interface::write_collection(unsigned run_number)
{

  // if serial application, or is master thread write data
  if ( !G4Threading::IsMultithreadedApplication() || G4Threading::IsMasterThread() ) {
    // append the run number to the base name
    std::stringstream namestr;
    namestr << base_name_ << run_number << ".cg";
    cg::WriteCollection(graph_events_,namestr.str());
  }

}


// merge runs
void CGG4Interface::merge()
{ 

  // if serial application - do nothing
  // if workder thread, append to static data
  if ( G4Threading::IsWorkerThread() && G4Threading::IsMultithreadedApplication() ) {
    G4AutoLock l(&cgMutex);
    const unsigned ngraphs = local_data_.size();
    for ( unsigned i=0; i != ngraphs; i++ )
      event_graphs_.push_back(local_data_[i]);
  }

}


// process a step
void CGG4Interface::process_step(G4Step * step)
{ 

}


// start a new event
void CGG4Interface::start_event()
{ }

// get the size of the collection
size_t CGG4Interface::size()
{ 

  // if serial application or worker thread, return thread local size
  if ( !G4Threading::IsMultithreadedApplication() || G4Threading::IsWorkerThread() ) {
    return local_data_.size();
  } else if ( G4Threading::IsMasterThread() ) {
    // if master thread, return static collection size.
    G4AutoLock l(&cgMutex);
    return event_graphs_.size();
  } else {
    return 0;
  }

}


// get the collection
cg::node_collection CGG4Interface::collection()
{ 

  // if serial application or worker thread, return thread local collection
  if ( !G4Threading::IsMultithreadedApplication() || G4Threading::IsWorkerThread() {
    return local_data_;
  } else if ( G4Threading::IsMasterThread() ) {
    // if master thread, return static collection.
    G4AutoLock l(&cgMutex);
    return event_graphs_;
  } else {
    return cg::node_collection();
  }

}


// get a particular node
cg::node CGG4Interface::node(const unsigned i)
{ 

  // if serial application or worker thread, return node from thread local 
  if ( !G4Threading::IsMultithreadedApplication() || G4Threading::IsWorkerThread() ) {
    if ( i < local_data_.size() ) return local_data_[i];
    else assert(false);  // throw assertion error if i is out of range
  } else if ( G4Threading::IsMasterThread() ) {
    // if master thread, return node from static data
    G4AutoLock l(&cgMutex);
    if ( i < event_graphs_.size() ) return event_graphs_[i];
    else assert(false);  // throw assertion error if i is out of range
  } else {
    return cg::node;
  }
 
}


