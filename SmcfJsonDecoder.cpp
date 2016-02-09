/*
 * SmcfJsonDecoder.cpp
 *
 *  Created on: 07/Feb/2016
 *      Author: SERGIO M C FIGUEIREDO
 */
#include "SmcfJsonDecoder.h"

using namespace std;

inline void SmcfJsonDecoder::skipSpace(char*& str) {
	while (isspace(*str)){str++;};
}

uint8_t SmcfJsonDecoder::readString(char *&expr, char *dest, int maxDestSize){
#ifdef JSON_DEBUG
	Serial.print(F("readString("));
    Serial.print(expr);
    Serial.println(F(");"));
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

uint8_t SmcfJsonDecoder::readObjMember() {
#ifdef JSON_DEBUG
	Serial.print(F("readObjMember("));
    Serial.print(this->jsonPtr);
    Serial.println(F("); (in)"));
#endif
	char key[JSON_KEY_MAX_SIZE+1];
	int err=readString(this->jsonPtr,key,JSON_KEY_MAX_SIZE);
	if (err) {
	   return err;
	}
	callback(JSON_ELEMENT_OBJECT_KEY, key, this->context);
    skipSpace(this->jsonPtr);
    if (*this->jsonPtr==':') {
    	this->jsonPtr++; // skip colon
    	err = readValue();
    	if (err) {
    	   return err;
    	}
    } else {
 	   return JSON_ERR_COLON_EXPECTED;
    }
#ifdef JSON_DEBUG
    Serial.print(F("readObjMember("));
    Serial.print(this->jsonPtr);
    Serial.println(F("); (out)"));
#endif
    return JSON_ERR_NO_ERROR;
}

uint8_t SmcfJsonDecoder::readElements(const char delimiters[2],
		uint8_t(SmcfJsonDecoder::*readElement)(void)) {
#ifdef JSON_DEBUG
    Serial.print(F("readElements("));
    Serial.print(this->jsonPtr);
    Serial.println(F(");"));
#endif
    int err;
	skipSpace(this->jsonPtr);
	if (*this->jsonPtr!=delimiters[0]) {
		return JSON_ERR_LEFT_DELIMITER_EXPECTED;
	}
    skipSpace(++this->jsonPtr);// pula o delimitador inicial e espaços subsequentes
	while (*this->jsonPtr && *this->jsonPtr!=delimiters[1]) {
		err=((this)->*(readElement))();
		if (err) {
		   return err;
		}
		skipSpace(this->jsonPtr);
	    if (*this->jsonPtr==',') {
	    	// encontrou virgula? le proximo valor
	    	this->jsonPtr++;
	    	continue;
	    }
	    // nao tem virgula? entao confere se eh o delimitador final
	    if (*this->jsonPtr!=delimiters[1]) {
    		return JSON_ERR_RIGHT_DELIMITER_EXPECTED;
    	}
	}
    this->jsonPtr++; // pula delimitador final
    return JSON_ERR_NO_ERROR;
}

uint8_t SmcfJsonDecoder::readNumber() {
#ifdef JSON_DEBUG
    Serial.print(F("readNumber("));
    Serial.print(this->jsonPtr);
    Serial.println(F(");"));
#endif
    char number[JSON_VALUE_MAX_SIZE+1];
    int len=0;
    bool isDouble = false;
    while (*this->jsonPtr && (isdigit(*this->jsonPtr)||*this->jsonPtr=='.'||*this->jsonPtr=='-') && len<JSON_VALUE_MAX_SIZE) {
    	if (!isDouble) isDouble = (*this->jsonPtr=='.'); // tem ponto decimal? converte para double
    	number[len++] = *this->jsonPtr;
    	this->jsonPtr++; // proximo digito
    }
    if (isDouble) {
    	double doubleValue = atof(number);
		return callback(JSON_ELEMENT_NUMBER_DOUBLE, &doubleValue, this->context);
    } else {
		long longValue = atol(number);
		return callback(JSON_ELEMENT_NUMBER_LONG, &longValue, this->context);
    }
}

uint8_t SmcfJsonDecoder::readBoolean() {
#ifdef JSON_DEBUG
    Serial.print(F("readBoolean("));
    Serial.print(this->jsonPtr);
    Serial.println(F(");"));
#endif
	boolean value;
	if (memcmp_P(this->jsonPtr,F("true"),4)==0) {
		value = true;
		this->jsonPtr+=4;
	} else if (memcmp_P(this->jsonPtr,F("false"),5)==0) {
		value = false;
		this->jsonPtr+=5;
	} else {
		return JSON_ERR_INVALID_BOOLEAN_VALUE;
	}
	return callback(JSON_ELEMENT_BOOLEAN, &value, this->context);
}

uint8_t SmcfJsonDecoder::readObject() {
	callback(JSON_ELEMENT_OBJECT_START, NULL, this->context);
	int err=readElements("{}", &SmcfJsonDecoder::readObjMember);
	callback(JSON_ELEMENT_OBJECT_END, NULL, this->context);
	return err;
}

uint8_t SmcfJsonDecoder::readArray() {
	callback(JSON_ELEMENT_ARRAY_START, NULL, this->context);
	int err=readElements("[]", &SmcfJsonDecoder::readValue);
	callback(JSON_ELEMENT_ARRAY_END, NULL, this->context);
	return err;
}

uint8_t SmcfJsonDecoder::readValue() {
#ifdef JSON_DEBUG
    Serial.print(F("readValue("));
    Serial.print(this->jsonPtr);
    Serial.println(F("); (in)"));
#endif
    uint8_t err=JSON_ERR_INVALID_JSON_EXPRESSION;
	skipSpace(this->jsonPtr);
	switch (*this->jsonPtr) {
	case '{':
	   err=readObject();
	   break;
	case '[':
	   err=readArray();
	   break;
	case '"':
	   char str[JSON_VALUE_MAX_SIZE+1];
	   err=readString(this->jsonPtr, str, JSON_VALUE_MAX_SIZE);
	   if (!err) callback(JSON_ELEMENT_STRING, str, this->context);
	   break;
	case 't':
	case 'f':
		err=readBoolean();
		break;
	case 'n':
		// todo ler null
		break;
	case '0'...'9':
	case '-':
	   err=readNumber();
       break;
    }
#ifdef JSON_DEBUG
    Serial.print(F("readValue("));
    Serial.print(this->jsonPtr);
    Serial.println(F("); (out)"));
#endif
    return err;
}

uint8_t SmcfJsonDecoder::decode(char jsonExpr[],  jsonCallback_t callback, void *context) {
	this->jsonPtr=jsonExpr;
	this->callback=callback;
	this->context=context;

#ifdef JSON_DEBUG
    Serial.print(F("decode("));
    Serial.print(this->jsonPtr);
    Serial.println(F(");"));
#endif
    int err=readValue();
	if (err) return err;
	// após a leitura do valor, a string deve ter acabado ou só conter espaços...
	skipSpace(this->jsonPtr);
	if (*this->jsonPtr) {
#ifdef JSON_DEBUG
		Serial.print(F("caracteres invalidos '"));
		Serial.print(this->jsonPtr);
		Serial.println(F("'"));
#endif
		err = JSON_ERR_INVALID_CHAR_AFTER_JSON;
	}
	return err;
}
