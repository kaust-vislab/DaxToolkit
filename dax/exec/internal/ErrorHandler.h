//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2012 Sandia Corporation.
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//=============================================================================
#ifndef __dax_exec_internal_ErrorHandler_h
#define __dax_exec_internal_ErrorHandler_h

#include <dax/internal/DataArray.h>

namespace dax {
namespace exec {
namespace internal {

/// Used to hold an error in the execution environment until the parallel
/// execution can complete. Currently the ErrorHandler holds an array of
/// charaters. This string should be global to all threads. If the first entry
/// in the string is '\0' (the C string terminator), then we consider it as no
/// error. Otherwise, the array contains the string describing the error.
///
/// Before scheduling worklets, the global array should be cleared to have no
/// error. This can only be reliably done by the device adapter.
///
class ErrorHandler
{
public:
  DAX_EXEC_EXPORT ErrorHandler(dax::internal::DataArray<char> m)
    : Message(m) { }

  DAX_EXEC_EXPORT void RaiseError(const char *message)
  {
    // Only raise the error if one has not been raised yet. This check is not
    // guaranteed to work across threads. However, chances are that if two or
    // more threads simultaneously pass this test, they will be writing the
    // same error, which is fine. Even in the much less likely case that two
    // threads simultaneously write different error messages, the worst case is
    // that you get a mangled message. That's not good (and it's what we are
    // trying to avoid), but it's not critical.
    if (this->IsErrorRaised()) { return; }

    // Safely copy message into array.
    dax::Id index;
    for (index = 0; index < this->Message.GetNumberOfEntries(); index++)
      {
      if (message[index] == '\0') break;
      this->Message.SetValue(index, message[index]);
      }
    this->Message.SetValue(index, '\0');
  }

  DAX_EXEC_EXPORT bool IsErrorRaised() const
  {
    return (this->Message.GetValue(0) != '\0');
  }

private:
  dax::internal::DataArray<char> Message;
};

}
}
} // namespace dax::exec::internal

#endif // __dax_exec_internal_ErrorHandler_h