/**
 * @file ita_chariot.h
 * @author yssickjgd (yssickjgd@mail.ustc.edu.cn)
 * @brief 人机交互控制逻辑
 * @version 0.1
 * @date 2023-08-29 0.1 23赛季定稿
 *
 * @copyright USTC-RoboWalker (c) 2022
 *
 */

#ifndef TSK_INTERACTION_H
#define TSK_INTERACTION_H

/* Includes ------------------------------------------------------------------*/

#include "drv_uart.h"
#include "drv_tim.h"
#include "alg_fsm.h"
#include "dvc_ER08.h"
#include "dvc_SIM900A.h"
#include "dvc_Tjc011.h"
#include "dvc_cargo.h"
#include "dvc_WS2812.h"
#include "stdio.h"
/* Exported macros -----------------------------------------------------------*/
class Class_Chariot;
/* Exported types ------------------------------------------------------------*/

/**
 * @brief 状态枚举
 *
 */
enum Enum_Chariot_Control_Status 
{
    Chariot_Disable_Status = 0,
    Chariot_Input_Cargo_Status, 
    Chariot_Output_Cargo_Status,
};



/**
 * @brief 机器人控制有限自动机
 *
 */
class Class_FSM_Chariot_Control : public Class_FSM
{
public:
    Class_Chariot *Chariot;

    void Reload_TIM_Status_PeriodElapsedCallback();


};

/**
 * @brief 控制对象
 *
 */
class Class_Chariot
{
public:

    Class_ER08 ER08; //扫码模块
    Class_SIM900A SIM900A; //通信模块
    Class_Tjc011 Tjc011; //液晶屏模块
    Class_Cargo_List Cargo_List; //货物链表
    Struct_Cargo Now_Cargo; //当前货物
    uint8_t Now_Cargo_Number; //当前入库货物数量
    Enum_Chariot_Control_Status Control_Status; //控制状态
    friend class Class_FSM_Chariot_Control; //有限自动机

    bool Sim_Send_Flag;
    bool Last_Sim_Send_Flag;

    //构造函数
    void Init();

    uint8_t Get_Cargo_Data();
    uint8_t Jundge_Cargo();
    void Output_Cargo();

    void UART_Send_Add_Cargo();
    void UART_Send_Delete_Cargo();

    inline void Set_Control_Status(Enum_Chariot_Control_Status status);
    inline Enum_Chariot_Control_Status Get_Control_Status();

    void TIM_Calculate_PeriodElapsedCallback();
    void TIM1msMod50_Alive_PeriodElapsedCallback();

protected:
    Struct_UART_Manage_Object *UART_Manage_Object; //UART管理对象

};

/* Exported variables --------------------------------------------------------*/

/* Exported function declarations --------------------------------------------*/


void Class_Chariot::Set_Control_Status(Enum_Chariot_Control_Status status)
{
    Control_Status = status;
}


Enum_Chariot_Control_Status Class_Chariot::Get_Control_Status()
{
    return (Control_Status);
}

#endif

/************************ COPYRIGHT(C) USTC-ROBOWALKER **************************/
