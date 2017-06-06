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
#ifndef DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H
#define DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H

// Framework includes
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/Printout.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace Alignments {
    
    /// Generic alignment processor facade for the Conditons::Processor object
    /**
     *  This wrapper converts any object, which has the signature
     *  int operator()(Alignment cond) const
     *  The object is automatically wrapped to a Alignment::Processor object
     *  and allows to scan trees using e.g. DetElementProcessors etc.
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class AlignmentsProcessor : public Alignment::Processor  {
      T& processor;
    public:
      /// Default constructor
      AlignmentsProcessor() = default;
      /// Initializing constructor
      AlignmentsProcessor(T& p) : processor(p) {}
      /// Copy constructor
      AlignmentsProcessor(const AlignmentsProcessor& copy) = default;
      /// Default destructor
      virtual ~AlignmentsProcessor() = default;
      /// Assignment operator
      AlignmentsProcessor& operator=(const AlignmentsProcessor& copy) = default;
      /// Processing callback
      virtual int operator()(Alignment alignment)  const override  {
        return (processor)(alignment);
      }
    };
    /// Creator utility function for AlignmentsProcessor objects
    template <typename T> inline AlignmentsProcessor<T> alignmentsProcessor(T* obj)  {
      return AlignmentsProcessor<T>(obj);
    }

    /// Generic alignment processor facade for the Conditons::Processor object
    /**
     *  This wrapper converts any object, which has the signature
     *  int operator()(Alignment cond) const
     *  The object is automatically wrapped to a Alignment::Processor object
     *  and allows to scan trees using e.g. DetElementProcessors etc.
     *
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class AlignmentsProcessorWrapper : public Alignment::Processor  {
      std::unique_ptr<T> processor;
    public:
      /// Default constructor
      AlignmentsProcessorWrapper() = default;
      /// Initializing constructor
      AlignmentsProcessorWrapper(T* p) : processor(p) {}
      /// Copy constructor
      AlignmentsProcessorWrapper(const AlignmentsProcessorWrapper& copy) = default;
      /// Default destructor
      virtual ~AlignmentsProcessorWrapper() = default;
      /// Assignment operator
      AlignmentsProcessorWrapper& operator=(const AlignmentsProcessorWrapper& copy) = default;
      /// Processing callback
      virtual int operator()(Alignment c)  const override  {
        return (*(processor.get()))(c);
      }
    };
    /// Creator utility function for AlignmentsProcessorWrapper objects
    template <typename T> inline AlignmentsProcessorWrapper<T>* createProcessorWrapper(T* obj)  {
      return new AlignmentsProcessorWrapper<T>(obj);
    }
    /// Creator utility function for AlignmentsProcessorWrapper objects
    template <typename T> inline AlignmentsProcessorWrapper<T> processorWrapper(T* obj)  {
      return AlignmentsProcessorWrapper<T>(obj);
    }

    /// Generic Alignment-Delta collector keyed by detector elements
    /**
     *   To be used with utilities like DetElementProcessor etc.
     *
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class DeltaCollector  {
    public:
      /// Reference to the user pool
      ConditionsMap& mapping;
      /// Collection container
      T&             deltas;
    public:
      /// Default constructor
      DeltaCollector(ConditionsMap& m, T& d) : mapping(m), deltas(d) {}
      /// Copy constructor
      DeltaCollector(const DeltaCollector& copy) = default;
      /// Default destructor
      ~DeltaCollector() = default;
      /// Assignment operator
      DeltaCollector& operator=(const DeltaCollector& copy) = default;
      /// Callback to output alignments information
      /** Note: Valid implementations exist for the container types:
       *        std::list<Delta>
       *        std::vector<Delta>
       *        std::map<DetElement,Delta>
       *        std::multimap<DetElement,Delta>
       *        std::map<std::string,Delta>        key = DetElement.path()
       *        std::multimap<std::string,Delta>   key = DetElement.path()
       */
      virtual int operator()(DetElement de, int level=0)  const final;
    };
    template <typename T> inline DeltaCollector<T> deltaCollector(ConditionsMap& m, T& deltas)  {
      return DeltaCollector<T>(m, deltas);
    }

    /// Generic alignment collector keyed by detector elements
    /**
     *   To be used with utilities like DetElementProcessor etc.
     *
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class AlignmentsCollector  {
    public:
      /// Reference to the user pool
      ConditionsMap& mapping;
      /// Collection container
      T&             alignments;
    public:
      /// Default constructor
      AlignmentsCollector(ConditionsMap& m, T& d) : mapping(m), alignments(d) {}
      /// Copy constructor
      AlignmentsCollector(const AlignmentsCollector& copy) = default;
      /// Default destructor
      ~AlignmentsCollector() = default;
      /// Assignment operator
      AlignmentsCollector& operator=(const AlignmentsCollector& copy) = default;
      /// Callback to output alignments information
      /** Note: Valid implementations exist for the container types:
       *        std::list<Alignment>
       *        std::vector<Alignment>
       *        std::map<DetElement,Alignment>
       *        std::multimap<DetElement,Alignment>
       *        std::map<std::string,Alignment>        key = DetElement.path()
       *        std::multimap<std::string,Alignment>   key = DetElement.path()
       */
      virtual int operator()(DetElement de, int level=0)  const final;
    };
    template <typename T> inline AlignmentsCollector<T> alignmentsCollector(ConditionsMap& m, T& alignments)  {
      return AlignmentsCollector<T>(m, alignments);
    }

  }    /* End namespace Alignments  */
}      /* End namespace DD4hep      */
#endif /* DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H  */
