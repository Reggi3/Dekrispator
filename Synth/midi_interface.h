/**
 ******************************************************************************
 * File Name          	: midi_interface.h
 * Author				: Xavier Halgand
 * Date               	:
 * Description        	:
 ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MIDI_INTERFACE_H
#define __MIDI_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "CONSTANTS.h"
#include "random.h"
#include "drifter.h"
#include "sinetable.h"
#include "notesTables.h"
#include "drifter.h"
#include "soundGen.h"
#include "audio.h"
#include <stm32f4xx.h>
/* Exported functions ------------------------------------------------------- */
void MIDI_Decode(uint8_t * outBuf);
void MagicFX(uint8_t val);
void MagicChorus(uint8_t val);
void MagicPhaser(uint8_t val);
void MagicDelay(uint8_t val);
void MagicPatch(uint8_t val);
#define 	MIDI_EVENT(virtualcable, command)   (((virtualcable) << 4) | ((command) >> 4))
/****************************************************************************************************************************/
#endif /* __MIDI_INTERFACE_H */
