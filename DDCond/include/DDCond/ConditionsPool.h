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
#ifndef DDCOND_CONDITIONSPOOL_H
#define DDCOND_CONDITIONSPOOL_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Conditions.h"
#include "DDCond/ConditionsManager.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class ConditionsPoolInsert;
    class ConditionsManagerObject;

    /// Class implementing the conditions collection for a given IOV type
    /**
     *  Implementation is mostly virtual, to allow to switch between
     *  different implementations, which allow for different lookup
     *  and store optimizations and/or various caches.
     *  The interface only represents the basic functionality required.
     *
     *  For convenience, the class definition is here.
     *  See ConditionsManager.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsPool : public NamedObject {
    protected:
      /// Handle to conditions manager object
      ConditionsManager m_manager;
      
    public:
      /// Forward definition of the key type
      typedef Condition::key_type key_type;

      enum { AGE_NONE    = 0, 
             AGE_ANY     = 9999999,
             AGE_EXPIRED = 12345678
      };
      /// IOV type of the conditions hosted by this pool
      const IOVType*   iovType;
      /// The IOV of the conditions hosted
      IOV*             iov;
      /// Aging value
      int              age_value;

    protected:
      friend class ConditionsPoolInsert;
      friend class ConditionsPoolRemove;
      friend class ConditionsManagerObject;

      /// Listener invocation when a condition is registered to the cache
      void onRegister(Condition condition);
      /// Listener invocation when a condition is deregistered from the cache
      void onRemove(Condition condition);
      /// Register a new condition to this pool
      virtual void insert(Condition cond) = 0;
      /// Register a new condition to this pool. May overload for performance reasons.
      virtual void insert(RangeConditions& cond) = 0;

    public:
      /// Default constructor
      ConditionsPool(ConditionsManager mgr);
      /// Default destructor. Note: pool must be cleared by the subclass!
      virtual ~ConditionsPool();
      /// Print pool basics
      void print(const std::string& opt)   const;
      /// Full cleanup of all managed conditions.
      virtual void clear() = 0;
      /// Check if a condition exists in the pool
      virtual Condition exists(Condition::key_type key)  const = 0;
      /// Select the conditions matching the DetElement and the conditions name
      virtual void select(Condition::key_type key, RangeConditions& result) = 0;
      /// Select all conditions contained
      virtual void select_all(RangeConditions& result) = 0;
      /// Select all conditions contained
      virtual void select_all(ConditionsPool& selection_pool) = 0;
      /// Select the conditons, used also by the DetElement of the condition
      virtual void select_used(RangeConditions& result) = 0;
      /// Total entry count
      virtual size_t count()  const = 0;
    };

    /// Interface for conditions pool optimized to host conditions updates.
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    class UpdatePool : public ConditionsPool  {

    public:
      typedef std::vector<Condition> ConditionEntries;
      /// Update container specification
      typedef std::map<const IOV*, ConditionEntries> UpdateEntries;

    public:
      /// Default constructor
      UpdatePool(ConditionsManager mgr);
      /// Default destructor.
      virtual ~UpdatePool();
      /// Adopt all entries sorted by IOV. Entries will be removed from the pool
      virtual void popEntries(UpdateEntries& entries) = 0;
      /// Select the conditions matching the key
      virtual void select_range(Condition::key_type key, 
				const Condition::iov_type& req_validity,
                                RangeConditions& result) = 0;
    };

    /// Interface for conditions pool optimized to host conditions updates.
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    class UserPool  {
    public:
      /// Forward definition of the key type
      typedef Condition::key_type             key_type;
      /// Forward definition of the dependency container
      typedef ConditionsManager::Dependencies Dependencies;

    protected:
      /// The pool's interval of validity
      IOV                    m_iov;
      /// Handle to conditions manager object
      ConditionsManager      m_manager;
      /// IOV Pool as data source
      ConditionsIOVPool*     m_iovPool;

    public:
      /// Default constructor
      UserPool(ConditionsManager mgr, ConditionsIOVPool* pool);
      /// Default destructor.
      virtual ~UserPool();
      /// Access the interval of validity for this user pool
      const IOV& validity() const    {  return m_iov;  }
      /// Access the interval of validity for this user pool
      const IOV* validityPtr() const {  return &m_iov; }
      /// Print pool content
      virtual void print(const std::string& opt) const = 0;
      /// Total entry count
      virtual size_t count()  const = 0;
      /// Full cleanup of all managed conditions.
      virtual void clear() = 0;
      /// Check a condition for existence
      virtual bool exists(key_type key)  const = 0;
      /// Check a condition for existence
      virtual bool exists(const ConditionKey& key)  const = 0;
      /// Check if a condition exists in the pool and return it to the caller
      virtual Condition get(key_type key)  const = 0;
      /// Check if a condition exists in the pool and return it to the caller
      virtual Condition get(const ConditionKey& key)  const = 0;
      /// Remove condition by key from pool.
      virtual bool remove(key_type hash_key) = 0;
      /// Remove condition by key from pool.
      virtual bool remove(const ConditionKey& key) = 0;
      /// Register a new condition to this pool
      virtual bool insert(Condition cond) = 0;
      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual long prepare(const IOV& required) = 0;
      /// Evaluate and register all derived conditions from the dependency list
      virtual long compute(const Dependencies& dependencies) = 0;
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif     /* DDCOND_CONDITIONSPOOL_H       */
