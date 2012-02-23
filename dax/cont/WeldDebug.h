/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __dax_cont_WeldDebug_h
#define __dax_cont_WeldDebug_h

#include <dax/Types.h>

#include <dax/internal/DataArray.h>
#include <dax/cont/ErrorExecution.h>
#include <dax/exec/internal/ErrorHandler.h>

#include <dax/cont/internal/ArrayContainerExecutionCPU.h>

#include <map>
#include <algorithm>
#include <vector>

namespace dax {
namespace cont {

template<typename T>
DAX_CONT_EXPORT void WeldDebug(dax::internal::DataArray<T> ids)
{
  dax::Id size = ids.GetNumberOfEntries();
  T max = 1 + *(std::max_element(ids.GetPointer(),ids.GetPointer()+size));

  //create a bit vector of point usage
  std::vector<bool> pointUsage(max);
  for(dax::Id i=0; i < size; ++i)
    {
    //flag each point id has being used
    pointUsage[ids.GetValue(i)]=true;
    }

  std::map<T,T> uniquePointMap;
  T newId = T();
  for(dax::Id i=0; i < max; ++i)
    {
    if(pointUsage[i])
      {
      uniquePointMap.insert(std::pair<T,T>(i,newId++));
      }
    }

  //Weld the passed in array
  for(dax::Id i=0; i < size; ++i)
    {
    ids.SetValue(i, uniquePointMap.find(ids.GetValue(i))->second);
    }
}

}
} // namespace dax::cont

#endif //__dax_cont_Schedule_h
