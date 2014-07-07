//----------------------------------------------------------------------------//

#ifndef _INCLUDED_Field3D_OgUtil_H_
#define _INCLUDED_Field3D_OgUtil_H_

//----------------------------------------------------------------------------//
// Includes
//----------------------------------------------------------------------------//

#include <iostream>
#include <string>

#include <OpenEXR/ImathVec.h>

#include "external/Ogawa/All.h"
#include "external/Ogawa/UtilFoundation.h"

//----------------------------------------------------------------------------//
// Defines
//----------------------------------------------------------------------------//

#define OGAWA_THREAD                0
#define OGAWA_START_ID              2
#define OGAWA_DATASET_BASEOFFSET    3
#define OGAWA_INVALID_DATASET_INDEX -1

//----------------------------------------------------------------------------//
// Types
//----------------------------------------------------------------------------//

#if !defined(_MSC_VER)
using ::uint8_t;
using ::int8_t;
using ::uint16_t;
using ::int16_t;
using ::uint32_t;
using ::int32_t;
using ::uint64_t;
using ::int64_t;
#else
typedef unsigned char           uint8_t;
typedef signed char             int8_t;
typedef unsigned short          uint16_t;
typedef signed short            int16_t;
typedef unsigned int            uint32_t;
typedef int                     int32_t;
typedef unsigned long long      uint64_t;
typedef long long               int64_t;
#endif

typedef half                    float16_t;
typedef float                   float32_t;
typedef double                  float64_t;

#ifdef FIELD3D_VERSION_NS
typedef Field3D::V3h            vec16_t;
typedef Field3D::V3f            vec32_t;
typedef Field3D::V3d            vec64_t;
#else
typedef Imath::Vec3<float16_t>  vec16_t;
typedef Imath::Vec3<float32_t>  vec32_t;
typedef Imath::Vec3<float64_t>  vec64_t;
#endif

//----------------------------------------------------------------------------//
// Enums
//----------------------------------------------------------------------------//

//! Enumerates the various uses for Ogawa-level groups. 
//! \warning Do not under any circumstances alter the order of these!
enum OgGroupType {
  F3DGroupType = 0,
  F3DAttributeType,
  F3DDatasetType
};

//----------------------------------------------------------------------------//

//! Enumerates the various uses for Ogawa-level groups. 
//! \warning Do not under any circumstances alter the order of these! If you
//! need to add more types, append them at the end, before F3DInvalidDataType.
enum OgDataType {

  // Signed and unsigned integers from char to long
  F3DInt8 = 0,
  F3DUint8,

  F3DInt16,
  F3DUint16,

  F3DInt32,
  F3DUint32,

  F3DInt64,
  F3DUint64,

  // Floats
  F3DFloat16,
  F3DFloat32,
  F3DFloat64,

  // Vec3
  F3DVec16,
  F3DVec32,
  F3DVec64,

  F3DNumDataTypes, 

  // Invalid type enum
  F3DInvalidDataType = 127
};

//----------------------------------------------------------------------------//
// OgawaTypeTraits
//----------------------------------------------------------------------------//

//! Declares the OgawaTypeTraits struct, but does not implement it.
template <typename T>
struct OgawaTypeTraits;

//----------------------------------------------------------------------------//

#define F3D_DECLARE_OG_TRAITS(type, enum, name)      \
  template <>                                        \
  struct OgawaTypeTraits<type>                       \
  {                                                  \
  typedef type value_type;                           \
  static const char* typeName() { return name; }     \
  static OgDataType  typeEnum() { return enum; }     \
  };

//----------------------------------------------------------------------------//

F3D_DECLARE_OG_TRAITS(int8_t,    F3DInt8,    "int8_t");
F3D_DECLARE_OG_TRAITS(uint8_t,   F3DUint8,   "uint8_t");
F3D_DECLARE_OG_TRAITS(int16_t,   F3DInt16,   "int16_t");
F3D_DECLARE_OG_TRAITS(uint16_t,  F3DUint16,  "uint16_t");
F3D_DECLARE_OG_TRAITS(int32_t,   F3DInt32,   "int32_t");
F3D_DECLARE_OG_TRAITS(uint32_t,  F3DUint32,  "uint32_t");
F3D_DECLARE_OG_TRAITS(int64_t,   F3DInt64,   "int64_t");
F3D_DECLARE_OG_TRAITS(uint64_t,  F3DUint64,  "uint64_t");
F3D_DECLARE_OG_TRAITS(float16_t, F3DFloat16, "float16_t");
F3D_DECLARE_OG_TRAITS(float32_t, F3DFloat32, "float32_t");
F3D_DECLARE_OG_TRAITS(float64_t, F3DFloat64, "float64_t");
F3D_DECLARE_OG_TRAITS(vec16_t,   F3DVec16,   "vec16_t");
F3D_DECLARE_OG_TRAITS(vec32_t,   F3DVec32,   "vec32_t");
F3D_DECLARE_OG_TRAITS(vec64_t,   F3DVec64,   "vec64_t");

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

//! Gets a string representation of the OgGroupType enum
const char* ogGroupTypeToString(OgGroupType type);

//----------------------------------------------------------------------------//

//! Reads a string
bool readString(Alembic::Ogawa::IGroupPtr group, const size_t idx, 
                std::string &s);

//----------------------------------------------------------------------------//

//! Reads a single value
template <typename T>
bool readData(Alembic::Ogawa::IGroupPtr group, const size_t idx, 
              T &value) 
{
  // Grab data
  Alembic::Ogawa::IDataPtr data = group->getData(idx, OGAWA_THREAD);
  // Check data length
  const size_t sizeLength = sizeof(T);
  const size_t length = data->getSize();
  if (length != sizeLength) {
    return false;
  }
  // Read the data directly to the input param
  data->read(length, &value, 0, OGAWA_THREAD);
  // Done
  return true;
}

//----------------------------------------------------------------------------//

//! Reads a single data type value
OgDataType readDataType(Alembic::Ogawa::IGroupPtr group, const size_t idx);

//----------------------------------------------------------------------------//

//! Lowest-level, writes directly to Ogawa Data.
void writeString(Alembic::Ogawa::OGroupPtr group, const std::string &s);

//----------------------------------------------------------------------------//

//! Lowest-level, writes directly to Ogawa Data.
template <typename T>
void writeData(Alembic::Ogawa::OGroupPtr group, const T &value)
{
  group->addData(sizeof(T), &value);
}

//----------------------------------------------------------------------------//

//! Lowest-level, writes directly to Ogawa Data.
template <typename T>
void writeDataType(Alembic::Ogawa::OGroupPtr group)
{
  OgDataType dataType = OgawaTypeTraits<T>::typeEnum();
  group->addData(sizeof(OgDataType), &dataType);
}

//----------------------------------------------------------------------------//

//! Finds the group name. Really just a wrapper around readString() for the
//! first data set in the group.
void getGroupName(Alembic::Ogawa::IGroupPtr group, 
                  std::string &name);

//----------------------------------------------------------------------------//
// OgIBase
//----------------------------------------------------------------------------//

//! Provides a few standard member functions and data members
class OgIBase
{
public:

  // Ctors, dtor ---------------------------------------------------------------

  //! No initialization - leaves the group pointer null
  OgIBase()
  { /* Empty */ }
  //! Initializes the group pointer
  OgIBase(Alembic::Ogawa::IGroupPtr group)
    : m_group(group)
  { /* Empty */ }

  // Main methods --------------------------------------------------------------

  //! Whether the group is valid
  bool               isValid() const
  { return m_group != NULL; }
  //! Returns the name
  const std::string& name() const
  { return m_name; }

protected:

  // Data members --------------------------------------------------------------

  Alembic::Ogawa::IGroupPtr m_group;
  std::string               m_name;

};

//----------------------------------------------------------------------------//

#endif // include guard

//----------------------------------------------------------------------------//