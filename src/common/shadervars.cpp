//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// shadervars.cpp:
//  Methods for GL variable types (varyings, uniforms, etc)
//

#include "common/shadervars.h"

namespace sh
{

ShaderVariable::ShaderVariable()
    : type(0),
      precision(0),
      arraySize(0),
      staticUse(false)
{}

ShaderVariable::ShaderVariable(const ShaderVariable &other)
    : type(other.type),
      precision(other.precision),
      name(other.name),
      mappedName(other.mappedName),
      arraySize(other.arraySize),
      staticUse(other.staticUse)
{}

void ShaderVariable::operator=(const ShaderVariable &other)
{
    type = other.type;
    precision = other.precision;
    name = other.name;
    mappedName = other.mappedName;
    arraySize = other.arraySize;
    staticUse = other.staticUse;
}

  ShaderVariable::ShaderVariable(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn)
        : type(typeIn),
          precision(precisionIn),
          name(nameIn),
          arraySize(arraySizeIn),
          staticUse(false)
    {}

  ShaderVariable::~ShaderVariable()
  {}


Uniform::Uniform()
{}

Uniform::Uniform(const Uniform &other)
    : ShaderVariable(other),
      fields(other.fields)
{}

void Uniform::operator=(const Uniform &other)
{
    ShaderVariable::operator=(other);
    fields = other.fields;
}

  Uniform::~Uniform()
  {}

Attribute::Attribute()
    : location(-1)
{}

Attribute::Attribute(const Attribute &other)
    : ShaderVariable(other),
      location(other.location)
{}

void Attribute::operator=(const Attribute &other)
{
    ShaderVariable::operator=(other);
    location = other.location;
}
  Attribute::Attribute(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, int locationIn)
      : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
        location(locationIn)
    {}

  Attribute::~Attribute()
  {}

InterfaceBlockField::InterfaceBlockField()
    : isRowMajorMatrix(false)
{}

InterfaceBlockField::InterfaceBlockField(const InterfaceBlockField &other)
    : ShaderVariable(other),
      isRowMajorMatrix(other.isRowMajorMatrix),
      fields(other.fields)
{}

  InterfaceBlockField::~InterfaceBlockField()
  {}

void InterfaceBlockField::operator=(const InterfaceBlockField &other)
{
    ShaderVariable::operator=(other);
    isRowMajorMatrix = other.isRowMajorMatrix;
    fields = other.fields;
}

  InterfaceBlockField::InterfaceBlockField(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, bool isRowMajorMatrix)
        : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
          isRowMajorMatrix(isRowMajorMatrix)
    {}

  

Varying::Varying()
    : interpolation(INTERPOLATION_SMOOTH)
{}

Varying::Varying(const Varying &other)
    : ShaderVariable(other),
      interpolation(other.interpolation),
      fields(other.fields),
      structName(other.structName)
{}
Varying::~Varying()
{}

void Varying::operator=(const Varying &other)
{
    ShaderVariable::operator=(other);
    interpolation = other.interpolation;
    fields = other.fields;
    structName = other.structName;
}

  Varying::Varying(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, InterpolationType interpolationIn)
        : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
          interpolation(interpolationIn)
    {}
InterfaceBlock::InterfaceBlock()
    : arraySize(0),
      layout(BLOCKLAYOUT_PACKED),
      isRowMajorLayout(false),
      staticUse(false)
{}

InterfaceBlock::InterfaceBlock(const InterfaceBlock &other)
    : name(other.name),
      mappedName(other.mappedName),
      arraySize(other.arraySize),
      layout(other.layout),
      isRowMajorLayout(other.isRowMajorLayout),
      staticUse(other.staticUse),
      fields(other.fields)
{}
InterfaceBlock::~InterfaceBlock()
{}

void InterfaceBlock::operator=(const InterfaceBlock &other)
{
    name = other.name;
    mappedName = other.mappedName;
    arraySize = other.arraySize;
    layout = other.layout;
    isRowMajorLayout = other.isRowMajorLayout;
    staticUse = other.staticUse;
    fields = other.fields;
}
  InterfaceBlock::InterfaceBlock(const char *name, unsigned int arraySize)
        : name(name),
          arraySize(arraySize),
          layout(BLOCKLAYOUT_SHARED),
          isRowMajorLayout(false),
          staticUse(false)
    {}


}
