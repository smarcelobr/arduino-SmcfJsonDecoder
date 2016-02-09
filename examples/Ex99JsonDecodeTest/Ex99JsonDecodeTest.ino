/*

 I use this example for quality test of code. Here I have implemented many tests to verify errors here.
 
 */
#include <SmcfJsonDecoder.h>

SmcfJsonDecoder jsonDecoder;

#define DEBUGGING 

#define LAST_JSON_ELEMENT_NUMBER JSON_ELEMENT_BOOLEAN
class JsonElementCounter {
private:  
  int counters[LAST_JSON_ELEMENT_NUMBER+1];
  
public:  
  JsonElementCounter() { this->reset(); }
  void reset() {for (int i=0;i<LAST_JSON_ELEMENT_NUMBER+1;i++) counters[i]=0;}
  void count(int jsonElementType) { counters[jsonElementType]++; }
  void assert(int jsonElementType, int expectedCount, __FlashStringHelper* message) { if (counters[jsonElementType]!=expectedCount) Serial.println(message); }
};

/**
 * callback for jsonparse when changename.
 * Se retornar qualquer coisa diferente de 0, para o processamento e retorna na hora.
 *
 */
int jsonCallback(int jsonElementType, void* value, void* context) {

  JsonElementCounter &jsonElementCounter = *(JsonElementCounter*)context;
  jsonElementCounter.count(jsonElementType);

#ifdef DEBUGGING  
  Serial.print("jsonCallback(");
  Serial.print(jsonElementType);
  Serial.println(");");
  switch (jsonElementType) {
    case JSON_ELEMENT_OBJECT_START:
    	Serial.println("Object Start");
	break;
    case JSON_ELEMENT_OBJECT_END:
    	Serial.println("Object End");
	break;
    case JSON_ELEMENT_OBJECT_KEY: {
    	Serial.print("Key: '");
        Serial.print((char*)value);
        Serial.println("'");
	break;
    }
    case JSON_ELEMENT_STRING: {
	Serial.print("String: '");
        Serial.print((char*)value);
        Serial.println("'");
    	break;
    }
    case JSON_ELEMENT_ARRAY_START:
    	Serial.println("Array Start");
	break;
    case JSON_ELEMENT_ARRAY_END:
    	Serial.println("Array End");
	break;
    case JSON_ELEMENT_NUMBER_LONG: {
    	Serial.println("Number (Long) element");
	long number = *((long*)value);
    	Serial.print("number: ");
        Serial.println(number);
	break;
    }
    case JSON_ELEMENT_NUMBER_DOUBLE: {
    	Serial.println("Number (Double) element");
	double number = *((double*)value);
    	Serial.print("number: ");
        Serial.println(number);
	break;
    }
    case JSON_ELEMENT_BOOLEAN:
    	Serial.println("Boolean element");
	break;
  }
#endif

  return JSON_ERR_NO_ERROR;
}

void teste1() {
  JsonElementCounter jsonElementCounter;
  
  Serial.println(F("=====TESTE 1"));
  char command[] = "{\"0\":\"TESTE\",\"1\":\"VRAND\"}";
  int ret = jsonDecoder.decode(command, jsonCallback, &jsonElementCounter);
  if (ret!=0) {
      Serial.print(F("TESTE 1 : JSON PARSER ERROR: "));
      Serial.println(ret);
  } else {
      jsonElementCounter.assert(JSON_ELEMENT_OBJECT_START,1,F("Inicio de objeto nao detectado."));
      jsonElementCounter.assert(JSON_ELEMENT_OBJECT_END,1,F("Fim de objeto nao detectado."));
      jsonElementCounter.assert(JSON_ELEMENT_OBJECT_KEY,2,F("Chaves de objetos na quantidade errada."));
      jsonElementCounter.assert(JSON_ELEMENT_STRING,2,F("Strings na quantidade errada."));
      jsonElementCounter.assert(JSON_ELEMENT_ARRAY_START,0,F("Inicio de Array na quantidade errada."));
      jsonElementCounter.assert(JSON_ELEMENT_ARRAY_END,0,F("Fim de Array na quantidade errada."));
      jsonElementCounter.assert(JSON_ELEMENT_NUMBER_LONG,0,F("Number (long) na quantidade errada."));
      jsonElementCounter.assert(JSON_ELEMENT_NUMBER_DOUBLE,0,F("Number (double) na quantidade errada."));
      jsonElementCounter.assert(JSON_ELEMENT_BOOLEAN,0,F("Boolean na quantidade errada."));
  }
  Serial.println(F("TESTE 1: END"));
}

void teste2() {
  JsonElementCounter jsonElementCounter;
  Serial.println(F("=====TESTE 2"));
  char command[] = "  [  \"0\",123 ,\"TESTE\" , \"1\\\"2\",  \"VR  AND\" ] ";
  int ret = jsonDecoder.decode(command, jsonCallback,&jsonElementCounter);
  if (ret!=0) {
    Serial.print(F("TESTE 2 : JSON PARSER ERROR: "));
    Serial.println(ret);
  } else {
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_START,0,F("Inicio de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_END,0,F("Fim de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_KEY,0,F("Chaves de objetos na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_STRING,4,F("Strings na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_START,1,F("Inicio de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_END,1,F("Fim de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_LONG,1,F("Number (long) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_DOUBLE,0,F("Number (double) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_BOOLEAN,0,F("Boolean na quantidade errada."));
  }
  Serial.println(F("TESTE 2: END"));
}

void teste3NumberLong() {
  JsonElementCounter jsonElementCounter;
  
  Serial.println(F("=====TESTE 3 - Number Long"));
  char command[] = "  -12345 ";
  int ret = jsonDecoder.decode(command, jsonCallback,&jsonElementCounter);
  if (ret!=0) {
    Serial.print(F("TESTE 3 : JSON PARSER ERROR: "));
    Serial.println(ret);
  } else {
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_START,0,F("Inicio de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_END,0,F("Fim de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_KEY,0,F("Chaves de objetos na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_STRING,0,F("Strings na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_START,0,F("Inicio de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_END,0,F("Fim de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_LONG,1,F("Number (long) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_DOUBLE,0,F("Number (double) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_BOOLEAN,0,F("Boolean na quantidade errada."));
  }
  Serial.println(F("TESTE 3: END"));
}

void teste4ArrayMultTypes() {
  JsonElementCounter jsonElementCounter;

  Serial.println(F("=====TESTE 4"));
  char command[] = " [\"string somente\",{},{\"chave 1\":\"teste \\\\legal\\\\ \",\"chave 2\":4567},-9876,{\"key\":true,\"other key\":false}] ";
  int ret = jsonDecoder.decode(command, jsonCallback,&jsonElementCounter);
  if (ret!=0) {
    Serial.println(F("TESTE 4 : JSON PARSER ERROR: "));
    Serial.println(ret);
  } else {
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_START,3,F("Inicio de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_END,3,F("Fim de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_KEY,4,F("Chaves de objetos na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_STRING,2,F("Strings na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_START,1,F("Inicio de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_END,1,F("Fim de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_LONG,2,F("Number (long) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_DOUBLE,0,F("Number (double) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_BOOLEAN,2,F("Boolean na quantidade errada."));
  }
  Serial.println(F("TESTE 4: END"));
}

void teste5Boolean() {
  JsonElementCounter jsonElementCounter;
  Serial.println(F("=====TESTE 5 - Boolean"));
  char command[] = "[true,false]";
  int ret = jsonDecoder.decode(command, jsonCallback,&jsonElementCounter);
  if (ret!=0) {
    Serial.println(F("TESTE 5 : JSON PARSER ERROR: "));
    Serial.println(ret);
  } else {
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_START,0,F("Inicio de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_END,0,F("Fim de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_KEY,0,F("Chaves de objetos na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_STRING,0,F("Strings na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_START,1,F("Inicio de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_END,1,F("Fim de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_LONG,0,F("Number (long) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_DOUBLE,0,F("Number (double) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_BOOLEAN,2,F("Boolean na quantidade errada."));
  }
  Serial.println(F("TESTE 5: END"));
}

void teste6NumberDouble() {
  JsonElementCounter jsonElementCounter;
  Serial.println(F("=====TESTE 6 - Number Double"));
  char command[] = "  -12345.22 ";
  int ret = jsonDecoder.decode(command, jsonCallback,&jsonElementCounter);
  if (ret!=0) {
    Serial.print(F("TESTE 6 : JSON PARSER ERROR: "));
    Serial.println(ret);
  } else {
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_START,0,F("Inicio de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_END,0,F("Fim de objeto nao detectado."));
    jsonElementCounter.assert(JSON_ELEMENT_OBJECT_KEY,0,F("Chaves de objetos na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_STRING,0,F("Strings na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_START,0,F("Inicio de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_ARRAY_END,0,F("Fim de Array na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_LONG,0,F("Number (long) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_NUMBER_DOUBLE,1,F("Number (double) na quantidade errada."));
    jsonElementCounter.assert(JSON_ELEMENT_BOOLEAN,0,F("Boolean na quantidade errada."));
  }
  Serial.println(F("TESTE 6: END"));
}

void setup() {
  Serial.begin(9600);

  teste1();
  teste2();
  teste3NumberLong();
  teste4ArrayMultTypes();
  teste5Boolean();
  teste6NumberDouble();
  Serial.println(F("FIM"));
}

void loop() {

}