/*
 * SmcfJsonDecoder.h
 *
 *  Created on: 07/Feb/2016
 *      Author: Sergio Marcelo Couto Figueiredo
 */

#ifndef SMCFJSONDECODER_H_
#define SMCFJSONDECODER_H_

#include "Arduino.h"

// uncomment the below line for debug messages from library:
//#define JSON_DEBUG

// =====JSON PARSER FOR LOW RAM DEVICES
#define JSON_KEY_MAX_SIZE 10
#define JSON_VALUE_MAX_SIZE 30

// JSON ELEMENTS
#define JSON_ELEMENT_OBJECT_START 1
#define JSON_ELEMENT_OBJECT_END 2
#define JSON_ELEMENT_OBJECT_KEY 3
#define JSON_ELEMENT_ARRAY_START 4
#define JSON_ELEMENT_ARRAY_END 5
#define JSON_ELEMENT_STRING 7
#define JSON_ELEMENT_NUMBER_LONG 8
#define JSON_ELEMENT_NUMBER_DOUBLE 9
#define JSON_ELEMENT_BOOLEAN 10

// JSON ERROR CODES
#define JSON_ERR_NO_ERROR 0
#define JSON_ERR_DOUBLE_QUOTES_EXPECTED 1
#define JSON_ERR_INVALID_STRING 2
#define JSON_ERR_INVALID_ESCAPE_CHAR 3
#define JSON_ERR_COLON_EXPECTED 4
#define JSON_ERR_LEFT_DELIMITER_EXPECTED 5
#define JSON_ERR_RIGHT_DELIMITER_EXPECTED 6
#define JSON_ERR_INVALID_JSON_EXPRESSION 7
#define JSON_ERR_COMMA_EXPECTED 8
#define JSON_ERR_INVALID_CHAR_AFTER_JSON 9
#define JSON_ERR_USER_ERR_BASE 100

class SmcfJsonDecoder {
private:
	static void skipSpace(char *&str);
	uint8_t readElements(char*& jsonExpr, int (*callback)(int,void*), const char delimiters[2],
			uint8_t(SmcfJsonDecoder::*readElement)(char*&,int(*)(int,void*)));

	uint8_t readString(char *&expr, char*dest, int maxDestSize);
	uint8_t readObjMember(char*& jsonExpr, int (* callback)(int,void *));
	uint8_t readObject(char*& jsonExpr, int (* callback)(int,void *));
	uint8_t readArray(char*& jsonExpr, int (* callback)(int,void *));
	uint8_t readNumber(char*& jsonExpr, int (* callback)(int,void *));
	uint8_t readValue(char*& jsonExpr, int (* callback)(int,void *));
public:

	uint8_t decode(char *jsonExpr, int (* callback)(int,void *));
};
//=============================


#endif /* SMCFJSONDECODER_H_ */
