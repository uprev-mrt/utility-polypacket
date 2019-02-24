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

typedef enum FieldFormat {
  FORMAT_DEC =0,
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
  *@param objSize size of single element in field
  *@param len number of elements in field
  *@param format format type for stringifying
  *@return ptr to new field descriptor
  */
poly_field_desc_t* new_poly_field_desc(const char* name, int objSize, uint32_t len, eFieldFormat format);

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
  *@returns size of parsed data
  */
int poly_field_parse(poly_field_t* field, uint8_t* data);
