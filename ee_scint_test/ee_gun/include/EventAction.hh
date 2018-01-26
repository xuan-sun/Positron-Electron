#ifndef EventAction_h
#define EventAction_h 1

#include "DetectorConstruction.hh"
#include "TrackerHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <G4Event.hh>

#include <time.h>

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcommand.hh>
#include <G4UIcmdWithAString.hh>

class EventAction : public G4UserEventAction, G4UImessenger
{
  public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* evt);
    virtual void EndOfEventAction(const G4Event* evt);

    virtual void SetNewValue(G4UIcommand* command, G4String newValue);

    void SetTrappedTrue() { fTrapped = true; };

    clock_t GetStartTime() { return fStartTime; };

  private:
    const DetectorConstruction* fMyDetectorConstruction;

    bool  fTrapped;             // check if event was killed due to being trapped

    clock_t fStartTime;		// time.h uses to define 'trapped' ptcl's & kill them

    G4String fSDNames[fNbSDs];
    G4int fHitsCollectionIDs[fNbSDs]; 	// note: fNbSDs is defined in DetectorConstruction.hh
					// it's a global const variable.
					// Since #include "..." copies the code, this should (and is) fine.
    int fScintEast_index;
    int fScintWest_index;
    int fActiveWireVolEast_index;
    int fActiveWireVolWest_index;

    // UI variables. Only needed for output file name (and/or path)
    G4UIdirectory* uiEventDir;

    G4UIcmdWithAString* uiOutputFileCmd;
    G4String sOutputFileName;


};

#endif


