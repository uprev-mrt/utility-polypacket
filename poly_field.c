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
    case TYPE_UINT8:
      new_desc->mObjSize=sizeof(uint8_t);
      break;
    case TYPE_UINT16:
      new_desc->mObjSize=sizeof(uint16_t);
      break;
    case TYPE_UINT32:
      new_desc->mObjSize=sizeof(uint32_t);
      break;
    case TYPE_UINT64:
      new_desc->mObjSize=sizeof(uint64_t);
      break;
    case TYPE_INT8:
      new_desc->mObjSize=sizeof(int8_t);
      break;
    case TYPE_INT16:
      new_desc->mObjSize=sizeof(int16_t);
      break;
    case TYPE_INT32:
    case TYPE_INT:
      new_desc->mObjSize=sizeof(int32_t);
      break;
    case TYPE_INT64:
      new_desc->mObjSize=sizeof(int64_t);
      break;
    case TYPE_FLOAT:
      new_desc->mObjSize=sizeof(float);
      break;
    case TYPE_DOUBLE:
      new_desc->mObjSize=sizeof(double);
      break;
    case TYPE_STRING:
    case TYPE_CHAR:
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

void poly_field_copy(poly_field_t* src, poly_field_t* dst)
{
  //must be same type to copy
  if(src->mDesc != dst->mDesc)
    return;

  memcpy(dst->mData, src->mData, src->mSize);
  dst->mSize = src->mSize;
  dst->mPresent = src->mPresent;
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


  //for ascii format we just print the char string
  if(field->mDesc->mFormat == FORMAT_ASCII)
  {
    idx+= sprintf(&buf[idx],"\"%s\"", field->mData);
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
      idx+= sprintf(&buf[idx],"[ ");

      for(int i=0; i < elementCount; i++)
      {
        //add before all but the first
        if(i != 0)
          idx+= sprintf(&buf[idx],",");

        idx+= poly_field_print_val(field, i, &buf[idx]);
      }

      idx+= sprintf(&buf[idx]," ]");
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
        idx+= sprintf(&buf[idx], "%u", *(uint8_t*)pData);
        break;
      case TYPE_UINT16:
        idx+= sprintf(&buf[idx], "%u", *(uint16_t*)pData);
        break;
      case TYPE_UINT32:
        idx+= sprintf(&buf[idx], "%u", *(uint32_t*)pData);
        break;
      case TYPE_UINT64:
        idx+= sprintf(&buf[idx], "%u", *(uint64_t*)pData);
        break;
      case TYPE_INT8:
        idx+= sprintf(&buf[idx], "%i", *(int8_t*)pData);
        break;
      case TYPE_INT16:
        idx+= sprintf(&buf[idx], "%i", *(int16_t*)pData);
        break;
      case TYPE_INT32:
      case TYPE_INT:
        idx+= sprintf(&buf[idx], "%i", *(int32_t*)pData);
        break;
      case TYPE_INT64:
        idx+= sprintf(&buf[idx], "%i", *(int64_t*)pData);
        break;
      case TYPE_FLOAT:
        idx+= sprintf(&buf[idx], "%f", *(float*)pData);
        break;
      case TYPE_DOUBLE:
        idx+= sprintf(&buf[idx], "%f", *(double*)pData);
        break;
      default:
        break;
    }
  }
  else if(field->mDesc->mFormat == FORMAT_HEX)
  {
    idx+= sprintf(&buf[idx], "\"x");
    for(int i=0; i < field->mDesc->mObjSize; i++)
    {
      idx+= sprintf(&buf[idx], "%02X", pData[i]);
    }
    idx+= sprintf(&buf[idx], "\"");
  }

  return idx;
}
