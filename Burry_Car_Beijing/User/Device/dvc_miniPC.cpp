#include "dvc_miniPC.h"



void Class_MiniPC::Init(UART_HandleTypeDef *huart,float __target_x,float __target_y)
{
    PID_X.Init(1,0.0,0.0);
    PID_Y.Init(1,0.0,0.0);
    target_x = __target_x;
    target_y = __target_y;

    if (huart->Instance == USART1)
    {
        UART_Manage_Object = &UART1_Manage_Object;
    }
    else if (huart->Instance == USART2)
    {
        UART_Manage_Object = &UART2_Manage_Object;
    }
    else if (huart->Instance == USART3)
    {
        UART_Manage_Object = &UART3_Manage_Object;
    }
    else if (huart->Instance == UART4)
    {
        UART_Manage_Object = &UART4_Manage_Object;
    }
    else if (huart->Instance == UART5)
    {
        UART_Manage_Object = &UART5_Manage_Object;
    }
    else if (huart->Instance == USART6)
    {
        UART_Manage_Object = &UART6_Manage_Object;
    }
}



void Class_MiniPC::UART_RxCpltCallback(uint8_t *Rx_Data)
{
    flag++;
    Data_Process();
}



void Class_MiniPC::Data_Process()
{
    uint8_t len = sizeof(Struct_MiniPC_UART_Data);
    if(UART_Manage_Object->Rx_Buffer[0] == 0xA5 && UART_Manage_Object->Rx_Buffer[len-1] == 0x11)
    {
        memcpy(&Now_UART_Rx_Data, UART_Manage_Object->Rx_Buffer, sizeof(Struct_MiniPC_UART_Data));
        now_x = Now_UART_Rx_Data.x;
        now_y = Now_UART_Rx_Data.y;
    }
    memcpy(&Pre_UART_Rx_Data, &Now_UART_Rx_Data, sizeof(Struct_MiniPC_UART_Data));
}


void Class_MiniPC::TIM_Calculate_PeriodElapsedCallback()
{
    if(MiniPC_Status == MiniPC_Enable_Status)
    {
        PID_X.Set_Target(target_x);
        PID_Y.Set_Target(target_y);
        PID_X.TIM_Adjust_PeriodElapsedCallback();
        PID_Y.TIM_Adjust_PeriodElapsedCallback();
        target_x_speed = PID_X.Get_Out();
        target_y_speed = PID_Y.Get_Out();
    }
    else if(MiniPC_Status == MiniPC_Disable_Status)
    {
        target_x_speed = 0;
        target_y_speed = 0;
    }
}


void Class_MiniPC::TIM_50ms_Alive_PeriodElapsedCallback()
{
    if(flag == pre_flag)
    {
        MiniPC_Status = MiniPC_Disable_Status;
    }
    else if (flag != pre_flag)
    {
        MiniPC_Status = MiniPC_Enable_Status;
    }
    pre_flag = flag;
}