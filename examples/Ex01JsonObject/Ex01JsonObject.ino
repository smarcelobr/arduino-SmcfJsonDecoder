/*
 Example for decode a simple Json Object. 
 
 Is this example, which member of json object is a key-value where the 
 key is a digital pin number and the value is a boolean expression that indicates the level status 
 of this port.
 
 Example:
 { "13":true, "2":false } - indicates a request to set pin 13 as HIGH and the pin 2 as LOW.
 
1- Note that the object key are always of string type. (as seen at http://json.org/)
2- Boolean values in json are true or false (always lowercase and no quotes)

 Created by Sergio M C Figueiredo
 
*/
#include <SmcfJsonDecoder.h>

SmcfJsonDecoder jsonDecoder;

/* selectedPin variable stores a current pin that will be modified */
int selectedPin=0; /* 0 value indicates no selected port */

void setup() {
   Serial.begin(9600);
   
   char * json = "{\"13\":true,\"4\":true,\"2\":false}";

   /* jsonCallback function will handle the json data */
   int err = jsonDecoder.decode(json, jsonCallback);   
   
   if (!err) {
     Serial.println("no error");
   } else {
     Serial.print("json decode error code:");
     Serial.print(err);
   }
}

void loop() {
	
}

/**
 * This function are called by json decoder while the json elements are identified.
 */
int jsonCallback(int jsonElementType, void* value) {
  switch (jsonElementType) {
    case JSON_ELEMENT_OBJECT_KEY: {

        char *key = (char*)value; // alias to point of char.
        int pin = atoi(key); // convert ascii to byte number
        if (pin>=2 && pin<=13) { // valid pin?
          selectedPin = pin; // stores the selected pin for next call, where the boolean value is expected.
        }

        Serial.print("Selected pin: ");
        Serial.println(pin);

	break;
    }
    case JSON_ELEMENT_BOOLEAN: {
        boolean levelHigh= *((boolean *)value);
        if (selectedPin) {
          Serial.print("PIN ");
          Serial.print(selectedPin);
          if (levelHigh) {
            Serial.println(" <- HIGH");
            digitalWrite(selectedPin, HIGH);
          } else {
            Serial.println(" <- LOW");
            digitalWrite(selectedPin, LOW);
          }
        } 
        break;
    }
    default:
       Serial.println("No selected pin.");
       selectedPin=0; // deselect any pin
  }

  return JSON_ERR_NO_ERROR;
}
