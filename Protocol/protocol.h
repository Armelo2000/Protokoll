/*
 * protocol.h
 *
 *  Created on: Feb 20, 2023
 *      Author: melah
 */

#ifndef PROTOCOL_PROTOCOL_H_
#define PROTOCOL_PROTOCOL_H_

#include <stdint.h>

#define PROTOCOL_READ_TEMP		0x72
#define PROTOCOL_WRITE_TEMP		0x77

#define LEERTASTE				0x20
#define ENTER					0x0D

/********************************************************
 * Funktion um die temperatur zu lesen
 *******************************************************/
float readTemp();

/*******************************************************
 * Funktion um temperatur zu schreiben
 *******************************************************/
void setTemp(float temp);

//char* floatToString(float value);
//int floatToCharArry(uint32_t vorkomma, uint32_t nachkomma, char* ptfloatStr);
/**********************************************************************
 * Funktion um daten von einem ort zum anderen Ort zu kopieren
 **********************************************************************/
void BufferCopy(void* dst, void* src, int len);
uint32_t BufferCopyUntil(void* dst, void* src, char endCahr);

/**********************************************************************
 * Diese Funktion gibt die Anzahl von digitale Stellen
 * @param: decimal, Zahl den die Anzahl von digit gezählt werden soll
 * @return uint8_t, Anzahl von digit von der gegebene Zahl
 *********************************************************************/
uint8_t getDigitCount(uint32_t decimal);

/********************************************************************
 * Diese Routine wandelt ein float zu char array.
 *
 * @param: vor, das ist der vorkommazahl (Zahl vor dem Komma)
 * @param: nach, das ist der nachkommazahl (Zahl nach dem Komma)
 * @param: len, Länge der gesamte array plus den Punkt (z.B "120.34")
 * @param: floatStr, pointer zu dem Ergebnis
 *********************************************************************/
void toArray(uint32_t vor, uint32_t nach, uint32_t len, char* floatStr);

/********************************************************************
 * Diese Routine wandelt ein float zu char array.
 *
 * @param: decbefore, das ist der vorkommazahl (Zahl vor dem Komma)
 * @param: beforeCount,das ist die Länge der vorkommazahl (Länge der Zahl vor dem Komma)
 * @param: decafter, das ist der nachkommazahl (Zahl nach dem Komma)
 * @param: afterCount, das ist die Länge der nachkommazahl (Länge der Zahl nach dem Komma)
 * @param: ptResult, pointer zu dem Ergebnis
 *********************************************************************/
int decToCharArry(uint32_t decbefore, uint8_t beforeCount, uint32_t decAfter, uint8_t afterCount, char* ptResult);

/**********************************************************************
 * Diese Funktion extrahiert der vorkommazahl von einem float Zahl
 * @param: value, float wert den der vorkommazahl extrahiert wird
 **********************************************************************/
uint32_t getHexBeforeComma(float value);

/**********************************************************************
 * Diese Funktion extrahiert der nachkommazahl von einem float Zahl
 * @param: value, float wert den der nachkommazahl extrahiert wird
 **********************************************************************/
uint32_t getHexAfterComma(float value);

#endif /* PROTOCOL_PROTOCOL_H_ */
