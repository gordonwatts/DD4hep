// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/World.h"

using std::string;
using namespace DD4hep::Geometry;

/// Access the conditions loading
World::Condition World::getCondition(Condition::key_type key, const Condition::iov_type& iov)  const  {
  return access()->getCondition(key, iov);
}

/// Access the conditions loading
World::Condition World::getCondition(Condition::key_type key, const UserPool& pool)  const  {
  return access()->getCondition(key, pool);
}

/// Access the detector descrion tree
LCDD& World::lcdd() const   {
  return *(access()->lcdd);
}

