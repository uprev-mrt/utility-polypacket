/**
  *@file poly_field.h
  *@brief header for c implementation of variable packet field
  *@author Jason Berger
  *@date 02/19/2019
  */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef MRT_SPRINTF
#define MRT_SPRINTF(f_, ...) sprintf((f_), __VA_ARGS__)
#endif

//Seed the checksum so even in packets with no data (acks) still have a partial check
#define CHECKSUM_SEED 1738

#pragma pack(push)
#pragma pack(1)

typedef enum FieldType {
  TYPE_UINT8,
  TYPE_UINT16,
  TYPE_UINT32,
  TYPE_UINT64,
  TYPE_INT8,
  TYPE_INT16,
  TYPE_INT32,
  TYPE_INT,
  TYPE_INT64,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_STRING,
  TYPE_CHAR
}eFieldType;

#define TYPE_UINT8_T TYPE_UINT8
#define TYPE_UINT16_T TYPE_UINT16
#define TYPE_UINT32_T TYPE_UINT32
#define TYPE_UINT64_T TYPE_UINT64
#define TYPE_INT8_T TYPE_INT8
#define TYPE_INT16_T TYPE_INT16
#define TYPE_INT32_T TYPE_INT32
#define TYPE_INT64_T TYPE_INT64


//Format used when printing field to json 
typedef enum FieldFormat {
  FORMAT_DEFAULT,
  FORMAT_DEC,
  FORMAT_HEX,
  FORMAT_ASCII,
  FORMAT_NONE
}eFieldFormat;

/**
  *@brief Struct for variable field descriptor
  */
typedef struct{
  const char* mName;            //friendly name of field
  eFieldFormat mFormat;   //format used for printing and jsonifying field
  uint8_t mObjSize;       //size of object in field
  eFieldType mDataType;
  uint16_t mLen;          //number of objects in field (max)
  bool mVarLen;           //indicates if field is of variable length
  bool mNullTerm;         //indicates field is null terminated
}poly_field_desc_t;

typedef struct{
  uint8_t* mData;       //buffer where field data is stored
  poly_field_desc_t* mDesc;    //pointer to descriptor
  uint8_t mSize;         //actual size (used for variable len fields)
  bool mPresent;          //indicated field is present in packet
  bool mBound;
  bool mAllocated;
}poly_field_t;

#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif

/**
  *@brief Creates a new field descriptor
  *@param name friendly name for field
  *@param type type of data
  *@param len number of elements in field
  *@param format format type for stringifying
  *@return ptr to new field descriptor
  */
poly_field_desc_t* new_poly_field_desc(const char* name, eFieldType type, uint32_t len, eFieldFormat format);

/**
  *@brief initializes a new field
  *@param field ptr to field
  *@param desc ptr to field descriptor
  *@param allocate whether or not to allocate memory
  *@param data ptr to store data
  */
void poly_field_init(poly_field_t* field, poly_field_desc_t* desc, bool allocate);

/**
  *@brief frees memory from poly_field
  *@param field ptr to field
  */
void poly_field_destroy(poly_field_t* field);

/**
  *@brief binds poly field to data in memory
  *@param field ptr to field
  *@param data ptr to data in memory
  *@param overwrite indicates whether or not the allocated memory is copied over to the new location
  */
void poly_field_bind(poly_field_t* field, uint8_t* data, bool copy);

/**
  *@brief sets data in field
  *@param field ptr to field
  *@param data ptr to data
  */
void poly_field_set(poly_field_t* field, const uint8_t* data);

/**
  *@brief copies data from field and returns pointer (for use with arrays)
  *@param field ptr to field
  *@returns pointer to data
  */
uint8_t* poly_field_get(poly_field_t* field, uint8_t* data);

/**
  *@brief Parses raw data for a field
  *@param field ptr to field being parsed
  *@param data ptr to raw data to be parsed
  *@param printMeta show meta data in json
  *@returns size of parsed data
  */
int poly_field_parse(poly_field_t* field, uint8_t* data);

/**
  *@brief prints out field in json format
  *@param field ptr to field
  *@param buf buffer to print to
  *@return return number of characters written
  */
int poly_field_print_json(poly_field_t* field, char* buf);

/**
  *@brief prints out an element in the field data
  *@param field ptr to field
  *@param ind idx index of element to print
  *@param buf buffer to print to
  *@return return number of characters written
  */
int poly_field_print_val(poly_field_t* field, int element, char* buf);

#ifdef __cplusplus
}
#endif
