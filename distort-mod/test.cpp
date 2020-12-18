//
// Created by joshu on 2020-04-23.
//

#include "usermodfx.h"
#include "fx_api.h"

float dist_depth;
int dist_type;
float dpth;
float len;

float __fast_inline softclip(float in, float lim, float smooth) {
    float out = clipminmaxf(-lim, in, lim);
    out = out - smooth * (out*out*out);
    return out;
}

float __fast_inline hardclip(float x, float lim) {
    return clipminmaxf(-lim, x, lim);
}

float __fast_inline wrap(float x, float lim) {
    float out = x;
    while (out > lim || out < -lim) {
        if (out > lim) {
            out = -lim + (out-lim);
        } else if (out < -lim) {
            out = lim - (-out-lim);
        }
    }
    return out;
}

float __fast_inline fold(float x, float lim) {
    float out = x;
    while (out > lim || out < -lim) {
        if (out > lim) {
            out = lim - (out - lim);
        } else if (out < -lim) {
            out = -lim + (-out-lim);
        }
    }
    return out;
}

void MODFX_INIT(uint32_t platform, uint32_t api)
{
    dist_depth = 1.f;
    dist_type = 01.f;
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn,  float *sub_yn,
                   uint32_t frames)
{
    const float tempo = fx_get_bpmf();
    const float * mx = main_xn;
    float * __restrict my = main_yn;
    const float * my_e = my + 2*frames;

    const float *sx = sub_xn;
    float * __restrict sy = sub_yn;
    float base_main;
    float base_sub;
    for (; my < my_e;) {
        base_main = *(mx++) * ((dist_depth * 10.0f) + 1.f);
        base_sub = *(sx++) * ((dist_depth * 10.0f) + 1.f);
        switch(dist_type) {
            case 0:
                *(my++) = softclip(base_main, 0.15f, 0.15f);
                *(sy++) = softclip(base_sub, 0.15f, 0.15f);
                break;
            case 1:
                *(my++) = hardclip(base_main, 0.15f);
                *(sy++) = hardclip(base_sub, 0.15f);
                break;
            case 2:
                *(my++) = wrap(base_main, 0.15f);
                *(sy++) = wrap(base_sub, 0.15f);
                break;
            case 3:
                *(my++) = fold(base_main, 0.15f);
                *(sy++) = fold(base_sub, 0.15f);
                break;
        }

    }
}

void MODFX_PARAM(uint8_t index, int32_t value)
    {
        const float valf = q31_to_f32(value);
        switch (index) {
            case k_user_modfx_param_time:
                if (valf < 0.25) {
                    dist_type = 0; //Soft
                } else if (valf < 0.5) {
                    dist_type = 1; //Hard
                } else if (valf < 0.75) {
                    dist_type = 2; //Wrap
                } else {
                    dist_type = 3; //..
                }
                break;
            case k_user_modfx_param_depth:
                dist_depth = valf;
                break;
            default:
                break;
        }
    }
