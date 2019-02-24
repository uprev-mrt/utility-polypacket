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

typedef enum FieldFormat {
  FORMAT_DEFAULT,
  FORMAT_DEC,
  FORMAT_HEX,
  FORMAT_ASCII
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
}poly_field_t;


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
  *@param data ptr to store data
  */
void poly_field_init(poly_field_t* field, poly_field_desc_t* desc);

/**
  *@brief binds poly field to data in memory
  *@param field ptr to field
  *@param data ptr to data in memory
  */
void poly_field_bind(poly_field_t* field, uint8_t* data);

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
