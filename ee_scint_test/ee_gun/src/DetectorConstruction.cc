#include "DetectorConstruction.hh"

#include <G4UserLimits.hh>		// stole from Michael Mendenhall's code.

#include <Randomize.hh>			// Stolen from Analysis Manager
#include <G4ios.hh>			// Pretty sure needed for TrackerSD
#include <G4Run.hh>			// Leave them here since we use registerSD in DetectorConstruction
#include <G4Event.hh>			// And the registerSD is totally not working without it
#include <G4Track.hh>
#include <G4VVisManager.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4IonTable.hh>
#include <G4SDManager.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4SDManager.hh>
#include <G4EventManager.hh>

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{
  // initialize some useful private class variables
  fStorageIndex = 0;    // this loops over our TrackerHit names storage array
  fCrinkleAngle = 0;

  // Here in the constructor we will create everything related to a "messenger" class
  uiDetectorDir = new G4UIdirectory("/detector/");
  uiDetectorDir -> SetGuidance("/detector control");

  uiDetectorGeometryCmd = new G4UIcmdWithAString("/detector/geometry",this);
  uiDetectorGeometryCmd -> SetGuidance("Set the geometry of the detector");
  uiDetectorGeometryCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);
  sGeometry = "C";      // this sets a default

  uiDetOffsetCmd = new G4UIcmdWith3VectorAndUnit("/detector/offset",this);
  uiDetOffsetCmd -> SetGuidance("antisymmetric offset of detector packages from central axis");
  uiDetOffsetCmd -> SetDefaultValue(G4ThreeVector());
  uiDetOffsetCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);
  vDetOffset = G4ThreeVector();

  uiDetRotCmd = new G4UIcmdWithADouble("/detector/rotation",this);
  uiDetRotCmd -> SetGuidance("Antisymmetric rotation of detector packages around z axis");
  uiDetRotCmd -> SetDefaultValue(0.);
  uiDetRotCmd -> AvailableForStates(G4State_PreInit);
  fDetRot = 0.;

  uiVacuumLevelCmd = new G4UIcmdWithADoubleAndUnit("/detector/vacuum",this);
  uiVacuumLevelCmd -> SetGuidance("Set SCS vacuum pressure");
  fVacuumPressure = 0;

  uiScintStepLimitCmd = new G4UIcmdWithADoubleAndUnit("/detector/scintstepsize",this);
  uiScintStepLimitCmd -> SetGuidance("step size limit in scintillator, windows");
  uiScintStepLimitCmd -> SetDefaultValue(1.0*mm);
  fScintStepLimit = 1.0*mm;                     // ...doesn't seem to work (see above comments set)

  experimentalHall_log = NULL;
  experimentalHall_phys = NULL;

}

DetectorConstruction::~DetectorConstruction()
{ }

void DetectorConstruction::SetNewValue(G4UIcommand * command, G4String newValue)
{
  if (command == uiDetectorGeometryCmd)
  {
    sGeometry = G4String(newValue);
  }
  else if (command == uiDetOffsetCmd)
  {
    vDetOffset = uiDetOffsetCmd->GetNew3VectorValue(newValue);
    G4cout << "Setting detector offsets to " << vDetOffset/mm << " mm" << G4endl;
  }
  else if (command == uiDetRotCmd)
  {
    fDetRot = uiDetRotCmd->GetNewDoubleValue(newValue);
    G4cout << "Setting detector rotation to " << fDetRot << " radians" << G4endl;
  }
  else if (command == uiVacuumLevelCmd)
  {
    fVacuumPressure = uiVacuumLevelCmd->GetNewDoubleValue(newValue);
  }
  else if (command == uiScintStepLimitCmd)
  {
    fScintStepLimit = uiScintStepLimitCmd->GetNewDoubleValue(newValue);
    G4cout << "Setting step limit in solids to " << fScintStepLimit/mm << "mm" << G4endl;
  }
  else
  {
    G4cout << "Unknown command:" << command->GetCommandName() << " passed to DetectorConstruction::SetNewValue" << G4endl;
  }
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  SetVacuumPressure(fVacuumPressure);	// this is the set vacuum pressure that was warned about in DefineMaterials()

  // user step limits
  G4UserLimits* UserCoarseLimits = new G4UserLimits();
  UserCoarseLimits->SetMaxAllowedStep(10*m);
  G4UserLimits* UserGasLimits = new G4UserLimits();
  UserGasLimits->SetMaxAllowedStep(1*cm);
  G4UserLimits* UserSolidLimits = new G4UserLimits();
  UserSolidLimits->SetMaxAllowedStep(fScintStepLimit);	// default value from Messenger class.

  // Experimental Hall. World volume.
  G4double expHall_x = 2.0*m;
  G4double expHall_y = 2.0*m;
  G4double expHall_z = 8.0*m;
  G4Box* experimentalHall_box = new G4Box("expHall_box", expHall_x/2, expHall_y/2, expHall_z/2);
  experimentalHall_log = new G4LogicalVolume(experimentalHall_box, Vacuum, "World_log");
  experimentalHall_log -> SetVisAttributes(G4VisAttributes::Invisible);
  experimentalHall_log -> SetUserLimits(UserCoarseLimits);
  experimentalHall_phys = new G4PVPlacement(NULL, G4ThreeVector(), "World_phys", experimentalHall_log, 0, false, 0);


  Scint[0].Build(0);


  scint_phys[0] = new G4PVPlacement(NULL, G4ThreeVector(0,0,1*m), Scint[0].container_log,
                                "scintContainer", experimentalHall_log, false, 0, true);


  //----- Register logical volumes as sensitive detectors. Used for all info tracking during sim
  SD_scint_scintillator[0] = RegisterSD("SD_scint_0", "HC_scint_0");
  Scint[0].scintillator_log -> SetSensitiveDetector(SD_scint_scintillator[0]);

  return experimentalHall_phys;
}

TrackerSD* DetectorConstruction::RegisterSD(G4String sdName, G4String hcName)
{
  TrackerSD* sd = new TrackerSD(sdName, hcName);
  G4SDManager::GetSDMpointer() -> AddNewDetector(sd);

  fSDNamesArray[fStorageIndex] = sdName;
  fHCNamesArray[fStorageIndex] = hcName;
  fStorageIndex++;

  return sd;
}
