/*
    BSD 3-Clause License

    Copyright (c) 2018, KORG INC.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//*/

/*
 * File: square.cpp
 *
 * Naive square oscillator test
 *
 */

#include "userosc.h"

typedef struct State {
    float lfo, lfoz;
    uint8_t wave_type;
    uint8_t flags;
    uint8_t key;
    uint8_t extension;
    
    uint8_t notes[4];
    float w0[12]; //phase increment
    float phase[12]; //phase
    float detune;
} State;

const int extensions[12][4] = {
    {0, 7, 4, 11}, // I
    {-1, 6, 4, 10}, // I
    {0, 7, 3, 10}, // ii
    {-1, 6, 4, 9}, // ii
    {0, 7, 3, 10}, // iii
    {0, 7, 4, 11}, // IV
    {-1, 6, 5, 10}, // IV
    {0, 7, 4, 10}, // V
    {-1, 6, 4, 9}, // V
    {0, 7, 3, 10}, // vi
    {-1, 6, 5, 9}, // vi
    {0, 6, 3, 10} // vii^0
};

enum {
    k_flags_none = 0,
    k_flag_reset = 1<<0, //it's just 1
};

static State s_state; //Init a state variable

void OSC_INIT(uint32_t platform, uint32_t api)
{
    //Default values
    for (int i = 0; i < 12; i++) {
        s_state.w0[i] = 0.f;
        s_state.phase[i] = 0.f;
    }
    s_state.wave_type = 0.f;
    s_state.key = 0;
}

void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames)
{
    for (int i = 0; i < 4; i++) {
        s_state.notes[i] = extensions[(((params->pitch)>>8) + s_state.key) % 12][i];
    }
    // Reset flags
    const uint8_t flags = s_state.flags;
    s_state.flags = k_flags_none;

    float w0[12];
    float phase[12];
    switch (s_state.extension) {
        case 0:
            w0[0] = s_state.w0[0] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune);
            w0[1] = s_state.w0[1] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune * 0.8f);
            w0[2] = s_state.w0[2] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune * 0.6f);
            w0[3] = s_state.w0[3] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune * 0.4f);
            w0[4] = s_state.w0[4] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune * 0.2f);
            w0[5] = s_state.w0[5] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune * 0.1f);
            w0[6] = s_state.w0[6] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune * 0.1f);
            w0[7] = s_state.w0[7] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune * 0.2f);
            w0[8] = s_state.w0[8] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune * 0.4f);
            w0[9] = s_state.w0[9] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune * 0.6f);
            w0[10] = s_state.w0[10] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune * 0.8f);
            w0[11] = s_state.w0[11] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune);
            break;
        case 1:
            w0[0] = s_state.w0[0] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune);
            w0[1] = s_state.w0[1] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune * 0.6f);
            w0[2] = s_state.w0[2] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune * 0.3f);
            w0[3] = s_state.w0[3] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune * 0.3f);
            w0[4] = s_state.w0[4] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune * 0.6f);
            w0[5] = s_state.w0[5] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune);
            w0[6] = s_state.w0[6] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) + s_state.detune);
            w0[7] = s_state.w0[7] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) + s_state.detune * 0.6f);
            w0[8] = s_state.w0[8] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) + s_state.detune * 0.3f);
            w0[9] = s_state.w0[9] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) - s_state.detune * 0.3f);
            w0[10] = s_state.w0[10] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) - s_state.detune * 0.6f);
            w0[11] = s_state.w0[11] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) - s_state.detune);
            break;
        case 2:
            w0[0] = s_state.w0[0] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune);
            w0[1] = s_state.w0[1] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune * 0.5f);
            w0[2] = s_state.w0[2] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune * 0.5f);
            w0[3] = s_state.w0[3] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune);
            w0[4] = s_state.w0[4] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) + s_state.detune);
            w0[5] = s_state.w0[5] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) + s_state.detune * 0.5f);
            w0[6] = s_state.w0[6] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) - s_state.detune * 0.5f);
            w0[7] = s_state.w0[7] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) - s_state.detune);
            w0[8] = s_state.w0[8] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[2], (params->pitch & 0xFF) + s_state.detune);
            w0[9] = s_state.w0[9] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[2], (params->pitch & 0xFF) + s_state.detune * 0.5f);
            w0[10] = s_state.w0[10] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[2], (params->pitch & 0xFF) - s_state.detune * 0.5f);
            w0[11] = s_state.w0[11] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[2], (params->pitch & 0xFF) - s_state.detune);
            break;
        case 3:
        case 4:
            w0[0] = s_state.w0[0] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) + s_state.detune);
            w0[1] = s_state.w0[1] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF));
            w0[2] = s_state.w0[2] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[0], (params->pitch & 0xFF) - s_state.detune);
            w0[3] = s_state.w0[3] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) + s_state.detune);
            w0[4] = s_state.w0[4] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF));
            w0[5] = s_state.w0[5] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[1], (params->pitch & 0xFF) - s_state.detune);
            w0[6] = s_state.w0[6] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[2], (params->pitch & 0xFF) + s_state.detune);
            w0[7] = s_state.w0[7] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[2], (params->pitch & 0xFF));
            w0[8] = s_state.w0[8] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[2], (params->pitch & 0xFF) - s_state.detune);
            w0[9] = s_state.w0[9] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[3], (params->pitch & 0xFF) + s_state.detune);
            w0[10] = s_state.w0[10] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[3], (params->pitch & 0xFF));
            w0[11] = s_state.w0[11] = osc_w0f_for_note(((params->pitch)>>8) + s_state.notes[3], (params->pitch & 0xFF) - s_state.detune);
            break;
    }
    
    for (int i = 0; i < 12; i++) {
        phase[i] = (flags & k_flag_reset) ? 0.f : s_state.phase[i];
    }
    // Get lfo parameters (q31 is a fixed-point 31 bit)
    const float lfo = s_state.lfo = q31_to_f32(params->shape_lfo);
    // Reset lfo if flag is on, otherwise just get next lfo value
    float lfoz = (flags & k_flag_reset) ? lfo : s_state.lfoz;
    // LFO increment
    const float lfo_inc = (lfo - lfoz) / frames;

    // yn = pointer to first buffer position
    q31_t * __restrict y = (q31_t *)yn; // pointer to current buffer position
    const q31_t * y_e = y + frames; // pointer to end of buffer

    for (; y < y_e; ) { //Loop to fill buffer (why is it not a while?)
        float sig;
        switch (s_state.wave_type) {
            case 0:
                sig = osc_softclipf(0.05f,
                        (osc_sawf(phase[0]) +
                        osc_sawf(phase[1]) +
                        osc_sawf(phase[2]) +
                        osc_sawf(phase[3]) +
                        osc_sawf(phase[4]) +
                        osc_sawf(phase[5]) +
                        osc_sawf(phase[6]) +
                        osc_sawf(phase[7]) +
                        osc_sawf(phase[8]) +
                        osc_sawf(phase[9]) +
                        osc_sawf(phase[10]) +
                        osc_sawf(phase[11])) * 0.1f
                      );
                break;
            case 1:
                sig = osc_softclipf(0.05f,
                        (osc_sqrf(phase[0]) +
                        osc_sqrf(phase[1]) +
                        osc_sqrf(phase[2]) +
                        osc_sqrf(phase[3]) +
                        osc_sqrf(phase[4]) +
                        osc_sqrf(phase[5]) +
                        osc_sqrf(phase[6]) +
                        osc_sqrf(phase[7]) +
                        osc_sqrf(phase[8]) +
                        osc_sqrf(phase[9]) +
                        osc_sqrf(phase[10]) +
                        osc_sqrf(phase[11])) * 0.1f
                      );
                break;
            case 2:
            case 3:
                sig = osc_softclipf(0.05f,
                        (osc_sinf(phase[0]) +
                        osc_sinf(phase[1]) +
                        osc_sinf(phase[2]) +
                        osc_sinf(phase[3]) +
                        osc_sinf(phase[4]) +
                        osc_sinf(phase[5]) +
                        osc_sinf(phase[6]) +
                        osc_sinf(phase[7]) +
                        osc_sinf(phase[8]) +
                        osc_sinf(phase[9]) +
                        osc_sinf(phase[10]) +
                        osc_sinf(phase[11])) * 0.1f
                      );
                break;
        }
        *(y++) = f32_to_q31(sig);
        for (int i = 0; i < 12; i++) {
            phase[i] += w0[i];
            phase[i] -= (uint32_t)phase[i];
        }
        lfoz += lfo_inc;
    }
    for (int i = 0; i < 12; i++) {
        s_state.phase[i] = phase[i];
    }
    s_state.lfoz = lfoz;
}

void OSC_NOTEON(const user_osc_param_t * const params)
{
    
}

void OSC_NOTEOFF(const user_osc_param_t * const params)
{
    (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
    const float valf = param_val_to_f32(value);
    // Parameters are from 0 to 1
    switch (index) {
        case k_user_osc_param_id1: //Wave type
            s_state.wave_type = (uint8_t)(value / 100.f * 3.f);
            break;
        case k_user_osc_param_id2: //Detune
            s_state.detune = 255.f * valf;
            break;
        case k_user_osc_param_id3: //Attenuation
            break;
        case k_user_osc_param_id4:
            break;
        case k_user_osc_param_id5:
            break;
        case k_user_osc_param_id6:
            break;
        case k_user_osc_param_shape: //Key
            s_state.key = (uint8_t)(11.f * valf);
            break;
        case k_user_osc_param_shiftshape: //Extension
            s_state.extension = (uint8_t)(4.f * valf);
            break;
        default:
            break;
    }
}

