/*
 * protocol.c
 *
 *  Created on: Feb 20, 2023
 *      Author: melah
 */

#include "protocol.h"
float temperaturValue;

/********************************************************
 * Funktion um die temperatur zu lesen
 *******************************************************/
float readTemp(){

	return temperaturValue;
}

/*******************************************************
 * Funktion um temperatur zu schreiben
 *******************************************************/
void setTemp(float temp){
   temperaturValue = temp;
}

void BufferCopy(void* dst, void* src, int len){
	char* ptDst = (char*)dst;
	char* ptSrc = (char*)src;

	while(len--){
		*ptDst++ = *ptSrc++;

	}
}

uint32_t BufferCopyUntil(void* dst, void* src, char endCahr){
	char* ptDst = (char*)dst;
	char* ptSrc = (char*)src;
	uint32_t len = 0;

	while(*ptSrc != endCahr){
		*ptDst++ = *ptSrc++;
		len++;
	}

	return len;
}

/**********************************************************************
 * Diese Funktion gibt die Anzahl von digitale Stellen
 * @param: decimal, Zahl den die Anzahl von digit gezählt werden soll
 * @return uint8_t, Anzahl von digit von der gegebene Zahl
 *********************************************************************/
uint8_t getDigitCount(uint32_t decimal){

 uint8_t count = 0;

 do {
	 decimal /= 10;
	 ++count;
  } while (decimal != 0);

 return count;
}

/********************************************************************
 * Diese Routine wandelt ein float zu char array.
 *
 * @param: vor, das ist der vorkommazahl (Zahl vor dem Komma)
 * @param: nach, das ist der nachkommazahl (Zahl nach dem Komma)
 * @param: len, Länge der gesamte array plus den Punkt (z.B "120.34")
 * @param: floatStr, pointer zu dem Ergebnis
 *********************************************************************/
void toArray(uint32_t vor, uint32_t nach, uint32_t len, char* floatStr)
{
	if(floatStr == (char *)0) return;

	if(len == 0) {
		*floatStr = 0;
		return;
	}
    int v = getDigitCount(vor);
    int n = getDigitCount(nach);
    int i;
    char numberArray[len];
    for (i = v-1; i >= 0; --i, vor /= 10)
    {
        numberArray[i] = (vor % 10) + '0';
    }
    numberArray[v] = '.';


    for (i = n-1; i >= 0; --i, nach /= 10)
    {
        numberArray[v+1+i] = (nach % 10) + '0';
    }

    BufferCopy(floatStr, numberArray, len);
}

/**********************************************************************
 * Diese Funktion extrahiert der vorkommazahl von einem float Zahl
 * @param: value, float wert den der vorkommazahl extrahiert wird
 **********************************************************************/
uint32_t getHexBeforeComma(float value){

	uint32_t vorkomma = (uint32_t)value;
	return vorkomma;
}

/**********************************************************************
 * Diese Funktion extrahiert der nachkommazahl von einem float Zahl
 * @param: value, float wert den der nachkommazahl extrahiert wird
 **********************************************************************/
uint32_t getHexAfterComma(float value){

	float temp;
	uint32_t vorkomma = (uint32_t)value;
	temp = value - vorkomma;
	uint32_t nachkomma = 0;
	uint8_t count = 0;


	while((temp - (uint32_t)temp) != 0.0){
		temp = temp * 10;
		count++;
		if(count >= 2) break;
	}
	nachkomma = (uint32_t)temp;

	return nachkomma;
}

/********************************************************************
 * Diese Routine wandelt ein float zu char array.
 *
 * @param: decbefore, das ist der vorkommazahl (Zahl vor dem Komma)
 * @param: beforeCount,das ist die Länge der vorkommazahl (Länge der Zahl vor dem Komma)
 * @param: decafter, das ist der nachkommazahl (Zahl nach dem Komma)
 * @param: afterCount, das ist die Länge der nachkommazahl (Länge der Zahl nach dem Komma)
 * @param: ptResult, pointer zu dem Ergebnis
 *********************************************************************/
int decToCharArry(uint32_t decbefore, uint8_t beforeCount, uint32_t decAfter, uint8_t afterCount, char* ptResult){
	if(ptResult == (char *)0) return 0;
	int i;
	char ch;
	char before[beforeCount + 1]; // +1 für den . Punkt
	char after[afterCount];

	if(beforeCount > 0){
		for(i=beforeCount - 1; i>=0; i--, decbefore /= 10){

			before[i] = (decbefore % 10) + '0';
		}
		//Diese Zeile trennt der Vorkommazahl mit dem Nachkommazahl
		before[beforeCount] = '.';
	}

	if(afterCount > 0){
		for(i=afterCount - 1; i>=0; i--, decAfter /= 10){

			after[i] = (decAfter % 10) + '0';
		}
	}

	BufferCopy(ptResult, before, beforeCount+1);
	BufferCopy(&ptResult[beforeCount + 1], after, afterCount);

	return 1;
}
