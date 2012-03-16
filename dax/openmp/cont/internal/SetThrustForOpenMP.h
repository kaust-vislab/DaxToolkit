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
#ifndef __dax_openmp_cont_internal_SetThrustForOpenMP_h
#define __dax_openmp_cont_internal_SetThrustForOpenMP_h

#ifndef THRUST_DEVICE_BACKEND
#define THRUST_DEVICE_BACKEND THRUST_DEVICE_BACKEND_OMP
#else // defined THRUST_DEVICE_BACKEND
#if THRUST_DEVICE_BACKEND != THRUST_DEVICE_BACKEND_OMP
#error Thrust device backend set incorrectly.
#endif // THRUST_DEVICE_BACKEND != THRUST_DEVICE_BACKEND_OMP
#endif // defined THRUST_DEVICE_BACKEND

#endif //__dax_openmp_cont_internal_SetThrustForOpenMP_h
