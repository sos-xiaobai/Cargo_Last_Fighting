/**
 * @file tsk_config_and_callback.cpp
 * @author yssickjgd (yssickjgd@mail.ustc.edu.cn)
 * @brief 临时任务调度测试用函数, 后续用来存放个人定义的回调函数以及若干任务
 * @version 0.1
 * @date 2023-08-29 0.1 23赛季定稿
 *
 * @copyright USTC-RoboWalker (c) 2022
 *
 */

/**
 * @brief TIM开头的默认任务均1ms, 特殊任务需额外标记时间
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "tsk_config_and_callback.h"
#include "tim.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

uint16_t Motor_Now_Encoder[4];
uint16_t Motor_Direction[4];
uint16_t Motor_Pre_Encoder[4];

//uint8_t buffer1[19];
uint8_t buffer2[4];

Class_Chariot Chariot;

//机器人控制对象
Class_FSM_Chariot_Control FSM_Chariot;

/* Private function declarations ---------------------------------------------*/

void TIM2_Task1ms_PeriodElapsedCallback();

/* Function prototypes -------------------------------------------------------*/
void ER08_UART3_Callback(uint8_t *Buffer, uint16_t Length)
{
    FSM_Chariot.Chariot->ER08.UART_RxCpltCallback(Buffer);
}

void Tjc011_UART2_Callback(uint8_t *Buffer, uint16_t Length)
{
    FSM_Chariot.Chariot->Tjc011.UART_RxCpltCallback(Buffer);
}

void SIM_UART5_Callback(uint8_t *Buffer, uint16_t Length)
{
//    FSM_Chariot.Chariot->SIM900A.UART_RxCpltCallback(Buffer);
}

/**
 * @brief 初始化任务
 *
 */
void Task_Init()
{  
    //定时器循环任务
    TIM_Init(&htim2, TIM2_Task1ms_PeriodElapsedCallback);

    UART_Init(&huart2, Tjc011_UART2_Callback, 20);
    UART_Init(&huart3, ER08_UART3_Callback, 23);
    UART_Init(&huart5, NULL, 0); //不启huart5的回调函数
    // HAL_UART_Receive_IT(&huart3, UART3_Manage_Object.Rx_Buffer, 19);

    /********************************* 设备层初始化 *********************************/

     //设备层集成在交互层初始化中，没有显视地初始化

    /********************************* 交互层初始化 *********************************/

    FSM_Chariot.Chariot = &Chariot;
    FSM_Chariot.Init(4,0);
    FSM_Chariot.Chariot->Init();

    /********************************* 使能调度时钟 *********************************/

    //启动定时器
   HAL_TIM_Base_Start_IT(&htim2);
   HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
   

}


/**
 * @brief TIM6任务回调函数
 *
 */
void TIM2_Task1ms_PeriodElapsedCallback()
{	
    FSM_Chariot.Reload_TIM_Status_PeriodElapsedCallback();
}




/**
 * @brief 前台循环任务
 *
 */
void Task_Loop()
{

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==Sim_Wake_Pin)
    {

        if(HAL_GPIO_ReadPin(Sim_Wake_GPIO_Port, Sim_Wake_Pin)==GPIO_PIN_RESET)
        {
            if(FSM_Chariot.Chariot->Sim_Send_Flag==0)
            {
                FSM_Chariot.Chariot->Sim_Send_Flag=1;
            }
            else
            {
                FSM_Chariot.Chariot->Sim_Send_Flag=0;
            }
        }
    }
}


// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART3)
//     {
// 		// 处理USART1接收到的数据
// 	    ER08_UART3_Callback(UART3_Manage_Object.Rx_Buffer, 19);
//         HAL_UART_Receive_IT(&huart3, UART3_Manage_Object.Rx_Buffer, 19);
//     }
// }

void SPI1_IMU_Task_Callback(uint8_t* Tx_Buffer,uint8_t* Rx_Buffer,uint16_t Now_Tx_Length)
{
    /*此SPI接收中断回调函数没用到，为空*/
}


/************************ COPYRIGHT(C) USTC-ROBOWALKER **************************/
