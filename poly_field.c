/**
  *@file poly_field.c
  *@brief implementation of variable fields
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "poly_field.h"

poly_field_desc_t* new_poly_field_desc(const char* name, eFieldType type, uint32_t len, eFieldFormat format)
{
  poly_field_desc_t* new_desc = (poly_field_desc_t*) malloc(sizeof(poly_field_desc_t));
  new_desc->mName = name;
  new_desc->mFormat = FORMAT_DEC;
  new_desc->mLen = len;
  new_desc->mDataType = type;
  new_desc->mVarLen = false;
  new_desc->mNullTerm = false;
  switch(type)
  {
    TYPE_UINT8:
      new_desc->mObjSize=sizeof(uint8_t);
      break;
    TYPE_UINT16:
      new_desc->mObjSize=sizeof(uint16_t);
      break;
    TYPE_UINT32:
      new_desc->mObjSize=sizeof(uint32_t);
      break;
    TYPE_UINT64:
      new_desc->mObjSize=sizeof(uint64_t);
      break;
    TYPE_INT8:
      new_desc->mObjSize=sizeof(int8_t);
      break;
    TYPE_INT16:
      new_desc->mObjSize=sizeof(int16_t);
      break;
    TYPE_INT32:
    TYPE_INT:
      new_desc->mObjSize=sizeof(int32_t);
      break;
    TYPE_INT64:
      new_desc->mObjSize=sizeof(int64_t);
      break;
    TYPE_FLOAT:
      new_desc->mObjSize=sizeof(float);
      break;
    TYPE_DOUBLE:
      new_desc->mObjSize=sizeof(double);
      break;
    TYPE_STRING:
    TYPE_CHAR:
      new_desc->mObjSize=sizeof(char);  //strings are treated as arrays of characters
      new_desc->mFormat = FORMAT_ASCII;
      break;
    default:
      break;
  }

  if(format != FORMAT_DEFAULT)
  {
    new_desc->mFormat = format;
  }

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

int poly_field_print_json(poly_field_t* field, char* buf)
{
  int elementCount;
  int idx =0;

  idx+= sprintf(&buf[idx],"\"%s\" : ", field->mDesc->mName);
  if(field->mDesc->mFormat != FORMAT_ASCII)
  {
    elementCount = field->mSize / field->mDesc->mObjSize;
    if(elementCount == 1)
    {
      idx+= sprintf(&buf[idx],"\"%s\" : ", field->mDesc->mName);

    }
    else //print array
    {

    }
  }
}

int poly_field_print_val(poly_field_t* field, int element, char* buf)
{
  int idx =0;
  uint8_t* pData = &field->mData[field->mDesc->mObjSize * element];
  if(field->mDesc->mFormat == FORMAT_DEC)
  {
    switch(field->mDesc->mDataType)
    {
      TYPE_UINT8:
      TYPE_CHAR:
        idx+= sprintf(&buf[idx], " %u", *(uint8_t*)pData);
        break;
      TYPE_UINT16:
        idx+= sprintf(&buf[idx], " %u", *(uint16_t*)pData);
        break;
      TYPE_UINT32:
        idx+= sprintf(&buf[idx], " %u", *(uint32_t*)pData);
        break;
      TYPE_UINT64:
        idx+= sprintf(&buf[idx], " %u", *(uint64_t*)pData);
        break;
      TYPE_INT8:
        idx+= sprintf(&buf[idx], " %i", *(int8_t*)pData);
        break;
      TYPE_INT16:
        idx+= sprintf(&buf[idx], " %i", *(int16_t*)pData);
        break;
      TYPE_INT32:
      TYPE_INT:
        idx+= sprintf(&buf[idx], " %i", *(int32_t*)pData);
        break;
      TYPE_INT64:
        idx+= sprintf(&buf[idx], " %i", *(int64_t*)pData);
        break;
      TYPE_FLOAT:
        idx+= sprintf(&buf[idx], " %f", *(float*)pData);
        break;
      TYPE_DOUBLE:
        idx+= sprintf(&buf[idx], " %f", *(double*)pData);
        break;
      default:
        break;
    }
  }
  else if(field->mDesc->mFormat == FORMAT_HEX)
  {
    for(int i=0; i < field->mDesc->mObjSize; i++)
    {
      idx+= sprintf(&buf[idx]);
    }
  }
}
