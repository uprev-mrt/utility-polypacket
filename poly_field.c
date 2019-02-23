/**
  *@file poly_field.c
  *@brief implementation of variable fields
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "poly_field.h"

poly_field_desc_t* new_poly_field_desc(const char* name, int objSize, uint32_t len, eFieldFormat format)
{
  poly_field_desc_t* new_desc = (poly_field_desc_t*) malloc(sizeof(poly_field_desc_t));
  new_desc->mName = name;
  new_desc->mFormat = format;
  new_desc->mObjSize = objSize;
  new_desc->mLen = len;
  new_desc->mVarLen = false;
  new_desc->mNullTerm = false;

  return new_desc;
}

void poly_field_init(poly_field_t* field, poly_field_desc_t* desc)
{
  field->mSize = desc->mObjSize * desc->mLen;
  field->mDesc = desc;
  field->mPresent = false;
}

void poly_field_bind(poly_field_t* field, uint8_t* data)
{
  field->mData = data;
}

int poly_field_parse(poly_field_t* field, uint8_t* data)
{
  int idx =0;

  //if field is variable length, the first byte is the length
  if(field->mDesc->mVarLen)
  {
    field->mSize = data[idx++];
  }

  memcpy(field->mData, &data[idx], field->mSize);
  idx+= field->mSize;

  return idx;
}
