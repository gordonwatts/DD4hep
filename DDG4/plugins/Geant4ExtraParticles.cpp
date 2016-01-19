//
// Authors: Tomohiko Tanabe <tomohiko@icepp.s.u-tokyo.ac.jp>
//          Taikan Suehara <suehara@icepp.s.u-tokyo.ac.jp>
// Proted from Mokka by A.Sailer (CERN )
//
// $Id$
// $Name: $

#include "Geant4ExtraParticles.h"
#include "DDG4/Geant4PhysicsConstructor.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Factories.h"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Version.hh"

#include "DDG4/Factories.h"
#include <fstream>
#include <sstream>
#include <string>


#include "G4RunManager.hh"

using namespace DD4hep::Simulation;

Geant4ExtraParticles::Geant4ExtraParticles(Geant4Context* ctxt, const std::string& nam)
  : Geant4PhysicsConstructor(ctxt, nam), m_decay(0), m_ionise(0), m_scatter(0)
{
  declareProperty("pdgfile", m_pdgfile);
}

Geant4ExtraParticles::~Geant4ExtraParticles()   {
  deletePtr(m_decay);
  deletePtr(m_ionise);
  deletePtr(m_scatter);
}

// bool Geant4ExtraParticles::FileExists() {
//   std::ifstream pdgFile( m_pdgfile.c_str(), std::ifstream::in );
//   return pdgFile.is_open();
// }

void Geant4ExtraParticles::constructParticle(Constructor& ) {
  if (m_pdgfile.empty()) return;

  info("pdgfile: %s",m_pdgfile.c_str());

  G4ParticleTable *theParticleTable = G4ParticleTable::GetParticleTable();
  std::ifstream pdgFile( m_pdgfile.c_str(), std::ifstream::in );

  if (!pdgFile.is_open()) {
    except("Could not open pdgfile: %s",m_pdgfile.c_str());
    return;
  }

  info("opened pdgfile: %s",m_pdgfile.c_str());

  while ( !pdgFile.eof() ) {
    // read line
    std::string linebuf;
    getline( pdgFile, linebuf );

    // ignore comments
    if (linebuf.substr(0,1) == "#") continue;
    if (linebuf.substr(0,2) == "//") continue;

    // ignore empty lines
    if (linebuf.empty()) continue;

    // parse line
    int pdg;
    std::string nam;
    double charge;
    double mass;
    double width;
    double lifetime;

    std::istringstream istr(linebuf);

    istr >> pdg >> nam >> charge >> mass >> width >> lifetime;

    // do add particles that don't fly
    // if (lifetime == 0) continue;

    if(width<0) width = 0;

    // normalize to G4 units
    mass *= GeV;

    if (charge != 0) {
      charge /= 3.;
    }

    if (lifetime > 0) {
      lifetime = lifetime*mm/c_light;
    }

    if (width == 0 && lifetime > 0) {
      width = hbar_Planck/lifetime;
    }

    // don't add if the particle already exists
    G4ParticleDefinition* p = theParticleTable->FindParticle(pdg);
    if ( !p ) {

      if (abs(pdg)>80 && abs(pdg)<=100) {
        // don't add generator-specific particles
      } else {
        /*
          if (pdg==5122) {
          G4cout << "Lambda_b0: " << "PDG=" << pdg << ", name=" << name << ", chrg=" << charge
          << ", mass=" << mass << ", width=" << width << ", lifetime=" << lifetime << "\n";
          G4cout << "debug: mass=" << 5.62 << ", width =" << 1.39e-12/6.582e-16 << ", lifetime=" << 1.39e-12 << "\n";
          }
        //*/
        p = new G4ParticleDefinition(
                                     nam,        // name
                                     mass,       // mass
                                     width,      // width
                                     charge,     // charge
                                     0,                                      // 2*spin
                                     0,          // parity
                                     0,          // C-conjugation
                                     0,          // 2*isospin
                                     0,          // 2*isospin3
                                     0,          // G-parity
                                     "extra",    // type
                                     0,          // lepton number
                                     0,          // baryon number
                                     pdg,        // PDG encoding
                                     width==0?true:false,      // stable
                                     lifetime,   // lifetime
                                     NULL,       // decay table
                                     false);      // short lived
      }
    }
  }

  G4cout << "Loaded extra particles using file: " << m_pdgfile << G4endl;
}

void Geant4ExtraParticles::constructProcess(Constructor& ctor) {
  G4ParticleTable::G4PTblDicIterator* ParticleIterator = ctor.particleIterator();
#if G4VERSION_NUMBER < 940
  if ( 0 == _scatter ) _scatter=new G4hMultipleScattering();
  if ( 0 == _ionise ) _ionise=new G4hIonisation()
  if ( 0 == _decay ) _decay=new G4Decay()
#endif
  while((*ParticleIterator)()) {
    G4ParticleDefinition* pdef = ParticleIterator->value();
    G4ProcessManager* pmgr = pdef->GetProcessManager();
    if (pdef->GetParticleType() == "extra") {
      if (pdef->GetPDGCharge() != 0) {
#if G4VERSION_NUMBER < 940
        pmgr->AddProcess(_scatter, -1,  1, 1); // multiple scattering
        pmgr->AddProcess(_ionise,  -1,  2, 2); // ionisation
        pmgr->AddProcess(_decay,   -1, -1, 2); // decay
#else
        pmgr->AddProcess(new G4hMultipleScattering(), -1,  1, 1); //multiple scattering
        pmgr->AddProcess(new G4hIonisation(),  -1,  2, 2); // ionisation
        pmgr->AddProcess(new G4Decay(),   -1, -1, 2); // decay 
#endif

      } else {

#if G4VERSION_NUMBER < 940
        pmgr->AddProcess(_scatter=new G4hMultipleScattering(), -1,  1, 1); // multiple scattering
        pmgr->AddProcess(_decay=new G4Decay(),   -1, -1, 2); // decay
#else
        //	pmgr->AddProcess(new G4hMultipleScattering(), -1,  1, 1); // multiple scattering
        pmgr->AddProcess(new G4Decay(),   -1, -1, 2); // decay 
#endif

      }
    }
  }
}

DECLARE_GEANT4ACTION(Geant4ExtraParticles)
