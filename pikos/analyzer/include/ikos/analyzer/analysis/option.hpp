/*******************************************************************************
 *
 * \file
 * \brief Analyses options
 *
 * Author: Maxime Arthaud
 *
 * Contact: ikos@lists.nasa.gov
 *
 * Notices:
 *
 * Copyright (c) 2011-2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Disclaimers:
 *
 * No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF
 * ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS,
 * ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE
 * ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO
 * THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN
 * ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS,
 * RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS
 * RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY
 * DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE,
 * IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST
 * THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL
 * AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS
 * IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH
 * USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM,
 * RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD
 * HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS,
 * AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.
 * RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE,
 * UNILATERAL TERMINATION OF THIS AGREEMENT.
 *
 ******************************************************************************/

#pragma once

#include <vector>

#include <boost/optional.hpp>

#include <ikos/ar/semantic/function.hpp>

#include <ikos/analyzer/analysis/hardware_addresses.hpp>
#include <ikos/analyzer/checker/name.hpp>
#include <ikos/analyzer/json/json.hpp>
#include <ikos/analyzer/support/assert.hpp>

namespace ikos {
namespace analyzer {

// forward declaration
class SettingsTable;

/// \brief Machine integer abstract domain
enum class MachineIntDomainOption {
  Interval,
  Congruence,
  IntervalCongruence,
  DBM,
  VarPackDBM,
  VarPackDBMCongruence,
  Gauge,
  GaugeIntervalCongruence,
  ApronInterval,
  ApronOctagon,
  ApronPolkaPolyhedra,
  ApronPolkaLinearEqualities,
  ApronPplPolyhedra,
  ApronPplLinearCongruences,
  ApronPkgridPolyhedraLinearCongruences,
  VarPackApronOctagon,
  VarPackApronPolkaPolyhedra,
  VarPackApronPolkaLinearEqualities,
  VarPackApronPplPolyhedra,
  VarPackApronPplLinearCongruences,
  VarPackApronPkgridPolyhedraLinearCongruences,
};

/// \brief Return a string representing a MachineIntDomainOption
inline const char* machine_int_domain_option_str(MachineIntDomainOption d) {
  switch (d) {
    case MachineIntDomainOption::Interval:
      return "interval";
    case MachineIntDomainOption::Congruence:
      return "congruence";
    case MachineIntDomainOption::IntervalCongruence:
      return "interval-congruence";
    case MachineIntDomainOption::DBM:
      return "dbm";
    case MachineIntDomainOption::VarPackDBM:
      return "var-pack-dbm";
    case MachineIntDomainOption::VarPackDBMCongruence:
      return "var-pack-dbm-congruence";
    case MachineIntDomainOption::Gauge:
      return "gauge";
    case MachineIntDomainOption::GaugeIntervalCongruence:
      return "gauge-interval-congruence";
    case MachineIntDomainOption::ApronInterval:
      return "apron-interval";
    case MachineIntDomainOption::ApronOctagon:
      return "apron-octagon";
    case MachineIntDomainOption::ApronPolkaPolyhedra:
      return "apron-polka-polyhedra";
    case MachineIntDomainOption::ApronPolkaLinearEqualities:
      return "apron-polka-linear-equalities";
    case MachineIntDomainOption::ApronPplPolyhedra:
      return "apron-ppl-polyhedra";
    case MachineIntDomainOption::ApronPplLinearCongruences:
      return "apron-ppl-linear-congruences";
    case MachineIntDomainOption::ApronPkgridPolyhedraLinearCongruences:
      return "apron-pkgrid-polyhedra-lin-cong";
    case MachineIntDomainOption::VarPackApronOctagon:
      return "var-pack-apron-octagon";
    case MachineIntDomainOption::VarPackApronPolkaPolyhedra:
      return "var-pack-apron-polka-polyhedra";
    case MachineIntDomainOption::VarPackApronPolkaLinearEqualities:
      return "var-pack-apron-polka-linear-equalities";
    case MachineIntDomainOption::VarPackApronPplPolyhedra:
      return "var-pack-apron-ppl-polyhedra";
    case MachineIntDomainOption::VarPackApronPplLinearCongruences:
      return "var-pack-apron-ppl-linear-congruences";
    case MachineIntDomainOption::VarPackApronPkgridPolyhedraLinearCongruences:
      return "var-pack-apron-pkgrid-polyhedra-lin-cong";
    default: {
      ikos_unreachable("unreachable");
    }
  }
}

/// \brief Return true if the MachineIntDomainOption has a narrowing operator
inline bool machine_int_domain_option_has_narrowing(MachineIntDomainOption d) {
  switch (d) {
    case MachineIntDomainOption::ApronPolkaPolyhedra:
    case MachineIntDomainOption::ApronPolkaLinearEqualities:
    case MachineIntDomainOption::ApronPplPolyhedra:
    case MachineIntDomainOption::ApronPplLinearCongruences:
    case MachineIntDomainOption::ApronPkgridPolyhedraLinearCongruences:
    case MachineIntDomainOption::VarPackApronPolkaPolyhedra:
    case MachineIntDomainOption::VarPackApronPolkaLinearEqualities:
    case MachineIntDomainOption::VarPackApronPplPolyhedra:
    case MachineIntDomainOption::VarPackApronPplLinearCongruences:
    case MachineIntDomainOption::VarPackApronPkgridPolyhedraLinearCongruences:
      return false;
    default:
      return true;
  }
}

/// \brief Represents the precision of an analysis
enum class Precision {
  /// \brief Only track values in "registers", ie. ar::InternalVariable
  Register = 0,

  /// \brief Register + track pointer base addresses and offsets
  Pointer = 1,

  /// \brief Pointer + track memory
  Memory = 2,
};

/// \brief Return a string representing a precision
inline const char* precision_str(Precision p) {
  switch (p) {
    case Precision::Register:
      return "reg";
    case Precision::Pointer:
      return "ptr";
    case Precision::Memory:
      return "mem";
    default: {
      ikos_unreachable("unreachable");
    }
  }
}

/// \brief Either Interprocedural or Intraprocedural
enum class Procedural {
  /// \brief Analyzes function by taking into account other functions
  Interprocedural,

  /// \brief Analyze function independently
  Intraprocedural,
};

/// \brief Return a string representing a Procedural
inline const char* procedural_str(Procedural proc) {
  switch (proc) {
    case Procedural::Interprocedural:
      return "interprocedural";
    case Procedural::Intraprocedural:
      return "intraprocedural";
    default: {
      ikos_unreachable("unreachable");
    }
  }
}

/// \brief Return a string representing the hardware addresses
inline std::string hardware_addresses_str(const HardwareAddresses& hwa) {
  JsonList json_intervals;
  for (const auto& interval : hwa.ranges()) {
    json_intervals.add(JsonList{interval.lb(), interval.ub()});
  }
  return json_intervals.str();
}

/// \brief Policy of initialization for global variables
///
/// This is mostly used to avoid losing time initializing big strings
enum class GlobalsInitPolicy {
  /// \brief Initialize all global variables
  All,

  /// \brief Initialize all global variables except arrays with more than 100
  /// elements
  SkipBigArrays,

  /// \brief Initialize all global variables except strings ([n x si8]*)
  SkipStrings,

  /// \brief Do not initialize any global variable
  None,
};

/// \brief Return a string representing a GlobalsInitPolicy
inline const char* globals_init_policy_str(GlobalsInitPolicy p) {
  switch (p) {
    case GlobalsInitPolicy::All:
      return "all";
    case GlobalsInitPolicy::SkipBigArrays:
      return "skip-big-arrays";
    case GlobalsInitPolicy::SkipStrings:
      return "skip-strings";
    case GlobalsInitPolicy::None:
      return "none";
    default: {
      ikos_unreachable("unreachable");
    }
  }
}

/// \brief Progress report option
enum class ProgressOption {
  /// \brief Interactive if output is a TTY, otherwise None
  Auto,

  /// \brief Interactive progress
  Interactive,

  /// \brief Linear progress
  Linear,

  /// \brief No progress
  None,
};

/// \brief Display option
enum class DisplayOption {
  /// \brief Display all
  All,

  /// \brief Display failed
  Fail,

  /// \brief Display nothing
  None,
};

/// \brief Hold all the analysis options
struct AnalysisOptions {
public:
  /// \brief List of checkers requested
  std::vector< CheckerName > analyses;

  /// \brief List of entry points
  std::vector< ar::Function* > entry_points;

  /// \brief List of entry points that start with uninitialized global variables
  std::vector< ar::Function* > no_init_globals;

  /// \brief Machine integer abstract domain
  MachineIntDomainOption machine_int_domain;

  /// \brief Whether we should use concurrent analysis or not
  bool use_concurrent;

  /// \brief Number of threads for parallel fixpoint iteration
  int numthreads;

  /// \brief Number of threads for parallel fixpoint iteration
  int context_sensitivity;

  /// \brief Is the analysis interprocedural or intraprocedural
  Procedural procedural;

  /// \brief Wether we should use a liveness analysis or not
  bool use_liveness;

  /// \brief Wether we should use a pointer analysis or not
  bool use_pointer;

  /// \brief Wether we should use a fixpoint profile analysis or not
  bool use_fixpoint_profiles;

  /// \brief Wether we should save fixpoints on called functions or not
  bool use_fixpoint_cache;

  /// \brief Whether we should run checks or not
  bool use_checks;

  /// \brief Precision of the analysis
  Precision precision;

  /// \brief Policy of initialization for global variables
  GlobalsInitPolicy globals_init_policy;

  /// \brief Option to show the progress of the analysis
  ProgressOption progress;

  /// \brief Option to display the invariants
  DisplayOption display_invariants;

  /// \brief Option to display the checks
  DisplayOption display_checks;

  /// \brief Hardware addresses
  HardwareAddresses hardware_addresses;

  /// \brief Value of argc, or boost::none
  boost::optional< int > argc;

public:
  /// \brief Save the options in the output database
  void save(SettingsTable&);

}; // end class AnalysisOptions

} // end namespace analyzer
} // end namespace ikos
