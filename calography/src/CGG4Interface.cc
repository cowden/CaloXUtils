
#include "CGG4Interface.h"

#include <sstream>

#include "G4Types.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4Threading.hh"



namespace { G4Mutex cgMutex = G4MUTEX_INITIALIZER; }


cg::node_collection cg::CGG4Interface::event_graphs_  = cg::node_collection();


// write collection
void cg::CGG4Interface::write_collection(unsigned run_number)
{

  // if serial application, or is master thread write data
  if ( !G4Threading::IsMultithreadedApplication() || G4Threading::IsMasterThread() ) {
    // append the run number to the base name
    std::stringstream namestr;
    namestr << base_name_ << run_number << ".cg";
    cg::WriteCollection(event_graphs_,namestr.str());
  }

}


// merge runs
void cg::CGG4Interface::merge()
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
void cg::CGG4Interface::process_step(G4Step * step)
{ 

  //  get the track information
  auto track = step->GetTrack();
  auto id = track->GetTrackID();
  auto eDep = step->GetTotalEnergyDeposit();
       
  // find the track in the graph
  // keep a stack of tracks as well to quickly look this up
  cg::track * theNode = stack_.pop();

  // check the track id
  assert(theNode->G4TrackID() == id);

  // update the energy lost in the step
  theNode->set_energy(eDep);
  

  // get the end process of the step
  auto post = step->GetPostStepPoint();
  auto proc = post->GetProcessDefinedStep();
  auto procname = proc->GetProcessName();

  auto t = post->GetGlobalTime();
  auto pos3 = post->GetPosition();
  cg::relvec pos(t,pos3.x(),pos3.y(),pos3.z());

  //  attach the process
  cg::process * procNode = new cg::process(procname,0.,pos);
  theNode->add_child(procNode); 
  

  // process secondaries
  auto secondaries = step->GetSecondariesInCurrentStep();
  auto nsec = secondaries->size();
  for ( unsigned i=0; i != nsec; i++ ){
    // get secondary information
    auto sectrk = (*secondaries)[i];
    auto secid = id + i;
    auto sectrk->GetParticleDefinition()->GetPDGEncoding();

    // create secondary track nodes
    // put track node on stack
    // 

    cg::track * subNode = new cg::track;
    procNode->add_child(subNode); 

    // get the particle energy/momentum
    //

  }

}




// start a new event
void cg::CGG4Interface::start_event()
{ 
  // 
  cg::node *nd = new cg::node;
  local_data_.push_back(nd);

  current_event_root_ = nd;

  // clear the stack
  stack_ = std::stack<node *>(); 
  stack_.push(nd);
}



// get the size of the collection
size_t cg::CGG4Interface::size() const
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
cg::node_collection & cg::CGG4Interface::collection()
{ 

  // if serial application or worker thread, return thread local collection
  if ( !G4Threading::IsMultithreadedApplication() || G4Threading::IsWorkerThread() ) {
    return local_data_;
  } else if ( G4Threading::IsMasterThread() ) {
    // if master thread, return static collection.
    G4AutoLock l(&cgMutex);
    return event_graphs_;
  } else {
    return local_data_;
  }

}


// get a particular node
cg::node * cg::CGG4Interface::get_node(const unsigned i)
{ 

  // if serial application or worker thread, return node from thread local 
  if ( !G4Threading::IsMultithreadedApplication() || G4Threading::IsWorkerThread() ) {
    if ( i < local_data_.size() ) return local_data_[i];
    else assert(false);  // throw assertion error if i is out of range
  } else {
    // if master thread, return node from static data
    G4AutoLock l(&cgMutex);
    if ( i < event_graphs_.size() ) return event_graphs_[i];
    else assert(false);  // throw assertion error if i is out of range
  } 
 
}


