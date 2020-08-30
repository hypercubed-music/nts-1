//
// Created by joshu on 2020-04-23.
//

#include "userosc.h"
//#include "test.h"

typedef struct State {
    float w0; //current delta phase for update
    float w_target;
    float w_init;
    float pitch_decay;
    float hold_time;
    float phase;
    float dist;
    float drive;
    float attack_pitch;
    float lfo, lfoz; //current lfo value (and depth?)
    uint8_t flags;
};

static State s_state;

enum {
    k_flags_none = 0,
    k_flag_reset = 1<<0,
};

void OSC_INIT(uint32_t platform, uint32_t api) {
    (void)platform;
    (void)api;
    s_state.w_target = 0.f; //target phase delta
    s_state.w_init   = 0.f;
    s_state.w0       = 0.f; //phase delta
    s_state.phase    = 0.f; //phase
    s_state.pitch_decay = 0.f; //pitch decay time
    s_state.hold_time = 0.f;
    s_state.dist     = 0.f;
    s_state.drive    = 0.f;
    s_state.attack_pitch = 0.f;
    //stores time held: 0-1 -> just pressed-decayed
}

// params: oscillator parameter
// yn: write address
// frames: requested frame count for write
void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames) {

    // Store current flag then reset
    const uint8_t flags = s_state.flags;
    s_state.flags = k_flags_none;

    const float attack_pitch = s_state.attack_pitch;
    // Get the target phase delta (where we want to end up)
    const float w_target = s_state.w_target = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF) / 2.f;
    const float w_init = s_state.w_init = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF) * attack_pitch;
    const float pitch_decay = s_state.pitch_decay;
    float hold_time = s_state.hold_time;

    const float w0 = s_state.w0 = linintf(hold_time,w_init,w_target);
    float phase = (flags & k_flag_reset) ? 0.f : s_state.phase;

    // phase distortion
    const float dist  = s_state.dist;
    const float drive = s_state.drive;

    // lfo stuf
    const float lfo = s_state.lfo = q31_to_f32(params->shape_lfo);
    float lfoz = (flags & k_flag_reset) ? lfo : s_state.lfoz;
    const float lfo_inc = (lfo - lfoz) / frames;

    q31_t * __restrict y = (q31_t *)yn; // pointer to current buffer position
    const q31_t * y_e = y + frames; // pointer to end of buffer
    for (; y != y_e; ) { // Time to fill the buffer!

        // Phase distortion
        float p = phase + linintf(dist, 0.f, dist * osc_sinf(phase));
        p = (p <= 0) ? 1.f - p : p - (uint32_t)p;

        const float sig = osc_softclipf(0.05f,drive * osc_sinf(p));
        *(y++) = f32_to_q31(sig);

        phase += w0;
        phase -= (uint32_t)phase;
        hold_time += k_samplerate_recipf * (20.0f - (pitch_decay*20.0f));
        hold_time = clip1f(hold_time);

        lfoz += lfo_inc;
    }
    s_state.hold_time = hold_time;
    s_state.phase = phase;
    s_state.lfoz = lfoz;
}

void OSC_NOTEON(const user_osc_param_t * const params) {
    //Reset the flag
    s_state.flags |= k_flag_reset;
    s_state.hold_time = 0.f;
}

void OSC_NOTEOFF(const user_osc_param_t * const params) {
    (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
    const float valf = param_val_to_f32(value);

    switch (index) {
        case k_user_osc_param_id1:
            s_state.drive = 1.f + valf;
            break;
        case k_user_osc_param_id2:
            s_state.attack_pitch = 1.f + (valf * 24.f);
            break;
        case k_user_osc_param_id3:
        case k_user_osc_param_id4:
        case k_user_osc_param_id5:
        case k_user_osc_param_id6:
            break;
        case k_user_osc_param_shape:
            s_state.pitch_decay = valf;
            break;
        case k_user_osc_param_shiftshape:
            s_state.dist = 0.7f * valf;
            break;
        default:
            break;
    }
}