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
#ifndef __dax_internal_Testing_h
#define __dax_internal_Testing_h

#include <dax/Types.h>
#include <dax/TypeTraits.h>
#include <dax/VectorTraits.h>

#include <iostream>
#include <sstream>
#include <string>

#include <math.h>

// Try to enforce using the correct testing version. (Those that include the
// control environment have more possible exceptions.) This is not guaranteed
// to work. To make it more likely, place the Testing.h include last.
#ifdef __dax_cont_Error_h
#ifndef __dax_cont_internal_Testing_h
#error Use dax::cont::internal::Testing instead of dax::internal::Testing.
#else
#define DAX_TESTING_IN_CONT
#endif
#endif

/// \def DAX_TEST_ASSERT(condition, message)
///
/// Asserts a condition for a test to pass. A passing condition is when \a
/// condition resolves to true. If \a condition is false, then the test is
/// aborted and failure is returned.

#define DAX_TEST_ASSERT(condition, message) \
  ::dax::internal::Testing::Assert( \
      condition, __FILE__, __LINE__, message, #condition)

/// \def DAX_TEST_FAIL(message)
///
/// Causes a test to fail with the given \a message.

#define DAX_TEST_FAIL(message) \
  throw ::dax::internal::Testing::TestFailure(__FILE__, __LINE__, message)

namespace dax {
namespace internal {

struct Testing
{
public:
  class TestFailure
  {
  public:
    DAX_CONT_EXPORT TestFailure(const std::string &file,
                                dax::Id line,
                                const std::string &message)
      : File(file), Line(line), Message(message) { }

    DAX_CONT_EXPORT TestFailure(const std::string &file,
                                dax::Id line,
                                const std::string &message,
                                const std::string &condition)
      : File(file), Line(line)
    {
      this->Message.append(message);
      this->Message.append(" (");
      this->Message.append(condition);
      this->Message.append(")");
    }

    DAX_CONT_EXPORT const std::string &GetFile() const { return this->File; }
    DAX_CONT_EXPORT dax::Id GetLine() const { return this->Line; }
    DAX_CONT_EXPORT const std::string &GetMessage() const {
      return this->Message;
    }
  private:
    std::string File;
    dax::Id Line;
    std::string Message;
  };

  static DAX_CONT_EXPORT void Assert(bool condition,
                                     const std::string &file,
                                     dax::Id line,
                                     const std::string &message,
                                     const std::string &conditionString)
  {
    if (condition)
      {
      // Do nothing.
      }
    else
      {
      throw TestFailure(file, line, message, conditionString);
      }
  }

#ifndef DAX_TESTING_IN_CONT
  /// Calls the test function \a function with no arguments. Catches any errors
  /// generated by DAX_TEST_ASSERT or DAX_TEST_FAIL, reports the error, and
  /// returns "1" (a failure status for a program's main). Returns "0" (a
  /// success status for a program's main).
  ///
  /// The intention is to implement a test's main function with this. For
  /// example, the implementation of UnitTestFoo might look something like
  /// this.
  ///
  /// \code
  /// #include <dax/internal/Testing.h>
  ///
  /// namespace {
  ///
  /// void TestFoo()
  /// {
  ///    // Do actual test, which checks in DAX_TEST_ASSERT or DAX_TEST_FAIL.
  /// }
  ///
  /// } // anonymous namespace
  ///
  /// int UnitTestFoo(int, char *[])
  /// {
  ///   return dax::internal::Testing::Run(TestFoo);
  /// }
  ///
  template<class Func>
  static DAX_CONT_EXPORT int Run(Func function)
  {
    try
      {
      function();
      }
    catch (TestFailure error)
      {
      std::cout << "***** Test failed @ "
                << error.GetFile() << ":" << error.GetLine() << std::endl
                << error.GetMessage() << std::endl;
      return 1;
      }
    catch (...)
      {
      std::cout << "***** Unidentified exception thrown." << std::endl;
      return 1;
      }
    return 0;
  }
#endif

  /// Check functors to be used with the TryAllTypes method.
  ///
  struct TypeCheckAlwaysTrue {
    template <typename T, class Functor>
    void operator()(T t, Functor function) const { function(t); }
  };
  struct TypeCheckInteger {
    template <typename T, class Functor>
    void operator()(T t, Functor function) const {
      this->DoInteger(typename dax::TypeTraits<T>::NumericTag(), t, function);
    }
  private:
    template <class Tag, typename T, class Functor>
    void DoInteger(Tag, T, const Functor&) const {  }
    template <typename T, class Functor>
    void DoInteger(dax::TypeTraitsIntegerTag, T t, Functor function) const {
      function(t);
    }
  };
  struct TypeCheckReal {
    template <typename T, class Functor>
    void operator()(T t, Functor function) const {
      this->DoReal(typename dax::TypeTraits<T>::NumericTag(), t, function);
    }
  private:
    template <class Tag, typename T, class Functor>
    void DoReal(Tag, T, const Functor&) const {  }
    template <typename T, class Functor>
    void DoReal(dax::TypeTraitsRealTag, T t, Functor function) const {
      function(t);
    }
  };
  struct TypeCheckScalar {
    template <typename T, class Functor>
    void operator()(T t, Functor func) const {
      this->DoScalar(typename dax::TypeTraits<T>::DimensionalityTag(), t, func);
    }
  private:
    template <class Tag, typename T, class Functor>
    void DoScalar(Tag, const T &, const Functor &) const {  }
    template <typename T, class Functor>
    void DoScalar(dax::TypeTraitsScalarTag, T t, Functor function) const {
      function(t);
    }
  };
  struct TypeCheckVector {
    template <typename T, class Functor>
    void operator()(T t, Functor func) const {
      this->DoVector(typename dax::TypeTraits<T>::DimensionalityTag(), t, func);
    }
  private:
    template <class Tag, typename T, class Functor>
    void DoVector(Tag, const T &, const Functor &) const {  }
    template <typename T, class Functor>
    void DoVector(dax::TypeTraitsVectorTag, T t, Functor function) const {
      function(t);
    }
  };

  template<class FunctionType>
  struct InternalPrintOnInvoke {
    InternalPrintOnInvoke(FunctionType function, std::string toprint)
      : Function(function), ToPrint(toprint) { }
    template <typename T> void operator()(T t) {
      std::cout << this->ToPrint << std::endl;
      this->Function(t);
    }
  private:
    FunctionType Function;
    std::string ToPrint;
  };

  /// Runs templated \p function on all the basic types defined in Dax. This is
  /// helpful to test templated functions that should work on all types. If the
  /// function is supposed to work on some subset of types, then \p check can
  /// be set to restrict the types used. This Testing class contains several
  /// helpful check functors.
  ///
  template<class FunctionType, class CheckType>
  static void TryAllTypes(FunctionType function, CheckType check)
  {
    dax::Id id = 0;
    check(id, InternalPrintOnInvoke<FunctionType>(function, "dax::Id"));

    dax::Id3 id3 = dax::make_Id3(0, 0, 0);
    check(id3, InternalPrintOnInvoke<FunctionType>(function, "dax::Id3"));

    dax::Scalar scalar = 0.0;
    check(scalar, InternalPrintOnInvoke<FunctionType>(function, "dax::Scalar"));

    dax::Vector3 vector3 = dax::make_Vector3(0.0, 0.0, 0.0);
    check(vector3, InternalPrintOnInvoke<FunctionType>(function, "dax::Vector3"));

    dax::Vector4 vector4 = dax::make_Vector4(0.0, 0.0, 0.0, 0.0);
    check(vector4, InternalPrintOnInvoke<FunctionType>(function, "dax::Vector4"));
  }
  template<class FunctionType>
  static void TryAllTypes(FunctionType function)
  {
    TryAllTypes(function, TypeCheckAlwaysTrue());
  }
};

}
} // namespace dax::internal

/// Helper function to test two quanitites for equality accounting for slight
/// variance due to floating point numerical inaccuracies.
///
template<typename VectorType>
DAX_EXEC_CONT_EXPORT bool test_equal(VectorType vector1,
                                     VectorType vector2,
                                     dax::Scalar tolerance = 0.0001)
{
  typedef typename dax::VectorTraits<VectorType> Traits;
  for (int component = 0; component < Traits::NUM_COMPONENTS; component++)
    {
    dax::Scalar value1 = Traits::GetComponent(vector1, component);
    dax::Scalar value2 = Traits::GetComponent(vector2, component);
    if ((fabs(value1) < 2*tolerance) && (fabs(value2) < 2*tolerance))
      {
      continue;
      }
    dax::Scalar ratio = value1/value2;
    if ((ratio < 1.0 - tolerance) || (ratio > 1.0 + tolerance))
      {
      return false;
      }
    }
  return true;
}

#endif //__dax_internal_Testing_h
