//
// Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// mathutil.cpp: Math and bit manipulation functions.

#include "common/mathutil.h"
#include <algorithm>
#include <math.h>

namespace gl
{

struct RGB9E5Data
{
    unsigned int R : 9;
    unsigned int G : 9;
    unsigned int B : 9;
    unsigned int E : 5;
};

// B is the exponent bias (15)
static const int g_sharedexp_bias = 15;

// N is the number of mantissa bits per component (9)
static const int g_sharedexp_mantissabits = 9;

// Emax is the maximum allowed biased exponent value (31)
static const int g_sharedexp_maxexponent = 31;

static const float g_sharedexp_max = ((pow(2.0f, g_sharedexp_mantissabits) - 1) /
                                       pow(2.0f, g_sharedexp_mantissabits)) *
                                     pow(2.0f, g_sharedexp_maxexponent - g_sharedexp_bias);

unsigned int convertRGBFloatsTo999E5(float red, float green, float blue)
{
    const float red_c = std::max<float>(0, std::min(g_sharedexp_max, red));
    const float green_c = std::max<float>(0, std::min(g_sharedexp_max, green));
    const float blue_c = std::max<float>(0, std::min(g_sharedexp_max, blue));

    const float max_c = std::max<float>(std::max<float>(red_c, green_c), blue_c);
    const float exp_p = std::max<float>(-g_sharedexp_bias - 1, floor(log(max_c))) + 1 + g_sharedexp_bias;
    const int max_s = floor((max_c / (pow(2.0f, exp_p - g_sharedexp_bias - g_sharedexp_mantissabits))) + 0.5f);
    const int exp_s = (max_s < pow(2.0f, g_sharedexp_mantissabits)) ? exp_p : exp_p + 1;

    RGB9E5Data output;
    output.R = floor((red_c / (pow(2.0f, exp_s - g_sharedexp_bias - g_sharedexp_mantissabits))) + 0.5f);
    output.G = floor((green_c / (pow(2.0f, exp_s - g_sharedexp_bias - g_sharedexp_mantissabits))) + 0.5f);
    output.B = floor((blue_c / (pow(2.0f, exp_s - g_sharedexp_bias - g_sharedexp_mantissabits))) + 0.5f);
    output.E = exp_s;

    return *reinterpret_cast<unsigned int*>(&output);
}

void convert999E5toRGBFloats(unsigned int input, float *red, float *green, float *blue)
{
    const RGB9E5Data *inputData = reinterpret_cast<const RGB9E5Data*>(&input);

    *red = inputData->R * pow(2.0f, (int)inputData->E - g_sharedexp_bias - g_sharedexp_mantissabits);
    *green = inputData->G * pow(2.0f, (int)inputData->E - g_sharedexp_bias - g_sharedexp_mantissabits);
    *blue = inputData->B * pow(2.0f, (int)inputData->E - g_sharedexp_bias - g_sharedexp_mantissabits);
}

}

#include "common/shadervars.h"

namespace sh
{

  ShaderVariable::ShaderVariable(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn)
  : type(typeIn),
    precision(precisionIn),
    name(nameIn),
    arraySize(arraySizeIn),
    staticUse(false)
{}

ShaderVariable::ShaderVariable(const ShaderVariable& other)
  : type(other.type),
    precision(other.precision),
    name(other.name),
    mappedName(other.mappedName),
    arraySize(other.arraySize),
    staticUse(other.staticUse)
{}

void ShaderVariable::operator=(const ShaderVariable& other)
{
    type = other.type;
    precision = other.precision;
    name = other.name;
    mappedName = other.mappedName;
    arraySize = other.arraySize;
    staticUse = other.staticUse;
}


Uniform::Uniform()
  : ShaderVariable(0, 0, "", 0),
    fields(),
    registerIndex(-1),
    elementIndex(-1)
{}

Uniform::Uniform(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn,
        unsigned int registerIndexIn, unsigned int elementIndexIn)
  : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
    fields(),
    registerIndex(registerIndexIn),
    elementIndex(elementIndexIn)
{}

Uniform::Uniform(const Uniform& other)
  : ShaderVariable(other),
    fields(other.fields),
    registerIndex(other.registerIndex),
    elementIndex(other.elementIndex)
{
}

void Uniform::operator=(const Uniform& other)
{
    ShaderVariable::operator=(other);
    fields = other.fields;
    registerIndex = other.registerIndex;
    elementIndex = other.elementIndex;
}


Attribute::Attribute()
  : ShaderVariable(0, 0, "", 0),
    location(-1)
{}

Attribute::Attribute(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, int locationIn)
  : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
    location(locationIn)
{}

Attribute::Attribute(const Attribute& other)
  : ShaderVariable(other),
    location(other.location)
{}

void Attribute::operator=(const Attribute& other)
{
    ShaderVariable::operator=(other);
    location = other.location;
}


InterfaceBlockField::InterfaceBlockField(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, bool isRowMajorMatrix)
  : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
    isRowMajorMatrix(isRowMajorMatrix),
    fields()
{}

InterfaceBlockField::InterfaceBlockField(const InterfaceBlockField& other)
  : ShaderVariable(other),
    isRowMajorMatrix(other.isRowMajorMatrix),
    fields(other.fields)
{}

void InterfaceBlockField::operator=(const InterfaceBlockField& other)
{
    ShaderVariable::operator=(other);
    isRowMajorMatrix = other.isRowMajorMatrix;
    fields = other.fields;
}


Varying::Varying()
  : ShaderVariable(0, 0, "", 0),
    interpolation(INTERPOLATION_SMOOTH)
{}

Varying::Varying(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, InterpolationType interpolationIn)
  : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
    interpolation(interpolationIn),
    fields(),
    structName()
{}

Varying::Varying(const Varying& other)
  : ShaderVariable(other),
    interpolation(other.interpolation),
    fields(other.fields),
    structName(other.structName)
{}

void Varying::operator=(const Varying& other)
{
    ShaderVariable::operator=(other);
    interpolation = other.interpolation;
    fields = other.fields;
    structName = other.structName;
}


InterfaceBlock::InterfaceBlock(const char *name, unsigned int arraySize, unsigned int registerIndex)
  : name(name),
    arraySize(arraySize),
    dataSize(0),
    layout(BLOCKLAYOUT_SHARED),
    isRowMajorLayout(false),
    registerIndex(registerIndex),
    fields(),
    blockInfo()
{}

InterfaceBlock::InterfaceBlock(const InterfaceBlock& other)
  : name(other.name),
    arraySize(other.arraySize),
    dataSize(other.dataSize),
    layout(other.layout),
    registerIndex(other.registerIndex),
    isRowMajorLayout(other.isRowMajorLayout),
    fields(other.fields),
    blockInfo(other.blockInfo)
{}

void InterfaceBlock::operator=(const InterfaceBlock& other)
{
    name = other.name;
    arraySize = other.arraySize;
    dataSize = other.dataSize;
    layout = other.layout;
    registerIndex = other.registerIndex;
    isRowMajorLayout = other.isRowMajorLayout;
    fields = other.fields;
    blockInfo = other.blockInfo;
}

} // namespace sh
