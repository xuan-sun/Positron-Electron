#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <iostream>
#include <fstream>
#include <math.h>	// NOTE: if you include these ROOT classes below, you'll get a ton of compiler warnings
#include <cmath>	// That is because, as far as I know, ROOT and GEANT4 use the same global variable names
#include <string>	// so they have a ton of "shadow declarations" of each other.
			// Should be ok since ideally they are in their own workspace.
			// these are the only instances of ROOT classes.
			// they compile fine on my machine. But if you are having trouble
			// can remove them and use them as a standalone. These guys only
			// appear in ConvertTreeToTxt(...). Can do this before sim if ROOT is tricky

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* myDC)
: G4VUserPrimaryGeneratorAction(), G4UImessenger(),
  fParticleGun(0),
  fMyDetector(myDC),
  fSourceRadius(3.*mm),	// this is default set. If you aren't using DiskRandom, don't care.
  fPosOffset(G4ThreeVector(0,0,0)),	// base positioning offset. Is non-zero only if main Decay Trap is offset (it is not)
  bIsLoaded(false)
{
  //----- Below is messenger class

  uiGenDir = new G4UIdirectory("/particleGun/");
  uiGenDir -> SetGuidance("/primaryGeneratorAction control");

  uiInputFileCmd = new G4UIcmdWithAString("/particleGun/inputName", this);
  uiInputFileCmd -> SetGuidance("Set the input file name of the primaries.");
  uiInputFileCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);
  sInputFileName = "none.txt";

  uiOutputFileCmd = new G4UIcmdWithAString("/particleGun/outputName", this);
  uiOutputFileCmd -> SetGuidance("Set the output file name of the primaries.");
  uiOutputFileCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);
  sOutputFileName = "none.txt";

  uiParticleGenTypeCmd = new G4UIcmdWithAString("/particleGun/ptclType", this);
  uiParticleGenTypeCmd -> SetGuidance("Set the particle type of the primaries.");
  uiParticleGenTypeCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);
  sPtclType = "Sn113";	// default to firing and recording tin. Can change to anything

  //----- Above is messenger class

  G4int nPtcls = 1;
  fParticleGun = new G4ParticleGun(nPtcls);

  // At the end of constructor, GEANT4 default calls GeneratePrimaries method
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == uiInputFileCmd)
  {
    sInputFileName = G4String(newValue);
    G4cout << "Setting input primary particles file to " << sInputFileName << G4endl;
  }
  else if(command == uiOutputFileCmd)
  {
    sOutputFileName = G4String(newValue);
    G4cout << "Setting output primary particles information file to " << sOutputFileName << G4endl;
  }
  else if(command == uiParticleGenTypeCmd)
  {
    sPtclType = G4String(newValue);
    G4cout << "Firing particles of type " << sPtclType << G4endl;
  }
  else
    G4cout << "COMMAND DOES NOT MATCH PRIMARY GENERATOR ACTION OPTIONS." << G4endl;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // use GEANT4 event id to track which part of fEvtsArray we will use for generated event
  int evtID = anEvent -> GetEventID();

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle;

  fParticleGun -> SetParticleEnergy(321*keV);
  particle = particleTable->FindParticle(particleName="e+");

  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleTime(0.0*ns);        // Michael's has this line. Idk why.

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1));
  fParticleGun->SetParticlePosition(G4ThreeVector(0,0,0));

  fParticleGun -> GeneratePrimaryVertex(anEvent);
}

void PrimaryGeneratorAction::SavePrimPtclInfo2(int eventID)
{
  ofstream outfile;
  outfile.open(sOutputFileName, ios::app);
  outfile << eventID << "\t"
	  << fParticleGun->GetParticleDefinition()->GetPDGEncoding() << "\t"
	  << fParticleGun->GetParticleEnergy()/keV << "\t"
	  << fParticleGun->GetParticlePosition()[0]/m << "\t"
	  << fParticleGun->GetParticlePosition()[1]/m << "\t"
	  << fParticleGun->GetParticlePosition()[2]/m << "\t"
	  << fParticleGun->GetParticleMomentumDirection()[0] << "\t"
	  << fParticleGun->GetParticleMomentumDirection()[1] << "\t"
	  << fParticleGun->GetParticleMomentumDirection()[2] << "\t"
	  << fParticleGun->GetParticleTime()/ns << "\t"
	  << "1 \t";	// this is done since all weights are 1
  outfile.close();

}
