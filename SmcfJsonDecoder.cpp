/*
 * SmcfJsonDecoder.cpp
 *
 *  Created on: 07/Feb/2016
 *      Author: SERGIO M C FIGUEIREDO
 */
#include "SmcfJsonDecoder.h"
#include <cstring>
#include <cstdlib>
#include <cctype>

using namespace std;

inline void SmcfJsonDecoder::skipSpace(char*& str) {
	while (isspace(*str)){str++;};
}

uint8_t SmcfJsonDecoder::readString(char *&expr, char *dest, int maxDestSize){
#ifdef JSON_DEBUG
	Serial.print("readString(");
    Serial.print(expr);
    Serial.println(");");
#endif
	skipSpace(expr);
	if (*expr!='"') {
		return JSON_ERR_DOUBLE_QUOTES_EXPECTED; // fail
	}
	expr++; // pula a primeira aspas
	//deve começar e terminar com aspas
	while ((maxDestSize>0) && (*expr) && (*expr!='"')) {
		if (*expr==0x00) {
			return JSON_ERR_INVALID_STRING;
		}
		if (*expr=='\\') {
			//escape char
			expr++;
			switch (*expr) {
			  case '\\':
			  case '/':
			  case '"':
				  *dest=*expr;
				  break;
			  case 'b': *dest='\b';break;
			  case 'f': *dest='\f';break;
			  case 'n': *dest='\n';break;
			  case 'r': *dest='\r';break;
			  case 't': *dest='\t';break;
			  default:
				  return JSON_ERR_INVALID_ESCAPE_CHAR;
			}
		} else {
			*dest=*expr;
		}
		maxDestSize--;
		dest++;
		expr++;
	}
	*dest=0x00; // end-of-string
	if (*expr!='"') {
		return JSON_ERR_DOUBLE_QUOTES_EXPECTED; // fail
	}
	expr++; // vai para o prox. char apos as aspas final
	return JSON_ERR_NO_ERROR;
}

uint8_t SmcfJsonDecoder::readObjMember(char*& jsonExpr, int (*callback)(int,void *)) {
#ifdef JSON_DEBUG
	Serial.print("readObjMember(");
    Serial.print(jsonExpr);
    Serial.println("); (in)");
#endif
	char key[JSON_KEY_MAX_SIZE+1];
	int err=readString(jsonExpr,key,JSON_KEY_MAX_SIZE);
	if (err) {
	   return err;
	}
	callback(JSON_ELEMENT_OBJECT_KEY, key);
    skipSpace(jsonExpr);
    if (*jsonExpr==':') {
    	err = readValue(++jsonExpr, callback);
    	if (err) {
    	   return err;
    	}
    } else {
 	   return JSON_ERR_COLON_EXPECTED;
    }
#ifdef JSON_DEBUG
    Serial.print("readObjMember(");
    Serial.print(jsonExpr);
    Serial.println("); (out)");
#endif
    return JSON_ERR_NO_ERROR;
}

uint8_t SmcfJsonDecoder::readElements(char*& jsonExpr, int (*callback)(int,void *), const char delimiters[2],
		uint8_t(SmcfJsonDecoder::*readElement)(char*&,int(*)(int,void *))) {
#ifdef JSON_DEBUG
    Serial.print("readElements(");
    Serial.print(jsonExpr);
    Serial.println(");");
#endif
    int err;
	skipSpace(jsonExpr);
	if (*jsonExpr!=delimiters[0]) {
		return JSON_ERR_LEFT_DELIMITER_EXPECTED;
	}
    skipSpace(++jsonExpr);// pula o delimitador inicial e espaços subsequentes
	while (*jsonExpr && *jsonExpr!=delimiters[1]) {
		err=((this)->*(readElement))(jsonExpr, callback);
		if (err) {
		   return err;
		}
		skipSpace(jsonExpr);
	    if (*jsonExpr==',') {
	    	// encontrou virgula? le proximo valor
	    	jsonExpr++;
	    	continue;
	    }
	    // nao tem virgula? entao confere se eh o delimitador final
	    if (*jsonExpr!=delimiters[1]) {
    		return JSON_ERR_RIGHT_DELIMITER_EXPECTED;
    	}
	}
    jsonExpr++; // pula delimitador final
    return JSON_ERR_NO_ERROR;
}

uint8_t SmcfJsonDecoder::readNumber(char*& jsonExpr, int (*callback)(int,void *)) {
#ifdef JSON_DEBUG
    Serial.print("readNumber(");
    Serial.print(jsonExpr);
    Serial.println(");");
#endif
    char number[JSON_VALUE_MAX_SIZE+1];
    int len=0;
    bool isDouble = false;
    while (*jsonExpr && (isdigit(*jsonExpr)||*jsonExpr=='.'||*jsonExpr=='-') && len<JSON_VALUE_MAX_SIZE) {
    	isDouble = (*jsonExpr=='.'); // tem ponto decimal? converte para double
    	number[len++] = *jsonExpr;
    	jsonExpr++; // proximo digito
    }
    if (isDouble) {
    	double doubleValue = atof(number);
		callback(JSON_ELEMENT_NUMBER_DOUBLE, &doubleValue);
    } else {
		long longValue = atol(number);
		callback(JSON_ELEMENT_NUMBER_LONG, &longValue);
    }
	return JSON_ERR_NO_ERROR;
}

uint8_t SmcfJsonDecoder::readObject(char*& jsonExpr, int (*callback)(int,void *)) {
	callback(JSON_ELEMENT_OBJECT_START, NULL);
	int err=readElements(jsonExpr, callback, "{}", &SmcfJsonDecoder::readObjMember);
	callback(JSON_ELEMENT_OBJECT_END, NULL);
	return err;
}

uint8_t SmcfJsonDecoder::readArray(char*& jsonExpr, int (*callback)(int,void *)) {
	callback(JSON_ELEMENT_ARRAY_START, NULL);
	int err=readElements(jsonExpr, callback, "[]", &SmcfJsonDecoder::readValue);
	callback(JSON_ELEMENT_ARRAY_END, NULL);
	return err;
}

uint8_t SmcfJsonDecoder::readValue(char*& jsonExpr, int (*callback)(int,void *)) {
#ifdef JSON_DEBUG
    Serial.print("readValue(");
    Serial.print(jsonExpr);
    Serial.println("); (in)");
#endif
    uint8_t err=JSON_ERR_INVALID_JSON_EXPRESSION;
	skipSpace(jsonExpr);
	switch (*jsonExpr) {
	case '{':
	   err=readObject(jsonExpr,callback);
	   break;
	case '[':
	   err=readArray(jsonExpr,callback);
	   break;
	case '"':
	   char str[JSON_VALUE_MAX_SIZE+1];
	   err=readString(jsonExpr, str, JSON_VALUE_MAX_SIZE);
	   if (!err) callback(JSON_ELEMENT_STRING, str);
	   break;
	case 't':
	case 'f':
		// todo ler boolean
		break;
	case 'n':
		// todo ler null
		break;
	case '0'...'9':
	case '-':
	   err=readNumber(jsonExpr,callback);
       break;
    }
#ifdef JSON_DEBUG
    Serial.print("readValue(");
    Serial.print(jsonExpr);
    Serial.println("); (out)");
#endif
    return err;
}

uint8_t SmcfJsonDecoder::decode(char *jsonExpr, int (*callback)(int,void *)) {
#ifdef JSON_DEBUG
    Serial.print("decode(");
    Serial.print(jsonExpr);
    Serial.println(");");
#endif
    int err=readValue(jsonExpr, callback);
	if (err) return err;
	// após a leitura do valor, a string deve ter acabado ou só conter espaços...
	skipSpace(jsonExpr);
	if (*jsonExpr) {
#ifdef JSON_DEBUG
		Serial.print("caracteres invalidos '");
		Serial.print(jsonExpr);
		Serial.println("'");
#endif
		err = JSON_ERR_INVALID_CHAR_AFTER_JSON;
	}
	return err;
}
