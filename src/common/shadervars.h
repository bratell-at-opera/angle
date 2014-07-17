//
// Copyright (c) 2013-2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// shadervars.h:
//  Types to represent GL variables (varyings, uniforms, etc)
//

#ifndef COMMON_SHADERVARIABLE_H_
#define COMMON_SHADERVARIABLE_H_

#include <string>
#include <vector>
#include <algorithm>
#include "GLSLANG/ShaderLang.h"

namespace sh
{

// Varying interpolation qualifier, see section 4.3.9 of the ESSL 3.00.4 spec
enum InterpolationType
{
    INTERPOLATION_SMOOTH,
    INTERPOLATION_CENTROID,
    INTERPOLATION_FLAT
};

// Uniform block layout qualifier, see section 4.3.8.3 of the ESSL 3.00.4 spec
enum BlockLayoutType
{
    BLOCKLAYOUT_STANDARD,
    BLOCKLAYOUT_PACKED,
    BLOCKLAYOUT_SHARED
};

// Base class for all variables defined in shaders, including Varyings, Uniforms, etc
// Note: we must override the copy constructor and assignment operator so we can
// work around excessive GCC binary bloating:
// See https://code.google.com/p/angleproject/issues/detail?id=697
struct ShaderVariable
{
    ShaderVariable();
    ShaderVariable(const ShaderVariable &other);
    ShaderVariable &operator=(const ShaderVariable &other);

    ShaderVariable(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn)
        : type(typeIn),
          precision(precisionIn),
          name(nameIn),
          arraySize(arraySizeIn),
          staticUse(false)
    {}

    bool isArray() const { return arraySize > 0; }
    unsigned int elementCount() const { return std::max(1u, arraySize); }

    GLenum type;
    GLenum precision;
    std::string name;
    std::string mappedName;
    unsigned int arraySize;
    bool staticUse;
};

// Uniform registers (and element indices) are assigned when outputting shader code
struct Uniform : public ShaderVariable
{
    Uniform();
    Uniform(const Uniform &other);
    Uniform &operator=(const Uniform &other);
    bool isStruct() const { return !fields.empty(); }

    std::vector<Uniform> fields;

    // HLSL-specific members
    unsigned int registerIndex;
    unsigned int elementIndex; // Offset within a register, for struct members
};

struct Attribute : public ShaderVariable
{
    Attribute();
    Attribute(const Attribute &other);
    Attribute &operator=(const Attribute &other);

    Attribute(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, int locationIn)
      : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
        location(locationIn)
    {}

    int location;
};

struct InterfaceBlockField : public ShaderVariable
{
    InterfaceBlockField();
    InterfaceBlockField(const InterfaceBlockField &other);
    InterfaceBlockField &operator=(const InterfaceBlockField &other);

    InterfaceBlockField(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, bool isRowMajorMatrix)
        : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
          isRowMajorMatrix(isRowMajorMatrix)
    {}

    bool isStruct() const { return !fields.empty(); }

    bool isRowMajorMatrix;
    std::vector<InterfaceBlockField> fields;
};

struct Varying : public ShaderVariable
{
    Varying();
    Varying(const Varying &other);
    Varying &operator=(const Varying &other);

    Varying(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, InterpolationType interpolationIn)
        : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
          interpolation(interpolationIn)
    {}

    bool isStruct() const { return !fields.empty(); }

    InterpolationType interpolation;
    std::vector<Varying> fields;
    std::string structName;
};

struct BlockMemberInfo
{
    BlockMemberInfo(int offset, int arrayStride, int matrixStride, bool isRowMajorMatrix)
        : offset(offset),
          arrayStride(arrayStride),
          matrixStride(matrixStride),
          isRowMajorMatrix(isRowMajorMatrix)
    {}

    static BlockMemberInfo getDefaultBlockInfo()
    {
        return BlockMemberInfo(-1, -1, -1, false);
    }

    int offset;
    int arrayStride;
    int matrixStride;
    bool isRowMajorMatrix;
};

typedef std::vector<BlockMemberInfo> BlockMemberInfoArray;

struct InterfaceBlock
{
    InterfaceBlock();
    InterfaceBlock(const InterfaceBlock &other);
    InterfaceBlock &operator=(const InterfaceBlock &other);

    InterfaceBlock(const char *name, unsigned int arraySize)
        : name(name),
          arraySize(arraySize),
          layout(BLOCKLAYOUT_SHARED),
          isRowMajorLayout(false),
          staticUse(false)
    {}

    std::string name;
    std::string mappedName;
    unsigned int arraySize;
    BlockLayoutType layout;
    bool isRowMajorLayout;
    bool staticUse;
    std::vector<InterfaceBlockField> fields;
    std::vector<BlockMemberInfo> blockInfo;
};

}

#endif // COMMON_SHADERVARIABLE_H_
