#include "led.h"
//0£º¹ØµÆ£¬1£º¿ªµÆ£¬2pwmºôÎüµÆ
void led_control(uint8_t state){
    static uint16_t pwm_value = 0;
    static uint8_t direction = 0; // 0:Ôö¼Ó£¬1:¼õÉÙ
    
    if(state==1) {
        TIM1->CCR1 = 1000;
    }
    else if(state==0) {
        TIM1->CCR1 = 0;
    }
    else if(state==2) {
        // ºôÎüµÆÂß¼­
        if(direction == 0) {
            pwm_value+=10;
            if(pwm_value >= 1000) {
                direction = 1;
            }
        } else {
            pwm_value-=10;
            if(pwm_value == 0) {
                direction = 0;
            }
        }
        TIM1->CCR1 = pwm_value;
    }
}

