/*

 I use this example for quality test of code. Here I have implemented many tests to verify errors here.
 
 */
#include <SmcfJsonDecoder.h>

SmcfJsonDecoder jsonDecoder;

#define DEBUGGING 
/**
 * callback for jsonparse when changename.
 * Se retornar qualquer coisa diferente de 0, para o processamento e retorna na hora.
 *
 */
int jsonCallback(int jsonElementType, void* value) {
#ifdef DEBUGGING  
  Serial.print("jsonCallback(");
  Serial.print(jsonElementType);
  Serial.println(");");
#endif  
  switch (jsonElementType) {
    case JSON_ELEMENT_OBJECT_START:
#ifdef DEBUGGING  
    	Serial.println("Object Start");
#endif
	break;
    case JSON_ELEMENT_OBJECT_END:
#ifdef DEBUGGING  
    	Serial.println("Object End");
#endif
	break;
    case JSON_ELEMENT_OBJECT_KEY: {
#ifdef DEBUGGING  
    	Serial.print("Key: '");
        Serial.print((char*)value);
        Serial.println("'");
#endif
	break;
    }
    case JSON_ELEMENT_STRING: {
#ifdef DEBUGGING
	Serial.print("String: '");
        Serial.print((char*)value);
        Serial.println("'");
#endif
    	break;
    }
    case JSON_ELEMENT_ARRAY_START:
#ifdef DEBUGGING  
    	Serial.println("Array Start");
#endif
	break;
    case JSON_ELEMENT_ARRAY_END:
#ifdef DEBUGGING  
    	Serial.println("Array End");
#endif
	break;
    case JSON_ELEMENT_NUMBER_LONG: {
#ifdef DEBUGGING  
    	Serial.println("Number (Long) element");
#endif
	long number = *((long*)value);
#ifdef DEBUGGING  
    	Serial.print("number: ");
        Serial.println(number);
#endif
	break;
    }
    case JSON_ELEMENT_NUMBER_DOUBLE: {
#ifdef DEBUGGING  
    	Serial.println("Number (Double) element");
#endif
	double number = *((double*)value);
#ifdef DEBUGGING  
    	Serial.print("number: ");
        Serial.println(number);
#endif
	break;
    }
    case JSON_ELEMENT_BOOLEAN:
#ifdef DEBUGGING  
    	Serial.println("Boolean element");
#endif
	break;
  }

  return JSON_ERR_NO_ERROR;
}

void teste1() {
  Serial.println("=====TESTE 1");
    char command[] = "{\"0\":\"TESTE\",\"1\":\"VRAND\"}";
    int ret = jsonDecoder.decode(command, jsonCallback);
    if (ret!=0) {
    	Serial.print("TESTE 1 : JSON PARSER ERROR: ");
        Serial.println(ret);
    } else {
    	Serial.println("TESTE 1: OK!");
    }
}

void teste2() {
  Serial.println("=====TESTE 2");
    char command[] = "  [  \"0\",123 ,\"TESTE\" , \"1\\\"2\",  \"VR  AND\" ] ";
    int ret = jsonDecoder.decode(command, jsonCallback);
    if (ret!=0) {
    	Serial.print("TESTE 2 : JSON PARSER ERROR: ");
        Serial.println(ret);
    } else {
    	Serial.println("TESTE 2: OK");
    }
}

void teste3NumberLong() {
  Serial.println("=====TESTE 3 - Number Long");
    char command[] = "  -12345 ";
    int ret = jsonDecoder.decode(command, jsonCallback);
    if (ret!=0) {
    	Serial.print("TESTE 3 : JSON PARSER ERROR: ");
        Serial.println(ret);
    } else {
    	Serial.println("TESTE 3: OK");
    }
}

void teste4ArrayMultTypes() {
  Serial.println("=====TESTE 4");
  char command[] = " [\"string somente\",{},{\"chave 1\":\"teste \\\\legal\\\\ \",\"chave 2\":4567},-9876,{\"key\":true,\"other key\":false}] ";
  int ret = jsonDecoder.decode(command, jsonCallback);
  if (ret!=0) {
    Serial.println("TESTE 4 : JSON PARSER ERROR: ");
    Serial.println(ret);
  } else {
    Serial.println("TESTE 4: OK");
  }
}

void teste5Boolean() {
  Serial.println("=====TESTE 5 - Boolean");
  char command[] = "[true,false]";
  int ret = jsonDecoder.decode(command, jsonCallback);
  if (ret!=0) {
    Serial.println("TESTE 5 : JSON PARSER ERROR: ");
    Serial.println(ret);
  } else {
    Serial.println("TESTE 5: OK");
  }
}

void teste6NumberDouble() {
  Serial.println("=====TESTE 6 - Number Double");
    char command[] = "  -12345.22 ";
    int ret = jsonDecoder.decode(command, jsonCallback);
    if (ret!=0) {
    	Serial.print("TESTE 6 : JSON PARSER ERROR: ");
        Serial.println(ret);
    } else {
    	Serial.println("TESTE 6: OK");
    }
}

void setup() {
  Serial.begin(9600);

  teste1();
  teste2();
  teste3NumberLong();
  teste4ArrayMultTypes();
  teste5Boolean();
  teste6NumberDouble();
  Serial.println("FIM");
}

void loop() {

}