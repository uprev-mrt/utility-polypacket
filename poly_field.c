/**
  *@file poly_field.c
  *@brief implementation of variable fields
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "poly_field.h"
#include <assert.h>

#define MEM_EXISTS( field) ((field->mAllocated) || (field->mBound))

poly_field_desc_t* poly_field_desc_init(poly_field_desc_t* desc, const char* name, eFieldType type, uint32_t len, eFieldFormat format)
{
  desc->mName = name;
  desc->mFormat = FORMAT_DEC;
  desc->mLen = len;
  desc->mDataType = type;
  if(len > 1)
  {
    desc->mVarLen = true;
  }
  else
  {
    desc->mVarLen = false;
  }
  desc->mNullTerm = false;
  switch(type)
  {
    case TYPE_UINT8:
      desc->mObjSize=sizeof(uint8_t);
      break;
    case TYPE_UINT16:
      desc->mObjSize=sizeof(uint16_t);
      break;
    case TYPE_UINT32:
      desc->mObjSize=sizeof(uint32_t);
      break;
    case TYPE_UINT64:
      desc->mObjSize=sizeof(uint64_t);
      break;
    case TYPE_INT8:
      desc->mObjSize=sizeof(int8_t);
      break;
    case TYPE_INT16:
      desc->mObjSize=sizeof(int16_t);
      break;
    case TYPE_INT32:
    case TYPE_INT:
      desc->mObjSize=sizeof(int32_t);
      break;
    case TYPE_INT64:
      desc->mObjSize=sizeof(int64_t);
      break;
    case TYPE_FLOAT:
      desc->mObjSize=sizeof(float);
      break;
    case TYPE_DOUBLE:
      desc->mObjSize=sizeof(double);
      break;
    case TYPE_STRING:
      desc->mNullTerm = true;
    case TYPE_CHAR:
      desc->mObjSize=sizeof(char);  //strings are treated as arrays of characters
      desc->mFormat = FORMAT_ASCII;
      break;
    default:
      break;
  }

  if(format != FORMAT_DEFAULT)
  {
    desc->mFormat = format;
  }

  return desc;
}

void poly_field_init(poly_field_t* field, poly_field_desc_t* desc, bool allocate)
{
  field->mSize = desc->mObjSize * desc->mLen;
  field->mDesc = desc;
  field->mPresent = false;
  field->mBound = false;
  if(allocate)
  {
    field->mData = (uint8_t*) malloc(field->mSize);
    field->mAllocated = true;
  }
  else
  {
    field->mAllocated = false;
  }
}

void poly_field_destroy(poly_field_t* field)
{
  //if we allocated our own memory, free it
  if(field->mAllocated)
  {
    free(field->mData);
  }
}

void poly_field_bind(poly_field_t* field, uint8_t* data, bool copy)
{
  //if data is alread present, copy it first
  if((field->mAllocated || field->mBound) && copy)
  {
    memcpy(data, field->mData, field->mSize);
  }

  //if data was allocated free the old
  if(field->mAllocated)
  {
    free(field->mData);
    field->mAllocated = false;
  }

  field->mData = data;
  field->mBound = true;
}

void poly_field_set(poly_field_t* field, const uint8_t* data)
{
  assert(MEM_EXISTS(field));

  //if its a null terminated type, we can adjust size
  if(field->mDesc->mNullTerm)
  {
    field->mSize = strlen((const char*)data);
  }

  field->mPresent =true;

  memcpy(field->mData, data, field->mSize);
}

uint8_t* poly_field_get(poly_field_t* field, uint8_t* data)
{
  assert(MEM_EXISTS(field));

  memcpy(data, field->mData, field->mSize);

  return field->mData;
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

  idx+= MRT_SPRINTF(&buf[idx],"\"%s\" : ", field->mDesc->mName);


  //for ascii format we just print the char string
  if(field->mDesc->mFormat == FORMAT_ASCII)
  {
    idx+= MRT_SPRINTF(&buf[idx],"\"%s\"", field->mData);
  }
  //for other formats we have to specialize the printing
  else
  {
    elementCount = field->mSize / field->mDesc->mObjSize;

    //If field contains a single element, print out json
    if(elementCount == 1)
    {
      idx+= poly_field_print_val(field, 0, &buf[idx]);
    }
    //If field is an array, print as JSON array
    else
    {
      idx+= MRT_SPRINTF(&buf[idx],"[ ");

      for(int i=0; i < elementCount; i++)
      {
        //add before all but the first
        if(i != 0)
          idx+= MRT_SPRINTF(&buf[idx],",");

        idx+= poly_field_print_val(field, i, &buf[idx]);
      }

      idx+= MRT_SPRINTF(&buf[idx]," ]");
    }
  }
  return idx;
}

int poly_field_print_val(poly_field_t* field, int element, char* buf)
{
  int idx =0;
  uint8_t* pData = &field->mData[field->mDesc->mObjSize * element];
  if(field->mDesc->mFormat == FORMAT_DEC)
  {
    switch(field->mDesc->mDataType)
    {
      case TYPE_UINT8:
      case TYPE_CHAR:
        idx+= MRT_SPRINTF(&buf[idx], "%u", *(uint8_t*)pData);
        break;
      case TYPE_UINT16:
        idx+= MRT_SPRINTF(&buf[idx], "%u", *(uint16_t*)pData);
        break;
      case TYPE_UINT32:
        idx+= MRT_SPRINTF(&buf[idx], "%u", *(uint32_t*)pData);
        break;
      case TYPE_UINT64:
        idx+= MRT_SPRINTF(&buf[idx], "%u", *(uint64_t*)pData);
        break;
      case TYPE_INT8:
        idx+= MRT_SPRINTF(&buf[idx], "%i", *(int8_t*)pData);
        break;
      case TYPE_INT16:
        idx+= MRT_SPRINTF(&buf[idx], "%i", *(int16_t*)pData);
        break;
      case TYPE_INT32:
      case TYPE_INT:
        idx+= MRT_SPRINTF(&buf[idx], "%i", *(int32_t*)pData);
        break;
      case TYPE_INT64:
        idx+= MRT_SPRINTF(&buf[idx], "%i", *(int64_t*)pData);
        break;
      case TYPE_FLOAT:
        idx+= MRT_SPRINTF(&buf[idx], "%f", *(float*)pData);
        break;
      case TYPE_DOUBLE:
        idx+= MRT_SPRINTF(&buf[idx], "%f", *(double*)pData);
        break;
      default:
        break;
    }
  }
  else if(field->mDesc->mFormat == FORMAT_HEX)
  {
    idx+= MRT_SPRINTF(&buf[idx], "\"x");
    for(int i=0; i < field->mDesc->mObjSize; i++)
    {
      idx+= MRT_SPRINTF(&buf[idx], "%02X", pData[i]);
    }
    idx+= MRT_SPRINTF(&buf[idx], "\"");
  }

  return idx;
}
