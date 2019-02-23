/**
  *@file poly_field.c
  *@brief implementation of variable fields
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "poly_field.h"

field_desc_t* new_field_desc(const char* name, int objSize, uint32_t len, eFieldFormat format)
{
  static int id=0;
  field_desc_t* new_desc = (field_desc_t*) malloc(sizeof(field_desc_t));
  new_desc->mName = name;
  new_desc->mHash = id++; //TODO use actual hashing
  new_desc->mFormat = format;
  new_desc->mObjSize = objSize;
  new_desc->mLen = len;
  new_desc->mVarSize = false;

  return new_desc;
}

void poly_field_init(poly_field_t* field, poly_field_desc_t* desc, void* data)
{
  field->mSize = desc->mObjSize * desc->mLen
  field->mData = data
  field->mDesc = desc;
  field->mPresent = false;

  return field;
}

void poly_field_parse(poly_field_t* field, uint8_t* data)
{
  int idx =0;

  //if field is variable length, the first byte is the length
  if(field->mVarLen)
  {
    field->mSize = data[idx++];
  }

  memcpy(field->mData, &data[idx], field->mSize);
}
