/*
 * midi.h
 *
 *  Created on: Sep 20, 2019
 *      Author: browt
 */

#ifndef MIDI_H_
#define MIDI_H_

// Summary of MIDI Messages linked here:
// https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message

// System Real-Time Messages
#define TIMING_CLOCK 0xF8
#define ACTIVE_SENSING 0xFE
#define START 0xFA
#define CONTINUE 0xFB
#define RESET 0xFF

// System Common Messages
#define SYSTEM_EXCLUSIVE 0xF0
#define MIDI_TIME_CODE_QUARTER_FRAME 0xF1
#define SONG_POSITION_POINTER 0xF2
#define SONG_SELECT 0xF3
#define TUNE_REQUEST 0xF6
#define END_OF_EXCLUSIVE 0xF7

// Channel Mode Messages
#define CHANNEL_MODE_MESSAGE 0xB0
// Control Numbers
#define ALL_SOUND_OFF 120
#define RESET_ALL_CONTROLLERS 121
#define LOCAL_CONTROL 122
#define ALL_NOTES_OFF 123
#define OMNI_MODE_OFF 124
#define OMNI_MODE_ON 125
#define MONO_MODE_ON 126
#define POLY_MODE_ON 127

// Channel Voice Messages
#define NOTE_OFF_EVENT 0x80
#define NOTE_ON_EVENT 0x90
#define POLY_KEY_PRESSURE 0xA0
#define CONTROL_CHANGE 0xB0
#define PROGRAM_CHANGE 0xC0
#define CHANNEL_PRESSURE 0xD0
#define PITCH_BEND_CHANGE 0xE0

#endif /* MIDI_H_ */
