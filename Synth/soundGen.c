/**
 ******************************************************************************
 * File Name          : soundGen.c
 * Author			  : Xavier Halgand
 * Date               :
 * Description        :
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


#include "soundGen.h"

/*-------------------------------------------------------*/

#define EPSI				.00002f
//#define MAX_SOUNDS			14 // number - 1 of different sounds (starts at 0)

/*-------------------------------------------------------*/

extern bool					demoMode;
extern bool					freeze;

extern Sequencer_t 			seq;
extern NoteGenerator_t 		noteG;

extern Oscillator_t 		op1 _CCM_;
extern Oscillator_t 		op2 _CCM_;
extern Oscillator_t 		op3 _CCM_;
extern Oscillator_t 		op4 _CCM_;

extern VCO_blepsaw_t		mbSawOsc;
extern VCO_bleprect_t		mbRectOsc;
extern VCO_bleptri_t		mbTriOsc;

extern Oscillator_t 		vibr_lfo _CCM_;
extern Oscillator_t 		filt_lfo _CCM_;
extern Oscillator_t 		filt2_lfo _CCM_;
//extern Oscillator_t 		filt3_lfo _CCM_;
extern Oscillator_t 		amp_lfo _CCM_;

extern ResonantFilter 	SVFilter;
extern ResonantFilter 	SVFilter2;
extern ResonantFilter 	SVFilter3;

extern float			filterFreq;
extern float			filterFreq2;
extern float			filterFreq3;
extern ADSR_t 			adsr;

//extern EightSegGenerator 	pitchGen _CCM_ ;

//hardcoded now for simplicity, needs to be dealt with according to the other switches here.
static bool        sequencerON _CCM_;

static bool 		autoFilterON _CCM_;
static bool		delayON _CCM_;
static bool		phaserON _CCM_;
static bool 		chorusON _CCM_;
static int8_t		autoSound _CCM_;

static float		f0 _CCM_ ;
static float 		vol  _CCM_;
static float		env  _CCM_;
static enum timbre	sound _CCM_;


/*===============================================================================================================*/

void autoSound_set(int8_t val)
{
	autoSound = val;
}
/*---------------------------------------------------------*/
void RandSound1(uint8_t val) /* random series of tones */
{
	if (val == MIDI_MAX)
	{
		if (autoSound == 0)
			autoSound = 1;
		else
			autoSound = 0;
	}
}
/*---------------------------------------------------------*/
void RandSound2(uint8_t val) /* random series of tones */
{
	if (val == MIDI_MAX)
	{
		if (autoSound == 0)
			autoSound = 2;
		else
			autoSound = 0;
	}
}
/*---------------------------------------------------------*/
uint8_t soundNumber_get(void)
{
	return sound;
}
/*---------------------------------------------------------*/
void Parameter_fine_tune(uint8_t val)
{

}
/*---------------------------------------------------------*/
void DemoMode_toggle(uint8_t val)
{
	if (val == MIDI_MAX)
	{
		demoMode = !demoMode;
	}
}
//void DemoMode_freeze(uint8_t val)
//{
//	if (val == MIDI_MAX)
//	{
//		freeze = !freeze;
//	}
//}
 /*-------------------------------------------------------*/
void sequencer_toggle(uint8_t val)
{
	if (val == MIDI_MAX)
	{
		sequencerON = !sequencerON;
	}

}
/*-------------------------------------------------------*/
void AmpLFO_amp_set(uint8_t val)
{
	amp_lfo.amp = val / MIDI_MAX;
}
/*-------------------------------------------------------*/
void AmpLFO_freq_set(uint8_t val)
{
	amp_lfo.freq = MAX_VIBRATO_FREQ / MIDI_MAX * val;
}
/*-------------------------------------------------------*/
void Filt1LFO_amp_set(uint8_t val)
{
	filt_lfo.amp = MAX_FILTER_LFO_AMP / MIDI_MAX * val;
}
/*-------------------------------------------------------*/
void Filt1LFO_freq_set(uint8_t val)
{
	filt_lfo.freq = MAX_VIBRATO_FREQ / MIDI_MAX * val;
}
/*-------------------------------------------------------*/
void Filt1LFO_freq_incdec(uint8_t val)
{
  if (val <= 0x0F){
    if (filt_lfo.freq + (0.1f *val) <= MAX_FLT_LFO_FREQ){
	filt_lfo.freq += 0.1f;
	   } else {
             filt_lfo.freq = MAX_FLT_LFO_FREQ;
           }
	 } else if (val >= 0x70){
           val &= 0x0F; // to get a value between 0-15
           val = (0x10 - val); // flip our value
           if (filt_lfo.freq - (0.1f *val) >= 0){
               filt_lfo.freq -= 0.1f;
         } else {
          filt_lfo.freq=0;
         }
         }

}
/*-------------------------------------------------------*/
void Filt2LFO_amp_set(uint8_t val)
{
	filt2_lfo.amp = MAX_FILTER_LFO_AMP / MIDI_MAX * val;
}
/*-------------------------------------------------------*/
void Filt2LFO_freq_set(uint8_t val)
{
	filt2_lfo.freq = MAX_VIBRATO_FREQ / MIDI_MAX * val;
}
/*-------------------------------------------------------*/

void Filt2LFO_freq_incdec(uint8_t val)
{
  // if we want finer control of the lfo freq to be able to hit divisions of bars, 2 bars etc.
  // then we'll need to reduce the 0.1f value
  if (val <= 0x0F){
    if (filt2_lfo.freq + (0.1f *val) <= MAX_FLT_LFO_FREQ){
	filt2_lfo.freq += 0.1f*val;
	   } else {
             filt2_lfo.freq = MAX_FLT_LFO_FREQ;
           }
	 } else if (val >= 0x70){
           val &= 0x0F; // to get a value between 0-15
           val = (0x10 - val); // flip our value
           if (filt2_lfo.freq - (0.1f * val) >= 0){
               filt2_lfo.freq -= 0.1f * val;
         } else {
          filt2_lfo.freq = 0;
         }
         }

}

/*
void Filt3LFO_amp_set(uint8_t val)
{
	filt3_lfo.amp = MAX_FILTER_LFO_AMP / MIDI_MAX * val;
}

void Filt3LFO_freq_set(uint8_t val)
{
	filt3_lfo.freq = MAX_VIBRATO_FREQ / MIDI_MAX * val;
}

void Filt3LFO_freq_incdec(uint8_t val)
{
  // if we want finer control of the lfo freq to be able to hit divisions of bars, 2 bars etc.
  // then we'll need to reduce the 0.1f value
  if (val <= 0x0F){
    if (filt3_lfo.freq + (0.1f *val) <= MAX_FLT_LFO_FREQ){
	filt3_lfo.freq += 0.1f*val;
	   } else {
             filt3_lfo.freq = MAX_FLT_LFO_FREQ;
           }
	 } else if (val >= 0x70){
           val &= 0x0F; // to get a value between 0-15
           val = (0x10 - val); // flip our value
           if (filt3_lfo.freq - (0.1f * val) >= 0){
               filt3_lfo.freq -= 0.1f * val;
         } else {
          filt3_lfo.freq = 0;
         }
         }
}
*/
/*-------------------------------------------------------*/
void toggleVibrato(void)
{
	if (vibr_lfo.amp !=0)
	{
		vibr_lfo.last_amp = vibr_lfo.amp;
		vibr_lfo.amp = 0;
	}
	else
		vibr_lfo.amp = vibr_lfo.last_amp;
}
/*-------------------------------------------------------*/
void VibratoAmp_set(uint8_t val)
{
	vibr_lfo.amp = MAX_VIBRATO_AMP / MIDI_MAX * val;
}
/*-------------------------------------------------------*/
void VibratoFreq_set(uint8_t val)
{
	vibr_lfo.freq = MAX_VIBRATO_FREQ / MIDI_MAX * val;
}
/*-------------------------------------------------------*/
void toggleSynthOut(void)
{
	if (op1.amp !=0)
	{
		op1.last_amp = op1.amp;
		op1.amp = 0;
		op2.last_amp = op2.amp;
		op2.amp = 0;
		op3.last_amp = op3.amp;
		op3.amp = 0;
	}
	else
	{
		op1.amp = op1.last_amp;
		op2.amp = op2.last_amp;
		op3.amp = op3.last_amp;
	}
}
/*-------------------------------------------------------*/
void SynthOut_switch(uint8_t val)
{
	//if (val == 127) toggleSynthOut();
	switch (val)
	{
	case MIDI_MAXi :
		op1.amp = op1.last_amp;
		op2.amp = op2.last_amp;
		op3.amp = op3.last_amp;
		mbSawOsc.amp = mbSawOsc.last_amp;
		mbRectOsc.amp = mbRectOsc.last_amp;
		mbTriOsc.amp = mbTriOsc.last_amp;
		break;

	case 0 :
		op1.last_amp = op1.amp;
		op1.amp = 0;
		op2.last_amp = op2.amp;
		op2.amp = 0;
		op3.last_amp = op3.amp;
		op3.amp = 0;
		mbSawOsc.last_amp = mbSawOsc.amp;
		mbSawOsc.amp = 0;
		mbRectOsc.last_amp = mbRectOsc.amp;
		mbRectOsc.amp = 0;
		mbTriOsc.last_amp = mbTriOsc.amp;
		mbTriOsc.amp = 0;
		break;
	}
}

/*-------------------------------------------------------*/
void incSynthOut(void)
{
	op1.amp *= 1.2f;
	op2.amp *= 1.2f;
	op3.amp *= 1.2f;
}
/*-------------------------------------------------------*/
void decSynthOut(void)
{
	op1.amp *= .8f;
	op2.amp *= .8f;
	op3.amp *= .8f;
}
/*-------------------------------------------------------*/
void SynthOut_amp_set(uint8_t val)
{
	float_t amp;
	amp = (val * 5.f / MIDI_MAX)*(val * 5.f / MIDI_MAX);
	op1.amp = amp;
	op2.amp = amp;
	op3.amp = amp;
	mbSawOsc.amp = amp;
	mbRectOsc.amp = amp;
	mbTriOsc.amp = amp;
}
/*-------------------------------------------------------*/
void Delay_toggle(void)
{
	if (delayON)
	{
		delayON = false;
		Delay_clean();
	}
	else delayON = true;
}
/*-------------------------------------------------------*/
void Delay_switch(uint8_t val)
{

	if (val > 63)
		delayON = true;
	else
		delayON = false;
}
/*-------------------------------------------------------*/
void toggleFilter(void)
{
	if (autoFilterON) autoFilterON = false;
	else autoFilterON = true;
}
/*-------------------------------------------------------*/
void Filter_Random_switch(uint8_t val)
{
	if (val > 63)
		autoFilterON = true;
	else
		autoFilterON = false;
}
/*-------------------------------------------------------*/
void Chorus_toggle(void)
{
	if (chorusON) chorusON = false;
	else chorusON = true;
}
/*-------------------------------------------------------*/
void Chorus_switch(uint8_t val)
{

	if (val > 63)
		chorusON = true;
	else
		chorusON = false;
}
/*-------------------------------------------------------*/
void Phaser_switch(uint8_t val)
{

	if (val > 63)
		phaserON = true;
	else
		phaserON = false;
}
/*-------------------------------------------------------*/
void nextSound(void)
{
	if (sound < LAST_SOUND) (sound)++ ; else sound = LAST_SOUND;
}
/*-------------------------------------------------------*/
void prevSound(void)
{
	if (sound > 0) (sound)-- ; else sound = 0;
}
/*-------------------------------------------------------*/
void Sound_set(uint8_t val)
{
	sound = (uint8_t) rintf((LAST_SOUND - 1) / MIDI_MAX * val);
	if (sound != ADDITIVE) AdditiveGen_newWaveform();
}

// Reggie added, small increment/decrement function to change the sound
// doesn't need to worry about acceleration as it's only 16? values
void Sound_incdec(uint8_t val){
  if (val <= 0x0F){
    if (sound < LAST_SOUND )
    sound++;
    } else if (val >= 0x70){
           if ( sound > 0){
             sound--;
             }
    }
  if (sound != ADDITIVE) AdditiveGen_newWaveform();
}


/*******************************************************************************************************************************/

void FM_OP1_freq_set(uint8_t val)
{
	FM_op_freq_set(&op1, val);
}
/*-------------------------------------------------------*/
void FM_OP1_modInd_set(uint8_t val)
{
	FM_op_modInd_set(&op1, val);
}

/*----------------------------------------------------------------------------------------------------------------------------*/
void FM_OP2_freq_set(uint8_t val)
{
	//FM_op_freq_set(&op2, val);
	op2.mul = Lin2Exp(val, 0.2f, 32.f); // the freq of op2 is a multiple of the main pitch freq (op1)
	//op2.mul = roundf(32 * val/MIDI_MAX);
}
/*-------------------------------------------------------*/
void FM_OP2_freqMul_inc(uint8_t val)
{
	if (val == MIDI_MAX)
	{
		op2.mul *= 1.01f;
	}
}
/*-------------------------------------------------------*/
void FM_OP2_freqMul_dec(uint8_t val)
{
	if (val == MIDI_MAX)
	{
		op2.mul *= 0.99f;
	}
}
/*-------------------------------------------------------*/
void FM_OP2_modInd_set(uint8_t val)
{
	FM_op_modInd_set(&op2, val);
}

/*------------------------------------------------------------------------------------------------------------------------------*/
void FM_OP3_freq_set(uint8_t val)
{
	//FM_op_freq_set(&op3, val);
	op3.mul = Lin2Exp(val, 0.2f, 32.f); // the freq of op3 is a multiple of the main pitch freq (op1)
}
/*-------------------------------------------------------*/
void FM_OP3_modInd_set(uint8_t val)
{
	FM_op_modInd_set(&op3, val);
}
/*-------------------------------------------------------*/
void FM_OP3_freqMul_inc(uint8_t val)
{
	if (val == MIDI_MAX)
	{
		op3.mul *= 1.01f;
	}
}
/*-------------------------------------------------------*/
void FM_OP3_freqMul_dec(uint8_t val)
{
	if (val == MIDI_MAX)
	{
		op3.mul *= 0.99f;
	}
}

/*--------------------------------------------------------------------------------------------------------------------------*/
void FM_OP4_freq_set(uint8_t val)
{
	//FM_op_freq_set(&op4, val);
	op4.mul = Lin2Exp(val, 0.2f, 32.f); // the freq of op4 is a multiple of the main pitch freq (op1)
}
/*-------------------------------------------------------*/
void FM_OP4_modInd_set(uint8_t val)
{
	FM_op_modInd_set(&op4, val);
}
/*-------------------------------------------------------*/
void FM_OP4_freqMul_inc(uint8_t val)
{
	if (val == MIDI_MAX)
	{
		op4.mul *= 1.01f;
	}
}
/*-------------------------------------------------------*/
void FM_OP4_freqMul_dec(uint8_t val)
{
	if (val == MIDI_MAX)
	{
		op4.mul *= 0.99f;
	}
}

void lfo_phase_offset1(uint8_t val){
   lfo_phase_offset(&filt_lfo,val);
}
/*===============================================================================================================*/

void
Synth_Init(void)
{
	vol = env = 1;
	sound = MORPH_SAW;
	autoFilterON = false;
	autoSound = 0;
	chorusON = false;
	delayON = false;
	phaserON = false;
    sequencerON = false;

	Delay_init();
	drifter_init();
	//pitchGen_init();
	sequencer_init();
	ADSR_init(&adsr);
	Chorus_init();
	PhaserInit();
	SVF_init();
	filterFreq = 0.25f;
	filterFreq2 = 0.25f;
	osc_init(&op1, 0.8f, 587.f, false);
	osc_init(&op2, 0.8f, 587.f, false);
	osc_init(&op3, 0.8f, 587.f, false);
	osc_init(&op4, 0.8f, 587.f, false);
	osc_init(&vibr_lfo, 0, VIBRATO_FREQ, false);
	osc_init(&filt_lfo, 0, 0, true);
	osc_init(&filt2_lfo, 0, 0, true);
//	osc_init(&filt3_lfo, 0, 0, true);
	osc_init(&amp_lfo, 0, 0, false);
	AdditiveGen_newWaveform();
	VCO_blepsaw_Init(&mbSawOsc);
	VCO_bleprect_Init(&mbRectOsc);
	VCO_bleptri_Init(&mbTriOsc);
}
/*---------------------------------------------------------------------------------------*/

void sequencer_newStep_action(void) // User callback function called by sequencer_process()
{
	if ((noteG.automaticON || noteG.chRequested))
	{
		seq_sequence_new(); // This is where the sequence gets loaded in, only called once
		                    // At first run and then any time the user changes pattern
		noteG.chRequested = false;
		AdditiveGen_newWaveform();
	}
        // should add a velocity check, that way some notes could be muted in the patterns?
        // || seq.track1.veloc[seq.step_idx] == 0 or check for a seq.track1.note value >11 
        // (notes indexed from 0-11) the latter would mean not implementing velocity
	if ( (noteG.someNotesMuted) && (rintf(frand_a_b(0.4f , 1)) == 0) )
		ADSR_keyOff(&adsr);
	else    {
		ADSR_keyOn(&adsr);
               osc_phase_reset(&filt_lfo);
               osc_phase_reset(&filt2_lfo);
//               osc_phase_reset(&filt3_lfo);
                }

	if (autoFilterON)
		SVF_directSetFilterValue(&SVFilter, Ts * 600.f * powf(5000.f / 600.f, frand_a_b(0 , 1)));

	if (noteG.transpose != 0)
	{
		noteG.rootNote += noteG.transpose ;
		seq_transpose();
	}

	if (autoSound == 1)
	{
		switch(rand() % 4) // 4 random timbers
		{
		case 0 : sound = CHORD15; break;
		case 1 : AdditiveGen_newWaveform(); sound = ADDITIVE; break;
		case 2 : sound = CHORD13min5; break;
		case 3 : sound = VOICES3; break;
		}
	}
	if (autoSound == 2)
	{
		sound = rand() % LAST_SOUND;
		if ((sound == CHORD13min5) || (sound == CHORD135))
			sound = VOICES3;
		if ( sound == ADDITIVE)
			AdditiveGen_newWaveform();

		//		switch (sound)
		//			{
		//			case 0 : sound = MORPH_SAW ; 	break ;
		//
		//			case 0 : sound = SPLIT ;	 	break;
		//
		//			case 0 : sound = ACC_SINE ;		break;
		//
		//			case 0 : sound = POWER_SINE ;	break;
		//
		//			case 0 : sound = BLEPTRIANGLE ; break;
		//
		//			case 0 : sound = BLEPSQUARE ;	break;
		//
		//			case 0 : sound = WT_SINE ;		break;
		//
		//			case 0 : sound = ADDITIVE ;	break;
		//
		//			case 0 : sound = NOISE ;	break; // noise !
		//
		//			case 0 : sound = CHORD15 ; break;
		//
		//			case 0 : sound = CHORD135 ; break;
		//
		//			case 0 : sound = CHORD13min5 ; break;
		//
		//			case 0 : sound = VOICES3 ;break;
		//
		//			case 0 : sound = DRIFTERS ; break;
		//
		//			case 0 : sound = FM2 ; break;
		//
		//			case 0 : sound = BLEPSAW ; break;
		//
		//			}
	}

	f0 = notesFreq[seq.track1.note[seq.step_idx]]; // Main "melody" frequency
	vol = frand_a_b(0.4f , .8f); // slightly random volume for each note
}
/*---------------------------------------------------------------------------------------*/

void sequencer_newSequence_action(void) // User callback function called by sequencer_process()
{
	/* A new sequence begins ... */
	if ((demoMode == true)&&(freeze == false))
	{
		MagicPatch(MIDI_MAXi);
		MagicFX(MIDI_MAXi);
	}
}
/*===============================================================================================================*/
static char currentNote = 0;

void newNoteArrived(char noteNumber, char velocity){  //called by note on's, at midi_interface.c
  currentNote = noteNumber;
  // calculate the frequency
  f0 = notesFreq[noteNumber-21];
  if (ADSR_getState(&adsr)==RELEASE || ADSR_getState(&adsr)==DONE )
    ADSR_keyOn(&adsr); //this sets the state to attack, directly
    //otherwise we just change the frequency, allowing legato to be played
    osc_phase_reset(&filt_lfo);
    osc_phase_reset(&filt2_lfo);
//    osc_phase_reset(&filt3_lfo);

}
void noteToBeKilled(char noteNumber){ //called by note off's, at midi_interface.c
    if(noteNumber == currentNote)
     ADSR_keyOff(&adsr);
    //otherwise do nothing, because the player is doing legato
}

void make_sound(uint16_t *buf , uint16_t length) // To be used with the Sequencer
{

	uint16_t 	pos;
	uint16_t 	*outp;
	float	 	y = 0;
	float	 	yL, yR ;
	float 		f1;
	uint16_t 	valueL, valueR;

	outp = buf;

	for (pos = 0; pos < length; pos++)
	{
		/*--- Sequencer actions and update ---*/
		
	        if (sequencerON) sequencer_process(); //computes f0 and calls sequencer_newStep_action() and sequencer_newSequence_action()
                //else monophonicKeyInterface(); //computes f0 and starts/stops the ADSR, based on keyboard input
                //probably not needed, just call stuff from the midi events.

		/*--- compute vibrato modulation ---*/
		f1 = f0 * (1 +  Osc_WT_SINE_SampleCompute(&vibr_lfo));

		/*--- Generate waveform ---*/
		y = waveCompute(sound, f1);

		/*--- Apply envelop and tremolo ---*/
		env = ADSR_computeSample(&adsr) * (1 + Osc_WT_SINE_SampleCompute(&amp_lfo));
		y *= vol * env; // apply volume and envelop
		if (adsr.cnt_ >= seq.gateTime) if (sequencerON) ADSR_keyOff(&adsr);

		/*--- Apply filter effect ---*/
		/* Update the filters cutoff frequencies */
		if ((! autoFilterON)&&(filt_lfo.amp != 0)){
                  SVF_directSetFilterValue(&SVFilter, filterFreq * (1 + OpSampleCompute7bis(&filt_lfo)));
                  }


		if ((! autoFilterON)&&(filt2_lfo.amp != 0)){
                  SVF_directSetFilterValue(&SVFilter2, filterFreq2 * (1 + OpSampleCompute7bis(&filt2_lfo)));
                  }

		y = 0.5f * (SVF_calcSample(&SVFilter, y) + SVF_calcSample(&SVFilter2, y)); // Two filters in parallel

		/*---  Apply delay effect ----*/
		if (delayON) 	y = Delay_compute(y);

		/*---  Apply phaser effect ----*/
		if (phaserON) 	y = Phaser_compute(y);

		/*--- Apply chorus/flanger effect ---*/
		if (chorusON) stereoChorus_compute (&yL, &yR, y) ;
		else yL = yR = y;
/*
 		if ((filt3_lfo.amp != 0)){
                  SVF_directSetFilterValue(&SVFilter3, filterFreq3 * (1 + OpSampleCompute7bis(&filt3_lfo)));
                  }
		y = 0.5f * (SVF_calcSample(&SVFilter3, y));
*/
		/*--- clipping ---*/
		yL = (yL > 1.0f) ? 1.0f : yL; //clip too loud left samples
		yL = (yL < -1.0f) ? -1.0f : yL;

		yR = (yR > 1.0f) ? 1.0f : yR; //clip too loud right samples
		yR = (yR < -1.0f) ? -1.0f : yR;

		/****** let's hear the new sample *******/

		valueL = (uint16_t)((int16_t)((32767.0f) * yL)); // conversion float -> int
		valueR = (uint16_t)((int16_t)((32767.0f) * yR));

		*outp++ = valueL; // left channel sample
		*outp++ = valueR; // right channel sample
	}

}

/*===============================================================================================================*/

//void make_soundX(uint16_t *buf , uint16_t length) // To be used with the PitchGenerator
//{
//	uint16_t pos;
//	uint16_t *outp;
//	float_t y = 0, f0;
//	float_t yL, yR ;
//	uint16_t valueL, valueR;
//
//	outp = buf;
//
//	for (pos = 0; pos < length; pos++)
//	{
//		/* compute main frequency + vibrato modulation */
//		f0 = gen_SampleCompute(&pitchGen) * (1 +  Osc_WT_SINE_SampleCompute(&vibr_lfo));
//
//
//		/* If this is a new note... */
//		if ((pitchGen.stage != pitchGen.oldstage))
//		{
//			vol = frand_a_b(0.4f , .8f); // random volume for each note
//
//			if (pitchGen.transpose != 0)
//			{
//				pitchGen.rootNote += pitchGen.transpose ;
//				pitchGen_Transpose(&pitchGen, pitchGen.transpose);
//				pitchGen.transpose = 0;
//			}
//
//			if ( (pitchGen.someNotesMuted) && (rintf(frand_a_b(0.4f , 1)) == 0) )
//				ADSR_keyOff(&adsr);
//			else
//				ADSR_keyOn(&adsr);
//
//			if (pitchGen.glideON) pitchGen.step = rintf(frand_a_b(0 , 1)); else pitchGen.step = 0;
//
//			if (autoFilterON)
//				SVF_directSetFilterValue(&SVFilter, Ts * 600.f * powf(5000.f / 600.f, frand_a_b(0 , 1)));
//
//
//			/* If a new sequence begins ... */
//			if (pitchGen.stage == 0 && (pitchGen.automaticON || pitchGen.chRequest))
//			{
//				pitchGenChangePoints();
//				pitchGen.chRequest = false;
//				AdditiveGen_newWaveform();
//			}
//		}
//
//		pitchGen.oldstage =pitchGen.stage;
//
//		/*--- Generate waveform ---*/
//		y = waveCompute(sound, f0);
//
//		/*--- Apply envelop and tremolo ---*/
//		env = ADSR_computeSample(&adsr) * (1 + Osc_WT_SINE_SampleCompute(&amp_lfo));
//		y *= vol * env; // apply volume and envelop
//		if (adsr.cnt_ >= pitchGen.gateTime) ADSR_keyOff(&adsr);
//
//		/*--- Apply filter effect ---*/
//		/* Update the filters cutoff frequencies */
//		if ((! autoFilterON)&&(filt_lfo.amp != 0))
//			SVF_directSetFilterValue(&SVFilter, filterFreq * (1 + OpSampleCompute7bis(&filt_lfo)));
//
//		if (filt2_lfo.amp != 0)
//			SVF_directSetFilterValue(&SVFilter2, filterFreq2 * (1 + OpSampleCompute7bis(&filt2_lfo)));
//
//		y = 0.5f * (SVF_calcSample(&SVFilter, y) + SVF_calcSample(&SVFilter2, y)); // Two filters in parallel
//
//		/*---  Apply delay effect ----*/
//		if (delayON) 	y = Delay_compute(y);
//
//		/*--- Apply chorus/flanger effect ---*/
//		if (chorusON) stereoChorus_compute (&yL, &yR, y) ;
//		else yL = yR = y;
//
//		/*--- clipping ---*/
//		yL = (yL > 1.0f) ? 1.0f : yL; //clip too loud left samples
//		yL = (yL < -1.0f) ? -1.0f : yL;
//
//		yR = (yR > 1.0f) ? 1.0f : yR; //clip too loud right samples
//		yR = (yR < -1.0f) ? -1.0f : yR;
//
//		/****** let's hear the new sample *******/
//
//		valueL = (uint16_t)((int16_t)((32767.0f) * yL)); // conversion float -> int
//		valueR = (uint16_t)((int16_t)((32767.0f) * yR));
//
//		*outp++ = valueL; // left channel sample
//		*outp++ = valueR; // right channel sample
//	}
//}
