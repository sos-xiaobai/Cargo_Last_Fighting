#ifndef __DVC_MINIPC_H__
#define __DVC_MINIPC_H__


#include "alg_pid.h"
#include "main.h"
#include "drv_uart.h"
#include "alg_pid.h"

enum Enum_MiniPC_Status
{
    MiniPC_Disable_Status = 0,
    MiniPC_Enable_Status,
};

enum Enum_Sheet_Numbers : uint8_t
{
    Sheet_Number_One = 0,
    Sheet_Number_Two,
    Sheet_Number_Three,
    Sheet_Number_Four,
};

struct Struct_MiniPC_UART_Data
{
    uint8_t head;
    uint8_t sheet;
    int16_t x;
    int16_t y;
    uint8_t tail;
}__attribute__((packed));



class Class_MiniPC
{
    public:

    Class_PID PID_X;
    Class_PID PID_Y;

    void Init(UART_HandleTypeDef *huart,float __target_x,float __target_y);
    void UART_RxCpltCallback(uint8_t *Rx_Data);
    void TIM_Calculate_PeriodElapsedCallback();
    void TIM_50ms_Alive_PeriodElapsedCallback();

    inline float Get_now_x(void){return (now_x);}
    inline float Get_now_y(void){return (now_y);}
    inline float Get_target_x(void){return (target_x);}
    inline float Get_target_y(void){return (target_y);}
    inline float Get_target_x_speed(void){return (target_x_speed);}
    inline float Get_target_y_speed(void){return (target_y_speed);}
    inline Enum_MiniPC_Status Get_MiniPC_Status(void){return (MiniPC_Status);}

    inline void Set_target_x(float __target_x){target_x = __target_x;}
    inline void Set_target_y(float __target_y){target_y = __target_y;}
    inline void Set_target_x_speed(float __target_x_speed){target_x_speed = __target_x_speed;}
    inline void Set_target_y_speed(float __target_y_speed){target_y_speed = __target_y_speed;}

    protected:

    Struct_UART_Manage_Object *UART_Manage_Object;

    Struct_MiniPC_UART_Data Now_UART_Rx_Data;
    Struct_MiniPC_UART_Data Pre_UART_Rx_Data;

    float target_x;
    float target_y;
    
    float now_x;
    float now_y;
    Enum_Sheet_Numbers sheet;

    float target_x_speed;
    float target_y_speed;

    Enum_MiniPC_Status MiniPC_Status = MiniPC_Disable_Status;

    uint16_t flag;
    uint16_t pre_flag;

    void Data_Process();

};


#endif

