/**
  *@file var_field.h
  *@brief header for c implementation of variable packet field
  *@author Jason Berger
  *@date 02/19/2019
  */

enum eFieldFormat {
  FORMAT_DEC =0,
  FORMAT_HEX,
  FORMAT_ASCII
};

/**
  *@brief Struct for variable field descriptor
  */
typedef struct{
  const char* mName;            //friendly name of field
  uin32_t mHash;          //unique identifier of field
  eFieldFormat mFormat;   //format used for printing and jsonifying field
  uint8_t mObjSize;       //size of object in field
  uint16_t mLen;          //number of objects in field (max)
  bool mVarLen;          //indicates that field can have variable sizes
}field_desc_t;

typedef struct{
  uint8_t* mBuffer;       //buffer where field data is stored
  uint16_t mSize;         //actual size (used for variable len fields)
  field_desc_t* mDesc;    //pointer to descriptor
  bool mPresent;          //indicated field is present in packet
}var_field_t;


/**
  *@brief Creates a new field descriptor
  *@param name friendly name for field
  *@param objSize size of single element in field
  *@param len number of elements in field
  *@return ptr to new field descriptor
  */
field_desc_t* new_field_desc(const char* name, int objSize, uint32_t len);

/**
  *@brief Creates a new field
  *@param desc ptr to field descriptor
  *@return ptr to new field
  */
var_field_t* new_field(field_desc_t* desc);

/**
  *@brief initialize existing field from descriptor
  *@param field ptr to field
  *@param desc ptr to field descriptor
  */
void init_field(var_field_t* field, field_desc_t* desc);

/**
  *@brief sets value of field
  *@param field ptr to field
  *@pre val ptr to value being set
  */
void var_field_set(var_field_t* field, void* val);

/**
  *@brief sets value of element in field
  *@param field ptr to field
  *@param idx index of element in field being set
  *@pre val ptr to value being set
  */
void var_field_set_idx(var_field_t* field, uint32_t idx, void* val );

/**
  *@brief sets values of field
  *@param field ptr to field
  *@pre val ptr to value being set
  *@pre len number of elements being set
  */
void var_field_set_buf(var_field_t* field, void* val, int len);


/**
  *@brief get field value
  *@param field ptr to field
  *@param val ptr to copy value to
  */
void var_field_get(var_field_t*field, void* val);

/**
  *@brief get value of element in field
  *@param field ptr to field
  *@param idx index of element to get
  *@param val ptr to copy value to
  */
void var_field_get_idx(var_field_t* field, uint32_t idx, void* val );

/**
  *@brief get buffer from field
  *@param field ptr to field
  *@param idx index of element to get
  *@param val ptr to copy value to
  */
int var_field_get_buf(var_field_t* field, void* var);

/**
  *@brief parses field value from raw data
  *@param field ptr to field being set
  *@pre data ptr to data buffer
  *@return len of parsed data
  */
int var_field_parse(var_field_t* field, uint8_t * data);
