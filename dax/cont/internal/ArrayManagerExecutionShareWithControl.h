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
#ifndef __dax_cont_internal_ArrayManagerExecutionShareWithControl_h
#define __dax_cont_internal_ArrayManagerExecutionShareWithControl_h

#include <dax/Types.h>

#include <dax/cont/Assert.h>
#include <dax/cont/ArrayContainerControl.h>

#include <algorithm>

namespace dax {
namespace cont {
namespace internal {

/// \c ArrayManagerExecutionShareWithControl provides an implementation for a
/// \c ArrayManagerExecution class for a device adapter when the execution
/// and control environments share memory. This class basically defers all its
/// calls to an \c ArrayContainerControl class and uses the array allocated
/// there.
///
template<typename T, class ArrayContainerControlTag>
class ArrayManagerExecutionShareWithControl
{
public:
  typedef T ValueType;
  typedef dax::cont::internal
      ::ArrayContainerControl<ValueType, ArrayContainerControlTag>
      ContainerType;
  typedef typename ContainerType::IteratorType IteratorType;
  typedef typename ContainerType::IteratorConstType IteratorConstType;

  DAX_CONT_EXPORT ArrayManagerExecutionShareWithControl()
    : IteratorsValid(false), ConstIteratorsValid(false) { }

  /// Saves the given iterators to be returned later.
  ///
  DAX_CONT_EXPORT void LoadDataForInput(IteratorConstType beginIterator,
                                        IteratorConstType endIterator)
  {
    this->BeginConstIterator = beginIterator;
    this->EndConstIterator = endIterator;
    this->ConstIteratorsValid = true;

    // Non-const versions not defined.
    this->IteratorsValid = false;
  }

  /// Saves the given iterators to be returned later.
  ///
  DAX_CONT_EXPORT void LoadDataForInput(IteratorType beginIterator,
                                        IteratorType endIterator)
  {
    // This only works if there is a valid cast from non-const to const
    // iterator.
    this->LoadDataForInput(IteratorConstType(beginIterator),
                           IteratorConstType(endIterator));

    this->BeginIterator = beginIterator;
    this->EndIterator = endIterator;
    this->IteratorsValid = true;
  }

  /// Actually just allocates memory in the given \p controlArray.
  ///
  DAX_CONT_EXPORT void AllocateArrayForOutput(ContainerType &controlArray,
                                              dax::Id numberOfValues)
  {
    controlArray.Allocate(numberOfValues);

    this->BeginIterator = controlArray.GetIteratorBegin();
    this->EndIterator = controlArray.GetIteratorEnd();
    this->IteratorsValid = true;

    this->BeginConstIterator = controlArray.GetIteratorConstBegin();
    this->EndConstIterator = controlArray.GetIteratorConstEnd();
    this->ConstIteratorsValid = true;
  }

  /// This method is a no-op (except for a few checks). Any data written to
  /// this class's iterators should already be written to the give \c
  /// controlArray (under correct operation).
  ///
  DAX_CONT_EXPORT void RetrieveOutputData(ContainerType &controlArray) const
  {
    DAX_ASSERT_CONT(this->ConstIteratorsValid);
    DAX_ASSERT_CONT(
          controlArray.GetIteratorConstBegin() == this->BeginConstIterator);
    DAX_ASSERT_CONT(
          controlArray.GetIteratorConstEnd() == this->EndConstIterator);
  }

  /// This methods copies data from the execution array into the given
  /// iterator.
  ///
  template <class IteratorTypeControl>
  DAX_CONT_EXPORT void CopyInto(IteratorTypeControl dest) const
  {
    DAX_ASSERT_CONT(this->ConstIteratorsValid);
    std::copy(this->BeginConstIterator, this->EndConstIterator, dest);
  }

  /// Adjusts saved end iterators to resize array.
  ///
  DAX_CONT_EXPORT void Shrink(dax::Id numberOfValues)
  {
    DAX_ASSERT_CONT(this->ConstIteratorsValid);
    DAX_ASSERT_CONT(numberOfValues <= std::distance(this->BeginConstIterator,
                                                    this->EndConstIterator));

    this->EndConstIterator = this->BeginConstIterator;
    std::advance(this->EndConstIterator, numberOfValues);

    if (this->IteratorsValid)
      {
      this->EndIterator = this->BeginIterator;
      std::advance(this->EndIterator, numberOfValues);
      }
  }

  /// Returns the iterator previously saved from an \c ArrayContainerControl.
  ///
  DAX_CONT_EXPORT IteratorType GetIteratorBegin()
  {
    DAX_ASSERT_CONT(this->IteratorsValid);
    return this->BeginIterator;
  }

  /// Returns the iterator previously saved from an \c ArrayContainerControl.
  ///
  DAX_CONT_EXPORT IteratorType GetIteratorEnd()
  {
    DAX_ASSERT_CONT(this->IteratorsValid);
    return this->EndIterator;
  }

  /// Const version of GetIteratorBegin.
  ///
  DAX_CONT_EXPORT IteratorConstType GetIteratorConstBegin() const
  {
    DAX_ASSERT_CONT(this->ConstIteratorsValid);
    return this->BeginConstIterator;
  }

  /// Const version of GetIteratorEnd.
  ///
  DAX_CONT_EXPORT IteratorConstType GetIteratorConstEnd() const
  {
    DAX_ASSERT_CONT(this->ConstIteratorsValid);
    return this->EndConstIterator;
  }

  /// A no-op.
  ///
  DAX_CONT_EXPORT void ReleaseResources() { }

private:
  // Not implemented.
  ArrayManagerExecutionShareWithControl(
      ArrayManagerExecutionShareWithControl<T, ArrayContainerControlTag> &);
  void operator=(
      ArrayManagerExecutionShareWithControl<T, ArrayContainerControlTag> &);

  IteratorType BeginIterator;
  IteratorType EndIterator;
  bool IteratorsValid;

  IteratorConstType BeginConstIterator;
  IteratorConstType EndConstIterator;
  bool ConstIteratorsValid;
};

}
}
} // namespace dax::cont::internal

#endif //__dax_cont_internal_ArrayManagerExecutionShareWithControl_h