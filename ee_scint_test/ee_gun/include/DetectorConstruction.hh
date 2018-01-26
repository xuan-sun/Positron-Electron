#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "TrackerSD.hh"
#include "DetectorConstructionUtils.hh"
#include "ScintillatorConstruction.hh"

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include <G4ElectroMagneticField.hh>	// Taken from WirechamberConstruction.
#include <G4MagneticField.hh>
#include <G4RotationMatrix.hh>

#include <G4UImessenger.hh>             // Taken from DetectorConstruction.hh M.M's
#include <G4UIdirectory.hh>
#include <G4UIcommand.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAString.hh>


#include <string>
#include <sstream>

const int fNbSDs = 1;

class G4VPhysicalVolume;
class G4LogicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction, G4UImessenger, MaterialUser
{
  public:
    DetectorConstruction();		// Constructor/destructors
    virtual ~DetectorConstruction();
    virtual G4VPhysicalVolume* Construct();

    virtual void SetNewValue(G4UIcommand * command,G4String newValue);  // UI communicator

    G4LogicalVolume* experimentalHall_log;
    G4VPhysicalVolume* experimentalHall_phys;

    ScintillatorConstruction Scint[1];
    G4VPhysicalVolume* scint_phys[1];

    G4String fSDNamesArray[fNbSDs];	// needs to be public since EventAction will access all elements
    G4String fHCNamesArray[fNbSDs];

  private:
    TrackerSD* RegisterSD(G4String sdName, G4String hcName);

    TrackerSD* SD_scint_scintillator[2];	// all the SD objects that will be used
    TrackerSD* SD_scint_deadScint[2];
    TrackerSD* SD_scint_backing[2];
    TrackerSD* SD_world;

    // User Interface commands from .mac files
    G4UIdirectory* uiDetectorDir;       // UI directory for detector-related commands

    G4UIcmdWithAString* uiDetectorGeometryCmd;  // which detector geometry to construct
    G4String sGeometry;

    G4UIcmdWith3VectorAndUnit* uiDetOffsetCmd;  // symmetrical detector offset from center origin
    G4ThreeVector vDetOffset;

    G4UIcmdWithADouble* uiDetRotCmd;            // symmetrical detector rotation angle around Z axis (radians, hence no units)
    G4float fDetRot;

    G4UIcmdWithADoubleAndUnit* uiVacuumLevelCmd;        // SCS bore vacuum
    G4float fVacuumPressure;

    G4UIcmdWithADoubleAndUnit* uiScintStepLimitCmd;     // step size limiter in scintillator
    G4float fScintStepLimit;

    // some of my own tools to help with DetectorConstruction
    int fStorageIndex;
    G4float fCrinkleAngle;
};

#endif

