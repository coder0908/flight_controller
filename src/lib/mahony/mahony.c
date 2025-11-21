/*
 * mahony.c
 *
 *  Created on: Nov 20, 2025
 *      Author: unRR
 */


#include "mahony.h"
#include "vmd.h"
#include "stm32f4xx_hal.h"

#include "mahony.h"
#include "vmd.h"
#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

bool mahony_init(struct mahony_ahrs *mah, float kp, float ki)
{
    VMD_ASSERT_PARAM(mah);

    mah->q0 = 1.0f;
    mah->q1 = 0.0f;
    mah->q2 = 0.0f;
    mah->q3 = 0.0f;

    mah->x_gyro_bias_radps = 0.0f;
    mah->y_gyro_bias_radps = 0.0f;
    mah->z_gyro_bias_radps = 0.0f;

    mah->kp = kp;
    mah->ki = ki;

    mah->prev_time = HAL_GetTick(); // <-- 반드시 초기화

    return true;
}

bool mahony_calc(struct mahony_ahrs *mah,
        float x_gyro_radps, float y_gyro_radps, float z_gyro_radps,
        float x_accel_g, float y_accel_g, float z_accel_g,
        float x_mag, float y_mag, float z_mag)
{
    VMD_ASSERT_PARAM(mah);

    uint32_t current_time = HAL_GetTick();
    /* dt in seconds. prev_time initialized in mahony_init */
    float dt = (current_time - mah->prev_time) / 1000.0f;
    if (dt <= 0.0f) dt = 1e-3f; // 보호: 0 이거나 음수이면 아주 작은 dt 사용

    /* cache quaternion (original values) */
    float q0 = mah->q0, q1 = mah->q1, q2 = mah->q2, q3 = mah->q3;

    /* 1. sensor preprocessing */
    float gx = x_gyro_radps - mah->x_gyro_bias_radps;
    float gy = y_gyro_radps - mah->y_gyro_bias_radps;
    float gz = z_gyro_radps - mah->z_gyro_bias_radps;

    /* normalize accelerometer */
    float recipNorm = x_accel_g * x_accel_g + y_accel_g * y_accel_g + z_accel_g * z_accel_g;
    if (recipNorm <= 0.0f) return false; /* can't normalize */
    recipNorm = 1.0f / sqrtf(recipNorm);
    x_accel_g *= recipNorm;
    y_accel_g *= recipNorm;
    z_accel_g *= recipNorm;

    /* normalize magnetometer */
    recipNorm = x_mag * x_mag + y_mag * y_mag + z_mag * z_mag;
    if (recipNorm <= 0.0f) {
        /* no valid mag -> fallback to IMU-only Mahony (skip mag terms) */
        x_mag = y_mag = z_mag = 0.0f;
    } else {
        recipNorm = 1.0f / sqrtf(recipNorm);
        x_mag *= recipNorm;
        y_mag *= recipNorm;
        z_mag *= recipNorm;
    }

    /* 2. estimated direction of gravity and magnetic field */
    float halfvx = q1 * q3 - q0 * q2;
    float halfvy = q0 * q1 + q2 * q3;
    float halfvz = q0 * q0 - 0.5f + q3 * q3;

    /* magnetic field in earth frame (h), then b = [bx, 0, bz] */
    float hx = 2.0f * (x_mag * (0.5f - q2*q2 - q3*q3) + y_mag * (q1*q2 - q0*q3) + z_mag * (q1*q3 + q0*q2));
    float hy = 2.0f * (x_mag * (q1*q2 + q0*q3) + y_mag * (0.5f - q1*q1 - q3*q3) + z_mag * (q2*q3 - q0*q1));
    float bx = sqrtf(hx * hx + hy * hy);
    float bz = 2.0f * (x_mag * (q1*q3 - q0*q2) + y_mag * (q2*q3 + q0*q1) + z_mag * (0.5f - q1*q1 - q2*q2));

    /* projected magnetic vector (half w) */
    float halfwx = bx * (q1 * q3 - q0 * q2) + bz * (q0 * q1 + q2 * q3); /* alternative forms exist */
    float halfwy = bx * (q0 * q1 + q2 * q3) + bz * (q1 * q2 - q0 * q3);
    float halfwz = bx * (0.5f - q1*q1 - q2*q2) + bz * (q1 * q3 + q0 * q2);

    /* 3. error = (a x v) + (m x w) */
    float halfex = (y_accel_g * halfvz - z_accel_g * halfvy) + (y_mag * halfwz - z_mag * halfwy);
    float halfey = (z_accel_g * halfvx - x_accel_g * halfvz) + (z_mag * halfwx - x_mag * halfwz);
    float halfez = (x_accel_g * halfvy - y_accel_g * halfvx) + (x_mag * halfwy - y_mag * halfwx);

    /* 3-2. integral (bias) */
    if (mah->ki > 0.0f) {
        mah->x_gyro_bias_radps += mah->ki * halfex * dt;
        mah->y_gyro_bias_radps += mah->ki * halfey * dt;
        mah->z_gyro_bias_radps += mah->ki * halfez * dt;
    }

    /* 3-3. apply proportional correction */
    gx += mah->kp * halfex;
    gy += mah->kp * halfey;
    gz += mah->kp * halfez;

    /* 4. quaternion integration:
       qDot = 0.5 * q * omega
       Use original q0..q3 for computation, update all at once. */
    float half_dt = 0.5f * dt;
    float rx = gx * half_dt;
    float ry = gy * half_dt;
    float rz = gz * half_dt;

    /* compute new quaternion using original q's */
    float q0_new = q0 + (-q1 * rx - q2 * ry - q3 * rz);
    float q1_new = q1 + ( q0 * rx + q2 * rz - q3 * ry);
    float q2_new = q2 + ( q0 * ry - q1 * rz + q3 * rx);
    float q3_new = q3 + ( q0 * rz + q1 * ry - q2 * rx);

    /* normalize */
    recipNorm = q0_new * q0_new + q1_new * q1_new + q2_new * q2_new + q3_new * q3_new;
    if (recipNorm <= 0.0f) return false;
    recipNorm = 1.0f / sqrtf(recipNorm);

    mah->q0 = q0_new * recipNorm;
    mah->q1 = q1_new * recipNorm;
    mah->q2 = q2_new * recipNorm;
    mah->q3 = q3_new * recipNorm;

    mah->prev_time = current_time;
    return true;
}

bool mahony_convert_euler_angle(const struct mahony_ahrs *mah, float *x_angle_deg, float *y_angle_deg, float *z_angle_deg)
{
    VMD_ASSERT_PARAM(mah);
    VMD_ASSERT_PARAM(x_angle_deg);
    VMD_ASSERT_PARAM(y_angle_deg);
    VMD_ASSERT_PARAM(z_angle_deg);

    float q0 = mah->q0, q1 = mah->q1, q2 = mah->q2, q3 = mah->q3;
    const float RAD_TO_DEG = 57.29577951308232f;

    /* roll (x) */
    float sinr_cosp = 2.0f * (q0 * q1 + q2 * q3);
    float cosr_cosp = 1.0f - 2.0f * (q1 * q1 + q2 * q2);
    *x_angle_deg = atan2f(sinr_cosp, cosr_cosp) * RAD_TO_DEG;

    /* pitch (y) */
    float sinp = 2.0f * (q0 * q2 - q3 * q1);
    if (fabsf(sinp) >= 1.0f)
        *y_angle_deg = copysignf(M_PI_2, sinp) * RAD_TO_DEG;
    else
        *y_angle_deg = asinf(sinp) * RAD_TO_DEG;

    /* yaw (z) */
    float siny_cosp = 2.0f * (q0 * q3 + q1 * q2);
    float cosy_cosp = 1.0f - 2.0f * (q2 * q2 + q3 * q3);
    *z_angle_deg = atan2f(siny_cosp, cosy_cosp) * RAD_TO_DEG;

    /* normalize yaw to 0..360 if desired */
    if (*z_angle_deg < 0.0f) *z_angle_deg += 360.0f;

    return true;
}
