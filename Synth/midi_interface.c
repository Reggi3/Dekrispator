/**
 ******************************************************************************
 * File Name          	: midi_interface.c
 * Author				: Xavier Halgand
 * Date               	:
 * Description        	:
 ******************************************************************************
 */

/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

#include "midi_interface.h"
/**
  * @brief   This function sends strings to the serial wire viewer.
  * @param  *s: pointer to string to be displayed on SWV
  * @retval None
  */
void SWV_puts(const char *s )
{
    while (*s) ITM_SendChar(*s++);

}

/**
  * @brief   This function sends numbers to the serial wire viewer.
  * @param  number: number to be displayed on SWV
  * @retval None
  */
void SWV_printnum(long number)
{
 unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars.
 unsigned int i = 0;

 //if number is 0
 if (number == 0)
 {
  ITM_SendChar('0'); //if number is zero
     return;
 }
 //account for negative numbers
 if (number < 0)
 {
  ITM_SendChar('-');
  number = number * -1;
 }
 while(number > 0)
 {
  buf[i++] = number % 10; //display in base 10
  number = number / 10;
  //NOTE: the effect of i++ means that the i variable will be at number of digits + 1
 }
 for(; i > 0; i--)
 {
  ITM_SendChar((char)('0' + buf[i-1]));
 }
}

/**
  * @brief  This function sends numbers to the serial wire viewer.
  * @param  number: number to be displayed on SWV
  * @param  digits: number of digits after decimal point
  * @retval None
  */
void SWV_printfloat(double number, int digits)
{
 int i = 0;
 //handle negative numbers
 if(number < 0.0)
 {
  ITM_SendChar('-');
  number = -number;
 }
 //round correctly so that uart_printfloat(1.999, 2) shows as "2.00"
 double rounding = 0.5;
 for(i = 0; i < digits; ++i) rounding = rounding / 10.0;
 number = number + rounding;

 //extract the integer part of the number and print it
 unsigned long int_part = (unsigned long) number;
 double remainder = (double)(number - (double)int_part);
 SWV_printnum(int_part); //print the integer part
 if(digits > 0) ITM_SendChar('.'); //print decimal pint
 int toprint;
 while(digits-- > 0)
 {
  remainder = remainder * 10.0;
  toprint = (int)remainder;
  SWV_printnum(toprint);
  remainder = remainder - toprint;
 }

}

/*-----------------------------------------------------------------------------*/
void MagicFX(uint8_t val) /* random effects parameters */
{
	if (val == MIDI_MAX)
	{
		SWV_puts("hello\n");
		Delay_switch(MIDI_MAXi);
		Delay_time_set(MIDIrandVal());
		DelayWet_set(MIDIrandVal());
		DelayFeedback_set(MIDIrandVal());

		Chorus_switch(MIDI_MAXi);
		ChorusRate_set(MIDIrandVal());
		ChorusSecondRate_set(MIDIrandVal());
		ChorusDelay_set(MIDIrandVal());
		ChorusSweep_set(MIDIrandVal());
		ChorusFeedback_set(MIDIrandVal());
		ChorusMode_switch(MIDIrandVal());
		ChorusFDBsign_switch(MIDIrandVal());

		Phaser_switch(MIDI_MAXi);
		Phaser_Rate_set(MIDIrandVal());
		Phaser_Feedback_set(MIDIrandVal());
		Phaser_Wet_set(MIDIrandVal());
	}
}
// Reggie added, lets make the magic not so random
// Quite often the magicFX can sound like mush
// so here's some functions to only randomise each FX unit individually

void MagicDelay(uint8_t val){
  	if (val == MIDI_MAX)
	{
		Delay_switch(MIDI_MAXi);
		Delay_time_set(MIDIrandVal());
		DelayWet_set(MIDIrandVal());
		DelayFeedback_set(MIDIrandVal());
	}
}

void MagicPhaser(uint8_t val){
  	if (val == MIDI_MAX)
	{
		Phaser_switch(MIDI_MAXi);
		Phaser_Rate_set(MIDIrandVal());
		Phaser_Feedback_set(MIDIrandVal());
		Phaser_Wet_set(MIDIrandVal());
	}
}

void MagicChorus(uint8_t val){
  	if (val == MIDI_MAX)
	{
		Chorus_switch(MIDI_MAXi);
		ChorusRate_set(MIDIrandVal());
		ChorusSecondRate_set(MIDIrandVal());
		ChorusDelay_set(MIDIrandVal());
		ChorusSweep_set(MIDIrandVal());
		ChorusFeedback_set(MIDIrandVal());
		ChorusMode_switch(MIDIrandVal());
		ChorusFDBsign_switch(MIDIrandVal());
	}
}



/*-----------------------------------------------------------------------------*/
void MagicPatch(uint8_t val) /* random sound parameters */
{
	if (val == MIDI_MAX)
	{
		seq_tempo_set(MIDIrandVal());
		seq_freqMax_set(MIDIrandVal());
		seq_scale_set(MIDIrandVal());
		seq_switchMovingSeq(MIDIrandVal());
		seq_switchMute(MIDIrandVal());
		seq_gateTime_set(MIDIrandVal());
		autoSound_set(rand()%3);
		Sound_set(MIDIrandVal());
		uint8_t snd = soundNumber_get();

		if (snd == FM2)
		{
			//STM_EVAL_LEDOn(LED_Blue);
			FM_OP1_freq_set(MIDIrandVal());
			FM_OP1_modInd_set(MIDIrandVal());
			FM_OP2_freq_set(MIDIrandVal());
			FM_OP2_modInd_set(MIDIrandVal());
			FM_OP3_freq_set(MIDIrandVal());
			FM_OP3_modInd_set(MIDIrandVal());
			FM_OP4_freq_set(MIDIrandVal());
			FM_OP4_modInd_set(MIDIrandVal());
		}
		else if (snd == DRIFTERS)
		{
			//STM_EVAL_LEDOn(LED_Green);
			Drifter_amp_set(MIDIrandVal());
			Drifter_minFreq_set(MIDIrandVal());
			Drifter_maxFreq_set(MIDIrandVal());
			Drifter_centralFreq_set(MIDIrandVal());
		}

		Filter1Freq_set(MIDIrandVal());
		Filter1Res_set(MIDIrandVal());
		Filter1Drive_set(MIDIrandVal());
		Filter1Type_set(MIDIrandVal());
		Filt1LFO_amp_set(MIDIrandVal());
		Filt1LFO_freq_set(MIDIrandVal());

		Filter2Freq_set(MIDIrandVal());
		Filter2Res_set(MIDIrandVal());
		Filter2Drive_set(MIDIrandVal());
		Filter2Type_set(MIDIrandVal());
		Filt2LFO_amp_set(MIDIrandVal());
		Filt2LFO_freq_set(MIDIrandVal());

		Filter_Random_switch(MIDIrandVal());

		AttTime_set((uint8_t)lrintf(frand_a_b(0 , MIDI_MAX/10)));
		DecTime_set(MIDIrandVal());
		SustLevel_set(MIDIrandVal());
		RelTime_set(MIDIrandVal());

		VibratoAmp_set(MIDIrandVal());
		VibratoFreq_set(MIDIrandVal());

		AmpLFO_amp_set(MIDIrandVal());
		AmpLFO_freq_set(MIDIrandVal());

	}
}
/*******************************************************************************
 * This function is called by USBH_MIDI_Handle(...) from file "usbh_midi_core.c"
 *
 * To fix : The first 4 bytes only of the data buffer  are interpreted, but there might be
 * several MIDI events in that 64 bytes buffer....
 *
 * *****************************************************************************/
void MIDI_Decode(uint8_t * outBuf)
{
        // Midi channels are referenced from 0 internally, the end user references from 1
        // Use chan to start filtering midi data, use chan 0 (user midi channel 1) for stock 7bit midi data
        // use chan 1 (user midi channel 2) for high resolution control via rotary encoders
	uint8_t chan = outBuf[1] & 0x0F;
	uint8_t val = 0;
        uint16_t val2 = 0;
	if (outBuf[1] != 0x00) start_LED_On(LED_Blue, 8);

        //If the midi message is a Note On
        //currently answering to all channels
        //this is the right place to implement
        //  channel discrimination in the future       
	//  note#     velocity
	if ((outBuf[1] & 0xF0) == 0x90){
	  if ((outBuf[3]) == 0x00) // because a note on with zero vel is in fact a note off
                   noteToBeKilled(outBuf[2]);
          else
                   newNoteArrived(outBuf[2],outBuf[3]);
	}
	if ((outBuf[1] & 0xF0) == 0x80) noteToBeKilled(outBuf[2]);
        //as far as MIDI is concerned, we have finished our job, now
        // it is up to the synth to figure glides, polyphony and whatnot 	
        //TODO we could put all the MIDI constants in an external file
        // like a yaml or an xml


	/* If the midi message is a Control Change... */
        // Reggie added, the start of sorting things out, we'll start sniffing for messages
        // from channel 2 (internally referenced from 0, 1 = channel 2)
        // I've chosen this channel for rotary encoder support, it gives the finest resolution
        // you can set the resolution as fine or as coarse as is necessary for a given control
        // this means that adjusting the tempo by 0.01 increments is possible, frequency by sub 0.1 
        // it also means that controls with < 127 values can be accessed directly instead of
        // doing division and having to do the equivalent of a full rotation to get to 5 values
        // quick thought for pattern edit mode, set the edit mode with a button, use 1 encoder per
        // step in the sequence
	if (chan == 1){
          if ((outBuf[1] & 0xF0) == 0xB0)
          {
            val = outBuf[3];
            switch(outBuf[2])
            {
		case 1 	: 	sequencer_toggle(val); 		break ;	// toggle sequencer on/off
		case 7  :       inc_dec_volume(val);            break;  // master volume via encoder
		case 39 :	MagicPatch(val);		break;	// random settings except effects
		case 38 :	MagicFX(val);			break;	// random settings for effects
		case 82 :	MagicPatch(val);		break;	//
		case 81 :	MagicFX(val);			break;	//
		case 44 :	MagicDelay(val);                break;	// Reggie Added to tame the magicFX
		case 45 :	MagicPhaser(val);               break;	// Reggie Added
		case 46 :	MagicChorus(val);               break;	// Reggie Added
		case 47 :	Delay_reset(val);		break;  // Reggie Added
                case 48 :	Phaser_reset(val);		break;  // Reggie Added
		case 6  :	Sound_incdec(val);		break;	// Reggie changed, waveform
		case 8  :	Filter1Freq_incdec(val);	break;	// Reggie changed, encoder testing
		case 9  :	Filt1LFO_freq_incdec(val);	break;	// Reggie changed, encoder testing
//		case 10 :	Filter3Freq_incdec(val);	break;	// Reggie changed, encoder testing
//		case 11 :	Filt3LFO_freq_incdec(val);	break;	// Reggie changed, encoder testing
//		case 91 :	Filter3Res_set(val);		break;	//
//		case 92 :	Filter3Drive_set(val);		break;	//
//		case 93 :	Filter3Type_set(val);		break;	//
//		case 99 : 	inc_dec_tempo(val);          	break;  // Reggie added
		case 99  :       lfo_phase_offset1(val);         break;  // Reggie added
//		case 100 : 	toggle_retrigger(val);          break;  // Reggie added, re-trigger filter lfo
//		case 101 : 	toggle_retrigger2(val);         break;  // Reggie added, re-trigger filter2 lfo
              }
	}
 }
      if (chan == 0){
	if ((outBuf[1] & 0xF0) == 0xB0)
	{
		val = outBuf[3];

		switch(outBuf[2])
		{
		case 1 	: 	sequencer_toggle(val); 		break ;	// toggle sequencer on/off
		case 3 	: 	seq_tempo_set(val); 		break ;	// tempo
		case 13 : 	Volume_set(val); 		break;	// master volume
		case 7  :       inc_dec_volume(val);            break;  // master volume via encoder
		case 31 :	SynthOut_switch(val); 		break;  // toggle synth output
		case 4  : 	seq_freqMax_set(val);		break;	// max frequency
		case 67 :	DemoMode_toggle(val);		break;
		//case 76 :	DemoMode_freeze(val);		break;
		case 39 :	MagicPatch(val);		break;	// random settings except effects
		case 38 :	MagicFX(val);			break;	// random settings for effects
		case 82 :	MagicPatch(val);		break;	//
		case 81 :	MagicFX(val);			break;	//
		case 44 :	MagicDelay(val);                break;	// Reggie Added to tame the magicFX
		case 45 :	MagicPhaser(val);               break;	// Reggie Added
		case 46 :	MagicChorus(val);               break;	// Reggie Added
		case 47 :	Delay_reset(val);		break;  // Reggie Added
                case 48 :	Phaser_reset(val);		break;  // Reggie Added
		case 5  :	seq_scale_set(val);		break; 	// scale
		case 6  :	Sound_set(val);			break;	// waveform
		case 76 :	RandSound1(val);		break;
		case 77 :	RandSound2(val);		break;
		case 8  :	Filter1Freq_incdec(val);	break;	// Reggie changed, encoder testing
		case 9  :	Filt1LFO_freq_incdec(val);	break;	// Reggie changed, encoder testing
		case 10 :	Filter2Freq_incdec(val);	break;	// Reggie changed, encoder testing
		case 11 :	Filt2LFO_freq_incdec(val);	break;	// Reggie changed, encoder testing
		case 53 :	Filter1Freq_set(val);		break;	//
		case 54 :	Filter1Res_set(val);		break;	//
		case 12 :	Filter1Drive_set(val);		break;	//
		case 55 :	Filter1Drive_set(val);		break;	//
		case 56 :	Filter1Type_set(val);		break;	//
		case 23 :	Delay_switch(val);		break;	// Delay ON/OFF
		case 14 :	Delay_time_set(val);		break;	// Delay time
		case 15 :	DelayFeedback_set(val);		break;	// Delay feedback
		case 2  :	DelayWet_set(val);		break;	// Delay wet signal amplitude
		case 40 :	Delay_time_dec(val);		break;
		case 41 :	Delay_time_inc(val);		break;
		case 16 :	VibratoAmp_set(val);		break;	// Vibrato amplitude
		case 17 :	VibratoFreq_set(val);		break;	// Vibrato frequency
		case 27 :	Filter_Random_switch(val);	break;	// Filter ON/OFF
		case 63 :	SynthOut_amp_set(val);		break;	// Distorsion
		case 28 :	Chorus_switch(val);		break;	// Chorus ON/OFF
		case 37 :	Chorus_reset(val);		break;
		case 18 :	ChorusRate_set(val);		break;	// Chorus rate
		case 22 :	ChorusSecondRate_set(val);	break;	// Chorus relative rate for LFO right
		case 19 :	ChorusDelay_set(val);		break;	// Chorus delay
		case 20 :	ChorusSweep_set(val);		break;	// Chorus sweep
		case 21 :	ChorusFeedback_set(val);	break;	// Chorus feedback
		case 29 :	ChorusMode_switch(val);		break;	// Chorus mode
		case 30 :	ChorusFDBsign_switch(val);	break;	// Chorus fdb sign
		case 24 :	Phaser_switch(val);		break;
		case 102 :	Phaser_Rate_set(val);		break;
		case 103 :	Phaser_Feedback_set(val);	break;
		case 89 :	Phaser_Wet_set(val);		break;
		case 26 :	seq_switchMovingSeq(val);	break;	// toggle sequence mode
		//case 24 :	seq_switchGlide(val);		break; 	// toggle glissando
		case 25 :	seq_switchMute(val);		break;	// toggle muted notes
		case 33 :	seq_transp(-2, val);		break;	// transpose 1 tone down
		case 34 :	seq_transp(2, val);		break;	// transpose 1 tone up
		case 35 :	seq_transp(-7, val);		break;	//
		case 36 :	seq_transp( 7, val);		break;	//
		case 42 :	seq_gateTime_set(val);		break;	//
		case 43 :	AttTime_set(val);		break;	//
		case 50 :	DecTime_set(val);		break;	//
		case 51 :	SustLevel_set(val);		break;	//
		case 52 :	RelTime_set(val);		break;	//
		case 65 :	Filt1LFO_amp_set(val);		break;	//
		case 66 :	Filt1LFO_freq_set(val);		break;	//
		case 57 :	AmpLFO_amp_set(val);		break;	//
		case 58 :	AmpLFO_freq_set(val);		break;	//
		case 90 :	Filter2Freq_set(val);		break;	//
		case 91 :	Filter2Res_set(val);		break;	//
		case 92 :	Filter2Drive_set(val);		break;	//
		case 93 :	Filter2Type_set(val);		break;	//
		case 105 :	Filt2LFO_amp_set(val);		break;	//
		case 106 :	Filt2LFO_freq_set(val);		break;	//
		case 85 : 	FM_OP1_freq_set(val);		break;
		case 94 :	FM_OP1_modInd_set(val);		break;
		case 86 : 	FM_OP2_freq_set(val);		break;
		case 95 :	FM_OP2_modInd_set(val);		break;
		case 108 : 	FM_OP2_freqMul_inc(val);	break;
		case 117 :	FM_OP2_freqMul_dec(val);	break;
		case 87 : 	FM_OP3_freq_set(val);		break;
		case 96 :	FM_OP3_modInd_set(val);		break;
		case 109 : 	FM_OP3_freqMul_inc(val);	break;
		case 118 :	FM_OP3_freqMul_dec(val);	break;
		case 88 : 	FM_OP4_freq_set(val);		break;
		case 97 :	FM_OP4_modInd_set(val);		break;
		case 110 : 	FM_OP4_freqMul_inc(val);	break;
		case 119 :	FM_OP4_freqMul_dec(val);	break;
  		case 59 : 	Drifter_amp_set(val);		break;
		case 60 : 	Drifter_minFreq_set(val);	break;
		case 61 : 	Drifter_maxFreq_set(val);	break;
		case 62 : 	Drifter_centralFreq_set(val);	break;
		case 99 : 	inc_dec_tempo(val);          	break; // Reggie added
		}
	}
      }
}
