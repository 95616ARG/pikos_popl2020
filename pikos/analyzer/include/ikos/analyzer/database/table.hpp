/*******************************************************************************
 *
 * \file
 * \brief Base class for database tables
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

#include <string>

#include <ikos/analyzer/database/sqlite.hpp>

namespace ikos {
namespace analyzer {

/// \brief Base class for database tables
class DatabaseTable {
protected:
  /// \brief Database connection
  sqlite::DbConnection& _db;

  /// \brief Table name
  std::string _name;

public:
  /// \brief Deleted default constructor
  DatabaseTable() = delete;

  /// \brief Constructor
  ///
  /// \param db The database connection
  /// \param name The table name
  /// \param cols The table columns
  /// \param indexes The table indexes
  DatabaseTable(
      sqlite::DbConnection& db,
      std::string name,
      llvm::ArrayRef< std::pair< StringRef, sqlite::DbColumnType > > cols,
      llvm::ArrayRef< StringRef > indexes);

  /// \brief Deleted copy constructor
  DatabaseTable(const DatabaseTable&) = delete;

  /// \brief Default move constructor
  DatabaseTable(DatabaseTable&&) = default;

  /// \brief Deleted copy assignment operator
  DatabaseTable& operator=(const DatabaseTable&) = delete;

  /// \brief Deleted move assignment operator
  DatabaseTable& operator=(DatabaseTable&&) = delete;

  /// \brief Destructor
  ~DatabaseTable() = default;

  /// \brief Name of the table
  const std::string& name() const { return this->_name; }

}; // end class DatabaseTable

} // end namespace analyzer
} // end namespace ikos
