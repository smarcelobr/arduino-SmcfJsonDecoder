#include <SmcfJsonDecoder.h>

SmcfJsonDecoder jsonDecoder;

//#define DEBUG 
/**
 * callback for jsonparse when changename.
 * Se retornar qualquer coisa diferente de 0, para o processamento e retorna na hora.
 *
 */
int jsonCallback(int jsonElementType, void* value) {
#ifdef DEBUG  
  Serial.print("jsonCallback(");
  Serial.print(jsonElementType);
  Serial.println(");");
#endif  
  switch (jsonElementType) {
    case JSON_ELEMENT_OBJECT_START:
#ifdef DEBUG  
    	Serial.println("Object Start");
#endif
	break;
    case JSON_ELEMENT_OBJECT_END:
#ifdef DEBUG  
    	Serial.println("Object End");
#endif
	break;
    case JSON_ELEMENT_OBJECT_KEY: {
#ifdef DEBUG  
    	Serial.print("Key: '");
        Serial.print((char*)value);
        Serial.println("'");
#endif
	break;
    }
    case JSON_ELEMENT_STRING: {
#ifdef DEBUG
	Serial.print("String: '");
        Serial.print((char*)value);
        Serial.println("'");
#endif
    	break;
    }
    case JSON_ELEMENT_ARRAY_START:
#ifdef DEBUG  
    	Serial.println("Array Start");
#endif
	break;
    case JSON_ELEMENT_ARRAY_END:
#ifdef DEBUG  
    	Serial.println("Array End");
#endif
	break;
    case JSON_ELEMENT_NUMBER_LONG: {
#ifdef DEBUG  
    	Serial.println("Number element");
#endif
	long number = *((long*)value);
#ifdef DEBUG  
    	Serial.print("number: ");
        Serial.println(number);
#endif
	break;
    }
    case JSON_ELEMENT_BOOLEAN:
#ifdef DEBUG  
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

void teste3() {
  Serial.println("=====TESTE 3");
    char command[] = "  -12345 ";
    int ret = jsonDecoder.decode(command, jsonCallback);
    if (ret!=0) {
    	Serial.print("TESTE 3 : JSON PARSER ERROR: ");
        Serial.println(ret);
    } else {
    	Serial.println("TESTE 3: OK");
    }
}

void teste4() {
  Serial.println("=====TESTE 4");
  char command[] = " [\"string somente\",{},{\"chave 1\":\"teste \\\\legal\\\\ \",\"chave 2\":4567},-9876] ";
  int ret = jsonDecoder.decode(command, jsonCallback);
  if (ret!=0) {
    Serial.println("TESTE 4 : JSON PARSER ERROR: ");
    Serial.println(ret);
  } else {
    Serial.println("TESTE 4: OK");
  }
}

void setup() {
  Serial.begin(9600);

  teste1();
  teste2();
  teste3();
  teste4();
  Serial.println("FIM");
}

void loop() {

}