/**
  *@file var_field.c
  *@brief implementation of variable fields
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "var_field.h"

field_desc_t* new_field_desc(const char* name, int objSize, uint32_t len, bool req, eFieldFormat format)
{
  static int id=0;
  field_desc_t* new_desc = (field_desc_t*) malloc(sizeof(field_desc_t));
  new_desc->mName = name;
  new_desc->mHash = id++; //TODO use actual hashing
  new_desc->mRequired = req;
  new_desc->mFormat = format;
  new_desc->mObjSize = objSize;
  new_desc->mLen = len;
  new_desc->mVarSize = false;

  return new_desc;
}

var_field_t* new_field(field_desc_t* desc)
{
  var_field_t* field = (var_field_t*) malloc(sizeof(var_field_t));
  field->mSize = desc->mObjSize * desc->mLen
  field->mBuffer = (uint8_t*) malloc(field->mSize);
  field->mDesc = desc;
  field->mPresent = false;

  return field;
}

void init_field(var_field_t* field, field_desc_t* desc)
{
  field.mSize = desc->mObjSize * desc->mLen
  field.mBuffer = (uint8_t*) malloc(field->mSize);
  field.mDesc = desc;
  field.mPresent = false;
}

void var_field_set(var_field_t* field, void* val)
{
  memcpy((void*)field->mBuffer, val, field->mDesc->mObjSize);
}


void var_field_set_idx(var_field_t* field, uint32_t idx, void* val )
{
  memcpy((void*)&field->mBuffer[field->mDesc->mObjSize * idx], val, field->mDesc->mObjSize);
}


void var_field_set_buf(var_field_t* field, void* val, int len)
{
  //if it is a variable len field, we free and resize it before setting
  if(field->mDesc->mVarLen)
  {
    free(field->mBuffer);
    field->mBuffer = (uint8_t*) malloc(len);
    field->mSize = len;
  }
  else
  {
    if(len > field->mDesc->mObjSize)
    {
      len = field->mDesc->mObjSize;
    }
  }

  memcpy((void*)field->mBuffer, val, len);
}


void var_field_get(var_field_t*field, void* val)
{
  memcpy(val,(void*)field->mBuffer, field->mDesc->mObjSize);
}

void var_field_get_idx(var_field_t* field, uint32_t idx, void* val )
{
    memcpy(val,(void*)field->mBuffer[field->mDesc->mObjSize * idx] , field->mDesc->mObjSize);
}


int var_field_get_buf(var_field_t* field, void* var)
{
  memcpy(val,(void*)field->mBuffer, field->mSize);
  return field->mSize;
}

int var_field_parse(var_field_t* field, uint8_t * data)
{
  uint8_t cursor=0;
  uint8_t newSize;

  //if its is a variable len field, we may need to reallocate the memory
  if(field->mDesc->mVarLen)
  {
    newSize = data[cursor++];
    if(field->mSize != newSize)
    {
      field->mSize = newSize;
      free(field->mBuffer);
      field->mBuffer = (uin8_t*)malloc(field->mSize);
    }

    memcpy((void*)field->mBuffer, (void*)&data[cursor], field->mSize );

    return cursor + field->mSize;
  }



}
