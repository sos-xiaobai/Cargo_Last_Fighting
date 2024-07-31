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

#include "config.h"
#include "iwdg.h"
#include "drv_uart.h"
#include "drv_tim.h"
#include "dvc_imu.h"
#include "alg_fsm.h"
#include "crt_chassis.h"
#include "dvc_ER08.h"
#include "dvc_SIM900A.h"
#include "dvc_Tjc011.h"
#include "dvc_cargo.h"
#include "dvc.servo.h"
#include "dvc_WS2812.h"
#include "dvc_miniPC.h"
#include "alg_fifo.h"
#include "alg_astart.h"

/* Exported macros -----------------------------------------------------------*/
class Class_Chariot;
/* Exported types ------------------------------------------------------------*/

#define INPUT (HAL_GPIO_ReadPin(INPUT_GPIO_Port, INPUT_Pin) == GPIO_PIN_RESET)
#define OUTPUT (HAL_GPIO_ReadPin(INPUT_GPIO_Port, INPUT_Pin) == GPIO_PIN_SET)

#define K210_SHEET  90
#define K210_INIT  -90

#ifdef NEW_CAR

    #define Init_Position_X 1.0
    #define Init_Position_Y 0.0       

    #define CLOSED  10
    #define OPEN   -50

#elif defined(OLD_CAR)

    #define Init_Position_X 0.0
    #define Init_Position_Y 0.0  

    #define CLOSED 30
    #define OPEN   0
 
#endif

enum Enum_Chariot_Status :uint8_t
{
    Chariot_Free_Status = 1,
    Chariot_Busy_Status,
};


enum Enum_FSM_Chariot_Status
{
    FSM_Chariot_Waiting_Status = 0,  //等待状态
    FSM_Chariot_Forward_Status,  //导航前往状态
    FSM_Chariot_Input_OR_Output_Cargo_Status, //入库或出库状态
    FSM_Chariot_Back_Status, //导航返回状态
    FSM_Chariot_Back_Judge_Status, //判断返回类型状态
};


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

    Class_Chassis Chassis;  //底盘
    Class_MiniPC MiniPC; //k210
    Class_Astart Astart; //A*算法
    CircularFifo Input_Cargo_FIFO; //待入库货物FIFO
    CircularFifo Output_Cargo_FIFO; //待出库货物FIFO
    Class_Cargo_List Cargo_List; //已经入库的货物链表
    Struct_Cargo Now_Cargo; //当前货物
    uint8_t Now_Cargo_Number; //当前入库货物数量
    Class_Servo Servo[6]; //5轴舵机自由度 + k210
    Enum_Chariot_Control_Status Control_Status = Chariot_Disable_Status; //控制状态
    friend class Class_FSM_Chariot_Control; //有限自动机

    bool Input_Cargo_Flag = false; //入库标志

    //构造函数
    void Init(UART_HandleTypeDef *huart);

    bool Servo_Caculate(float x, float y, float angle);
    uint8_t Get_Cargo_Data();
    uint8_t Jundge_Cargo();
    void Output_Cargo();

    bool Burry_Output_Cargo_1(uint16_t __time_cnt);
    bool Burry_Output_Cargo_2(uint16_t __time_cnt);
    bool Burry_Input_Cargo_1(uint16_t __time_cnt);
    bool Burry_Input_Cargo_2(uint16_t __time_cnt);
    bool Midlle_Position(uint16_t __time_cnt);
    void Scan_Burry(uint16_t __time_cnt);
    void Init_Position();

    void Set_Control_Status(Enum_Chariot_Control_Status status);
    Enum_Chariot_Control_Status Get_Control_Status();

    inline void Set_Chariot_Status(Enum_Chariot_Status status) { Chariot_Status = status; }
    inline Enum_Chariot_Status Get_Chariot_Status() { return (Chariot_Status); }

    inline void Set_Another_Chariot_Status(Enum_Chariot_Status status) { Another_Chariot_Status = status; }
    inline Enum_Chariot_Status Get_Another_Chariot_Status() { return (Another_Chariot_Status); }


    void UART_Transmit_Status();
    void UART_Transmit_Output_Cargo_Information();
    void UART_Transmit_Input_Cargo_Information();

    void TIM_Calculate_PeriodElapsedCallback();
    void TIM1msMod50_Alive_PeriodElapsedCallback();

protected:
    Struct_UART_Manage_Object *UART_Manage_Object;
    Enum_Chariot_Status Chariot_Status = Chariot_Free_Status; //小车状态
    Enum_Chariot_Status Another_Chariot_Status = Chariot_Free_Status; //对面小车状态
};

/* Exported variables --------------------------------------------------------*/

/* Exported function declarations --------------------------------------------*/
#endif

/************************ COPYRIGHT(C) USTC-ROBOWALKER **************************/
