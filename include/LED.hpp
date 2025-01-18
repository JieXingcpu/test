#ifndef LED_HPP
#define LED_HPP
#include <HXCthread.hpp>



uint8_t pwmChannel=0;                     //PWM通道,默认0
uint32_t pwmFreq=1000;                    //PWM频率
uint8_t pwmResolution=8;                  //PWM分辨率

void LED_PWM_Init(int pin){                 //初始化PWM函数：初始化指示LED引脚的PWM输出
    ledcSetup(pwmChannel,pwmFreq,pwmResolution);        //初始化PWM通道
    ledcAttachPin(pin,pwmChannel);          //将PWM通道绑定到指示LED引脚
}


enum LED_state:uint8_t{                 //指示用LED的各种状态枚举
    Normal,                             //正常工作情况
    CanOutline,                         //CAN离线情况
    DeviceOutline,                      //设备离线情况
    Other                               //其他异常情况
};

class LED{                 
    public:
    LED(uint8_t _pin):pin(_pin){}

    void setup(){
        pinMode(pin,OUTPUT);
        digitalWrite(pin,LOW);
        now_state_thread=&LED_Normal;
        now_state_thread->start(pin);
    }

    void change_state(LED_state state){
        now_state_thread->stop();
        switch (state){
            case Normal:
                now_state_thread=&LED_Normal;
                break;
            case CanOutline:
                now_state_thread=&LED_CanOutline;
                break;
            case DeviceOutline:
                now_state_thread=&LED_DeviceOutline;
                break;
            case Other:
                now_state_thread=&LED_Other;
                break;
        }
        now_state_thread->start(pin);
        return;
    }   
    protected:
    //LED各种状态线程

    HXC::thread<uint8_t> LED_Normal=HXC::thread<uint8_t>(               //正常工作状态,指示用LED呈现呼吸灯效果
        [](uint8_t LED_pin){
            while (1){
                /*LED状态循环*/
                for(uint8_t i=0;i<150;i++){
                    ledcWrite(pwmChannel,i);
                    delay(10);
                }
            }
        }
    );

    HXC::thread<uint8_t> LED_CanOutline=HXC::thread<uint8_t>(               //CAN离线状态,指示LED呈现较亮一次快闪效果
        [](uint8_t LED_pin){
            while (1){
                /*LED状态循环*/
                ledcWrite(pwmChannel,220);
                delay(500);
                ledcWrite(pwmChannel,0);
                delay(500);
            }
        }
    );

    HXC::thread<uint8_t> LED_DeviceOutline=HXC::thread<uint8_t>(                //设备离线状态,指示LED呈现常高亮
        [](uint8_t LED_pin){
            while (1){
                /*LED状态循环*/
                ledcWrite(pwmChannel,220);
            }
        }
    );

    HXC::thread<uint8_t> LED_Other=HXC::thread<uint8_t>(                    //其他异常状态,指示LED呈现较亮二次快闪效果
        [](uint8_t LED_pin){
            while (1){
                /*LED状态循环*/
                ledcWrite(pwmChannel,220);
                delay(100);
                ledcWrite(pwmChannel,0);
                delay(100);
                ledcWrite(pwmChannel,220);
                delay(100);
                ledcWrite(pwmChannel,0);
                delay(1000);
            }
        }
    );
    uint8_t pin;
    HXC::thread<uint8_t>* now_state_thread;
    
};






#endif