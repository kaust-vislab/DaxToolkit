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
#ifndef __dax_cont_arg_FieldArrayHandle_h
#define __dax_cont_arg_FieldArrayHandle_h

#include <dax/Types.h>
#include <dax/cont/arg/ConceptMap.h>
#include <dax/cont/arg/Field.h>
#include <dax/cont/ArrayHandle.h>
#include <dax/cont/sig/Tag.h>
#include <dax/exec/arg/FieldPortal.h>
#include <dax/internal/Tags.h>

namespace dax { namespace cont { namespace arg {

/// \headerfile FieldArrayHandle.h dax/cont/arg/FieldArrayHandle.h
/// \brief Map array of user values to \c Field worklet parameters.
template <typename Tags, typename T, typename ContainerTag, typename Device>
struct ConceptMap< Field(Tags), dax::cont::ArrayHandle<T, ContainerTag, Device> >
{
private:
  typedef dax::cont::ArrayHandle<T,ContainerTag, Device > HandleType;
  HandleType Handle;

public:
  //ignore constant values when finding size of domain
  typedef typename dax::cont::arg::SupportedDomains<dax::cont::sig::AnyDomain>::Tags DomainTags;
  typedef dax::exec::arg::FieldPortal<T,Tags,
          typename HandleType::PortalExecution,
          typename HandleType::PortalConstExecution> ExecArg;

  ConceptMap(HandleType handle):
    Handle(handle),
    ExecArg_()
    {}

  DAX_CONT_EXPORT ExecArg& GetExecArg() { return this->ExecArg_; }

  DAX_CONT_EXPORT void ToExecution(dax::Id size, boost::false_type, boost::true_type)
    { /* Output */
    this->ExecArg_.SetPortal(this->Handle.PrepareForOutput(size));
    }

  DAX_CONT_EXPORT void ToExecution(dax::Id, boost::true_type,  boost::false_type)
    { /* Input  */
    this->ExecArg_.SetPortal(this->Handle.PrepareForInput());
    }

  //we need to pass the number of elements to allocate
  DAX_CONT_EXPORT void ToExecution(dax::Id size)
    {
    ToExecution(size,typename Tags::template Has<dax::cont::sig::In>(),
           typename Tags::template Has<dax::cont::sig::Out>());
    }

  DAX_CONT_EXPORT dax::Id GetDomainLength(sig::Domain) const
    {
    //determine the proper work count be seing if we are being used
    //as input or output
    return this->Handle.GetNumberOfValues();
    }

private:
  ExecArg ExecArg_;
};

/// \headerfile FieldArrayHandle.h dax/cont/arg/FieldArrayHandle.h
/// \brief Map array of user values to \c Field worklet parameters.
template <typename Tags, typename T, typename ContainerTag, typename Device>
struct ConceptMap< Field(Tags), const dax::cont::ArrayHandle<T, ContainerTag, Device> >
{
private:
  typedef dax::cont::ArrayHandle<T,ContainerTag, Device > HandleType;
  HandleType Handle;

public:
  //ignore constant values when finding size of domain
  typedef typename dax::cont::arg::SupportedDomains<dax::cont::sig::AnyDomain>::Tags DomainTags;
  typedef dax::exec::arg::FieldPortal<T,Tags,
          typename HandleType::PortalExecution,
          typename HandleType::PortalConstExecution> ExecArg;

  ConceptMap(HandleType handle):
    Handle(handle),
    ExecArg_()
    {}

  DAX_CONT_EXPORT ExecArg& GetExecArg() { return this->ExecArg_; }
  DAX_CONT_EXPORT void ToExecution(dax::Id, boost::true_type,  boost::false_type)
    { /* Input  */
    this->ExecArg_.SetPortal(this->Handle.PrepareForInput());
    }
  //we need to pass the number of elements to allocate
  DAX_CONT_EXPORT void ToExecution(dax::Id size)
    {
    ToExecution(size,typename Tags::template Has<dax::cont::sig::In>(),
           typename Tags::template Has<dax::cont::sig::Out>());
    }
  DAX_CONT_EXPORT dax::Id GetDomainLength(sig::Domain) const
    {
    return this->Handle.GetNumberOfValues();
    }

private:
  ExecArg ExecArg_;
};


} } } //namespace dax::cont::arg

#endif //__dax_cont_arg_FieldArrayHandle_h
