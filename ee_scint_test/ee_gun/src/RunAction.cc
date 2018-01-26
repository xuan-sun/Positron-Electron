#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <iostream>
#include <fstream>
#include <math.h>
#include <cmath>
using   namespace       std;

RunAction::RunAction()
: G4UserRunAction()
{ }


RunAction::~RunAction()
{}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  fKillCount = 0;

  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}


void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run----------------------- \n";
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------ \n";
  }

  G4cout << "Number of trapped events killed: " << fKillCount << G4endl;
}
