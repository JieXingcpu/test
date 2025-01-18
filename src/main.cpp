/*
 * @version: no version
 * @LastEditors: xingjie
 * @Description: 
 * @Author: qingmeijiupiao
 * @LastEditTime: 2024-12-21 19:08:27
 */
#include <Arduino.h>
#include "HXC_TWAI.hpp"
#include "HXC_CAN.hpp"
#include "HXCthread.hpp"
#include "LED.hpp"


LED led(4);         //LED指示灯引脚,默认4
HXC_TWAI CAN_BUS(6,7);      //CAN总线引脚,默认6,7
HXC_CAN_message_t receive_msg;          //接收数据线程接收到的消息所使用的全局变量



//收发线程
HXC::thread<void> send_and_receive([]{        

    HXC_CAN_message_t send_msg;
    send_msg.identifier=0x123;          //CAN_ID
    send_msg.data_length_code=8;        //数据长度
     while(1){

        /*接收其他接口的传感器数据*/

        float sensor_data1=1.551125;//传感器数据1

        float sensor_data2=2.551125;//传感器数据2

        if(false){
            led.change_state(DeviceOutline);//设备离线
        }

        /*接收其他接口的传感器数据*/

        memcpy(send_msg.data,&sensor_data1,4);//将传感器数据1复制到发送消息对象中

        memcpy(send_msg.data+4,&sensor_data2,4);//将传感器数据2复制到发送消息对象中

        auto err=CAN_BUS.send(&send_msg);//调用API发送CAN消息，同时设定err来标记发送状态
        if(err!=ESP_OK){
            led.change_state(CanOutline);//CAN离线
        }
        delay(1);       //延时控制发送频率
        
        if(false){
            led.change_state(Other);//其他
        }

            

    }
    
});

//以下为异步收发流程所使用的线程对象

//接收数据线程
HXC::thread<void> receive([]{        

    receive_msg.identifier=0x123;          //CAN_ID
    receive_msg.data_length_code=8;        //数据长度
    while(1){

        /*这里的两种数据为给定的，当前还未添加接收数据函数*/

        float sensor_data1=1.551125;//传感器数据1

        float sensor_data2=2.551125;//传感器数据2



        if(false){
            led.change_state(DeviceOutline);//设备离线
        }
        memcpy(receive_msg.data,&sensor_data1,4);//将传感器数据1复制到接收消息的全局变量中

        memcpy(receive_msg.data+4,&sensor_data2,4);//将传感器数据2制到接收消息的全局变量中

    }
});

//CAN消息数据发送线程
HXC::thread<void> send([]{        
    HXC_CAN_message_t send_msg_t;
    send_msg_t=receive_msg;       //将接收消息的全局变量赋值给发送消息对象，便于分辨，同时区分于收发线程
     auto err=CAN_BUS.send(&send_msg_t);//调用API发送CAN消息，同时设定err来标记发送状态
        if(err!=ESP_OK){
            led.change_state(CanOutline);//CAN离线
        }
        delay(1);       //延时控制发送频率
        
        if(false){
            led.change_state(Other);//其他
        }
    
    
});


void setup() {


    /*同步收发流程*/
    led.setup();
    CAN_BUS.setup();         //初始化CAN1
    send_and_receive.start("send_and_receive");     //启动收发线程
    

    /*异步收发线程*/
    led.setup();
    CAN_BUS.setup();         //初始化CAN1
    receive.start("receive");     //启动接收线程
    send.start("send");     //启动发送线程

}

void loop() {}

