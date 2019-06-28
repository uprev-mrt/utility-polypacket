/**
  *@file poly_field.c
  *@brief implementation of variable fields
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "poly_field.h"
//#include <cstdlib>
#include <assert.h>

#define MEM_EXISTS( field) ((field->mAllocated) || (field->mBound))

#define PARSE_U(x,s,b)  x = strtoul(s, NULL, b);  \
                          poly_field_set(field, (uint8_t*) &x)

#define PARSE_I(x,s,b)  x = strtol(s, NULL, b);  \
                          poly_field_set(field, (uint8_t*) &x)

poly_field_desc_t* poly_field_desc_init(poly_field_desc_t* desc, const char* name, eFieldType type, uint32_t len, eFieldFormat format)
{
  desc->mName = name;
  desc->mNameLen = strlen(name);
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
    memset(field->mData,0,field->mSize);
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

    if(strlen((const char*)data) > field->mDesc->mLen)
    {
      field->mSize = field->mDesc->mLen;
    }
    field->mData[field->mSize] = 0;
  }

  field->mPresent =true;

  memcpy(field->mData, data, field->mSize);
}

int poly_field_get(poly_field_t* field, uint8_t* data)
{
  assert(MEM_EXISTS(field));
  int ret =0;


  if(data != NULL)
  {
    memcpy(data, field->mData, field->mSize);
    ret = field->mSize;

    if(field->mDesc->mNullTerm)
    {
      data[field->mSize] =0;
    }
  }


  return ret;
}

int poly_field_copy(poly_field_t* dst,poly_field_t* src)
{
  //if descriptor is the same and the src has data
  if((dst->mDesc == src->mDesc) && (src->mPresent))
  {
    assert(MEM_EXISTS(dst));
    assert(MEM_EXISTS(src));

    poly_field_set(dst, src->mData);
    return 1;
  }
  return 0;
}

int poly_field_parse(poly_field_t* field, const uint8_t* data)
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

int poly_field_parse_str(poly_field_t* field, const char* str)
{

  uint8_t u8tmp;
  uint16_t u16tmp;
  uint32_t u32tmp;
  uint64_t u64tmp;

  int8_t i8tmp;
  int16_t i16tmp;
  int32_t i32tmp;
  int64_t i64tmp;

  float ftmp;
  double dtmp;

  int base = 10;

  if(field->mDesc->mFormat == FORMAT_HEX)
  {
    base = 16;
  }
  //copy string
  if((field->mDesc->mNullTerm) || (field->mDesc->mDataType == TYPE_CHAR))
  {
    poly_field_set(field, (uint8_t*)str);
    return 0 ;
  }

  //parse data types
  switch(field->mDesc->mDataType)
  {
    case TYPE_UINT8:
      PARSE_U( u8tmp,str,base);
      break;
    case TYPE_UINT16:
      PARSE_U( u16tmp,str,base);
      break;
    case TYPE_UINT32:
      PARSE_U( u32tmp,str,base);
      break;
    case TYPE_UINT64:
      PARSE_U(u64tmp,str,base);
      break;
    case TYPE_INT8:
      PARSE_I(i8tmp,str,base);
      break;
    case TYPE_INT16:
      PARSE_I(i16tmp,str,base);
      break;
    case TYPE_INT32:
    case TYPE_INT:
      PARSE_I(i32tmp,str,base);
      break;
    case TYPE_INT64:
      PARSE_I(i64tmp,str,base);
      break;
    case TYPE_FLOAT:
      ftmp = atof(str);
      poly_field_set(field, (uint8_t*) &ftmp);
      break;
    case TYPE_DOUBLE:
      dtmp = atof(str);
      poly_field_set(field, (uint8_t*) &ftmp);
      break;
    case TYPE_STRING:
    case TYPE_CHAR:
      break;
  }

  return 0;
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
        idx+= MRT_SPRINTF(&buf[idx], "%llu", *(uint64_t*)pData);
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
        idx+= MRT_SPRINTF(&buf[idx], "%lli", *(int64_t*)pData);
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
    idx+= MRT_SPRINTF(&buf[idx], "\"0x");
    for(int i=0; i < field->mDesc->mObjSize; i++)
    {
      idx+= MRT_SPRINTF(&buf[idx], "%02X", pData[i]);
    }
    idx+= MRT_SPRINTF(&buf[idx], "\"");
  }

  return idx;
}
