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
#ifndef __dax_VectorTraits_h
#define __dax_VectorTraits_h

#include <dax/Types.h>

#include <boost/type_traits/remove_const.hpp>

namespace dax {

/// A tag for vectors that are "true" vectors (i.e. have more than one
/// component).
///
struct VectorTraitsTagMultipleComponents { };

/// A tag for vectors that a really just scalars (i.e. have only one component)
///
struct VectorTraitsTagSingleComponent { };

namespace internal {

template<int numComponents>
struct VectorTraitsMultipleComponentChooser {
  typedef VectorTraitsTagMultipleComponents Type;
};

template<>
struct VectorTraitsMultipleComponentChooser<1> {
  typedef VectorTraitsTagSingleComponent Type;
};

} // namespace detail

/// The VectorTraits class gives several static members that define how
/// to use a given type as a vector.
///
template<class VectorType>
struct VectorTraits
#ifdef DAX_DOXYGEN_ONLY
{
  /// Type of the components in the vector.
  ///
  typedef typename VectorType::ComponentType ComponentType;

  /// Number of components in the vector.
  ///
  static const int NUM_COMPONENTS = VectorType::NUM_COMPONENTS;

  /// A tag specifying whether this vector has multiple components (i.e. is a
  /// "real" vector). This tag can be useful for creating specialized functions
  /// when a vector is really just a scalar.
  ///
  typedef typename internal::VectorTraitsMultipleComponentChooser<
      NUM_COMPONENTS>::Type HasMultipleComponents;

  /// Returns the value in a given component of the vector.
  ///
  DAX_EXEC_CONT_EXPORT static const ComponentType &GetComponent(
      const typename boost::remove_const<VectorType>::type &vector,
      int component);
  DAX_EXEC_CONT_EXPORT static ComponentType &GetComponent(
      typename boost::remove_const<VectorType>::type &vector,
      int component);

  /// Changes the value in a given component of the vector.
  ///
  DAX_EXEC_CONT_EXPORT static void SetComponent(VectorType &vector,
                                                int component,
                                                ComponentType value);

  /// Converts whatever type this vector is into the standard Dax Tuple.
  ///
  DAX_EXEC_CONT_EXPORT
  static dax::Tuple<ComponentType,NUM_COMPONENTS>
  ToTuple(const VectorType &vector);
};
#else // DAX_DOXYGEN_ONLY
    ;
#endif // DAX_DOXYGEN_ONLY

// This partial specialization allows you to define a non-const version of
// VectorTraits and have it still work for const version.
//
template<typename T>
struct VectorTraits<const T> : VectorTraits<T>
{  };

template<typename T, int Size>
struct VectorTraits<dax::Tuple<T,Size> >
{
  typedef dax::Tuple<T,Size> VectorType;

  /// Type of the components in the vector.
  ///
  typedef typename VectorType::ComponentType ComponentType;

  /// Number of components in the vector.
  ///
  static const int NUM_COMPONENTS = VectorType::NUM_COMPONENTS;

  /// A tag specifying whether this vector has multiple components (i.e. is a
  /// "real" vector). This tag can be useful for creating specialized functions
  /// when a vector is really just a scalar.
  ///
  typedef typename internal::VectorTraitsMultipleComponentChooser<
      NUM_COMPONENTS>::Type HasMultipleComponents;

  /// Returns the value in a given component of the vector.
  ///
  DAX_EXEC_CONT_EXPORT
  static const ComponentType &GetComponent(const VectorType &vector,
                                           int component)
  {
    return vector[component];
  }
  DAX_EXEC_CONT_EXPORT
  static ComponentType &GetComponent(VectorType &vector, int component) {
    return vector[component];
  }

  /// Changes the value in a given component of the vector.
  ///
  DAX_EXEC_CONT_EXPORT static void SetComponent(VectorType &vector,
                                                int component,
                                                ComponentType value) {
    vector[component] = value;
  }

  /// Converts whatever type this vector is into the standard Dax Tuple.
  ///
  DAX_EXEC_CONT_EXPORT
  static dax::Tuple<ComponentType,NUM_COMPONENTS>
  ToTuple(const VectorType &vector)
  {
    return vector;
  }
};

namespace internal {
/// Used for overriding VectorTraits for basic scalar types.
///
template<typename ScalarType>
struct VectorTraitsBasic {
  typedef ScalarType ComponentType;
  static const int NUM_COMPONENTS = 1;
  typedef VectorTraitsTagSingleComponent HasMultipleComponents;

  DAX_EXEC_CONT_EXPORT static const ComponentType &GetComponent(
      const ScalarType &vector,
      int) {
    return vector;
  }
  DAX_EXEC_CONT_EXPORT
  static ComponentType &GetComponent(ScalarType &vector, int) {
    return vector;
  }

  DAX_EXEC_CONT_EXPORT static void SetComponent(ScalarType &vector,
                                                int,
                                                ComponentType value) {
    vector = value;
  }

  DAX_EXEC_CONT_EXPORT
  static dax::Tuple<ScalarType,1> ToTuple(const ScalarType &vector)
  {
    return dax::Tuple<ScalarType,1>(vector);
  }
};
}

#define DAX_BASIC_TYPE_VECTOR(type) \
  template<> \
  struct VectorTraits<type> \
      : public dax::internal::VectorTraitsBasic<type> { };/* \
  template<> \
  struct VectorTraits<const type> \
      : public dax::internal::VectorTraitsBasic<type> { }*/

/// Allows you to treat basic types as if they were vectors.

DAX_BASIC_TYPE_VECTOR(float);
DAX_BASIC_TYPE_VECTOR(double);
DAX_BASIC_TYPE_VECTOR(char);
DAX_BASIC_TYPE_VECTOR(unsigned char);
DAX_BASIC_TYPE_VECTOR(short);
DAX_BASIC_TYPE_VECTOR(unsigned short);
DAX_BASIC_TYPE_VECTOR(int);
DAX_BASIC_TYPE_VECTOR(unsigned int);
#if DAX_SIZE_LONG == 8
DAX_BASIC_TYPE_VECTOR(long);
DAX_BASIC_TYPE_VECTOR(unsigned long);
#elif DAX_SIZE_LONG_LONG == 8
DAX_BASIC_TYPE_VECTOR(long long);
DAX_BASIC_TYPE_VECTOR(unsigned long long);
#else
#error No implementation for 64-bit vector traits.
#endif

#undef DAX_BASIC_TYPE_VECTOR

}

#endif //__dax_VectorTraits_h
