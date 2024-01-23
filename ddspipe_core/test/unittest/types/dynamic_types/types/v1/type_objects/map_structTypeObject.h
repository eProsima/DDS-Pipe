// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file map_structTypeObject.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _FAST_DDS_GENERATED_MAP_STRUCT_TYPE_OBJECT_H_
#define _FAST_DDS_GENERATED_MAP_STRUCT_TYPE_OBJECT_H_


#include <fastrtps/types/TypeObject.h>
#include <map>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif
#else
#define eProsima_user_DllExport
#endif

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(map_struct_SOURCE)
#define map_struct_DllAPI __declspec( dllexport )
#else
#define map_struct_DllAPI __declspec( dllimport )
#endif // map_struct_SOURCE
#else
#define map_struct_DllAPI
#endif
#else
#define map_struct_DllAPI
#endif // _WIN32

using namespace eprosima::fastrtps::types;

eProsima_user_DllExport void registermap_structTypes();

eProsima_user_DllExport const TypeIdentifier* Getmap_structIdentifier(bool complete = false);
eProsima_user_DllExport const TypeObject* Getmap_structObject(bool complete = false);
eProsima_user_DllExport const TypeObject* GetMinimalmap_structObject();
eProsima_user_DllExport const TypeObject* GetCompletemap_structObject();


#endif // _FAST_DDS_GENERATED_MAP_STRUCT_TYPE_OBJECT_H_
