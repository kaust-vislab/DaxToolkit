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
//  Copyright 2013 Sandia Corporation.
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//=============================================================================

#define DAX_ARRAY_CONTAINER_CONTROL DAX_ARRAY_CONTAINER_CONTROL_BASIC
#define DAX_DEVICE_ADAPTER DAX_DEVICE_ADAPTER_SERIAL

#include <dax/cont/testing/TestingGridGenerator.h>
#include <dax/cont/testing/Testing.h>

#include <dax/CellTag.h>
#include <dax/TypeTraits.h>

#include <dax/cont/ArrayHandle.h>
#include <dax/cont/ArrayHandleConstant.h>
#include <dax/cont/UniformGrid.h>
#include <dax/cont/DispatcherGenerateTopology.h>

#include <dax/exec/WorkletGenerateTopology.h>

#include <iostream>
#include <vector>


namespace {

const dax::Id DIM = 4;
const dax::Id COUNTS = 7;

struct TestGenerateTopologyCellPermutationWorklet
    : public dax::exec::WorkletGenerateTopology
{
  typedef void ControlSignature(TopologyIn, TopologyOut, FieldInCell);
  typedef void ExecutionSignature(AsVertices(_2), _3, VisitIndex);

  DAX_EXEC_EXPORT
  void operator()(dax::exec::CellVertices<dax::CellTagVertex> &outVertex,
                  dax::Id index,
                  dax::Id visitIndex) const
  {
    outVertex[0] = 10*index + 1000*visitIndex;
  }
};

struct TestGenerateTopologyPointPermutationWorklet
    : public dax::exec::WorkletGenerateTopology
{
  typedef void ControlSignature(TopologyIn, TopologyOut, FieldPointIn);
  typedef void ExecutionSignature(AsVertices(_2), _3, VisitIndex);

  template<class InCellTag>
  DAX_EXEC_EXPORT
  void operator()(dax::exec::CellVertices<dax::CellTagVertex> &outVertex,
                  const dax::exec::CellField<dax::Id,InCellTag> &pointScalars,
                  dax::Id visitIndex) const
  {
    outVertex[0] =
        pointScalars[visitIndex%dax::CellTraits<InCellTag>::NUM_VERTICES];
  }
};

//-----------------------------------------------------------------------------
struct TestGenerateTopologyPermutation
{
  //----------------------------------------------------------------------------
  template <typename InGridType>
  void operator()(const InGridType&) const
    {
    typedef dax::CellTagVertex OutGridTag;
    typedef dax::cont::UnstructuredGrid<OutGridTag> OutGridType;

    dax::cont::testing::TestGrid<InGridType> inGenerator(DIM);
    InGridType inGrid = inGenerator.GetRealGrid();
    OutGridType outGrid;

    // Perhaps there should be a better way to specify a constant value for
    // the number of cells generated per location.
    typedef dax::cont::ArrayHandleConstant<dax::Id> CellCountArrayType;
    CellCountArrayType cellCounts =
        dax::cont::make_ArrayHandleConstant(dax::Id(COUNTS),
                                                 inGrid.GetNumberOfCells());

    std::vector<dax::Id> cellFieldData(inGrid.GetNumberOfCells());
    for (dax::Id cellIndex = 0;
         cellIndex < inGrid.GetNumberOfCells();
         cellIndex++)
      {
      cellFieldData[cellIndex] = cellIndex + 1;
      }
    dax::cont::ArrayHandle<dax::Id> cellField =
        dax::cont::make_ArrayHandle(cellFieldData);

    std::cout << "Trying cell field permutation" << std::endl;

    dax::cont::DispatcherGenerateTopology<
        TestGenerateTopologyCellPermutationWorklet,
        CellCountArrayType> cellPermutationWorklet(cellCounts);
    cellPermutationWorklet.SetRemoveDuplicatePoints(false);
    cellPermutationWorklet.Invoke(inGrid, outGrid, cellField);

    this->CheckCellPermutation(
          outGrid.GetCellConnections().GetPortalConstControl());

    std::vector<dax::Id> pointFieldData(inGrid.GetNumberOfPoints());
    for (dax::Id pointIndex = 0;
         pointIndex < inGrid.GetNumberOfPoints();
         pointIndex++)
      {
      pointFieldData[pointIndex] = pointIndex + 1;
      }
    dax::cont::ArrayHandle<dax::Id> pointField =
        dax::cont::make_ArrayHandle(pointFieldData);

    std::cout << "Trying point field permutation" << std::endl;

    dax::cont::DispatcherGenerateTopology<
        TestGenerateTopologyPointPermutationWorklet,
        CellCountArrayType> pointPermutationWorklet(cellCounts);
    pointPermutationWorklet.SetRemoveDuplicatePoints(false);
    pointPermutationWorklet.Invoke(inGrid, outGrid, pointField);

    this->CheckPointPermutation(
          inGenerator,
          outGrid.GetCellConnections().GetPortalConstControl());
  }

private:
  //----------------------------------------------------------------------------
  template<class OutConnectionsPortal>
  void CheckCellPermutation(const OutConnectionsPortal &outConnections) const
  {
    std::cout << "Checking cell field permutation" << std::endl;
    for (dax::Id index = 0; index < outConnections.GetNumberOfValues(); index++)
      {
      dax::Id expectedValue = 10*((index/COUNTS)+1) + 1000*(index%COUNTS);
      dax::Id actualValue = outConnections.Get(index);
      DAX_TEST_ASSERT(actualValue == expectedValue,
                      "Got bad value testing cell permutation.");
      }
  }

  //----------------------------------------------------------------------------
  template<class InGridType, class OutConnectionsPortal>
  void CheckPointPermutation(
      const dax::cont::testing::TestGrid<InGridType> &inGenerator,
      const OutConnectionsPortal &outConnections) const
  {
    std::cout << "Checking point field permutation" << std::endl;
    for (dax::Id index = 0; index < outConnections.GetNumberOfValues(); index++)
      {
      dax::Id actualValue = outConnections.Get(index);
      dax::Id cellIndex = index/COUNTS;
      dax::Id visitIndex = index%COUNTS;
      typedef typename InGridType::CellTag InCellTag;
      int vertexIndex = visitIndex%dax::CellTraits<InCellTag>::NUM_VERTICES;
      dax::Id expectedValue =
          inGenerator.GetCellConnections(cellIndex)[vertexIndex] + 1;
      DAX_TEST_ASSERT(actualValue == expectedValue,
                      "Got bad value testing cell permutation.");
      }
  }
};


//-----------------------------------------------------------------------------
void RunTestGenerateTopologyPermutation()
  {
  TestGenerateTopologyPermutation()(dax::cont::UniformGrid<>());
  }
} // Anonymous namespace

//-----------------------------------------------------------------------------
int UnitTestGenerateTopologyPermutation(int, char *[])
{
  return dax::cont::testing::Testing::Run(RunTestGenerateTopologyPermutation);
}
