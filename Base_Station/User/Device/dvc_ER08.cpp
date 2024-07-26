#include "dvc_ER08.h"
#include "string.h"
#include "main.h"


void Class_ER08::Init(UART_HandleTypeDef *huart)
{
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
    Updata_Flag=0;
  }

/**
 * @brief 数据处理过程
 *
 */
uint8_t Cnt=0;
void Class_ER08::Data_Process()
{
    // HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
    // 获取当前原始值数据
    memcpy(&Now_UART_Rx_Data, UART_Manage_Object->Rx_Buffer, 23 * sizeof(uint8_t));
    // 数据处理过程
    Struct_ER08_UART_Data *tmp_buffer = (Struct_ER08_UART_Data *)UART_Manage_Object->Rx_Buffer;

    // 判断是否为无效数据
    if(tmp_buffer->code[0] == 0x00 && tmp_buffer->code[1] == 0x00 && tmp_buffer->code[2] == 0x00 && tmp_buffer->code[3] == 0x00)
    {
        memset(&Now_UART_Rx_Data, 0, 23 * sizeof(uint8_t));
        return;
    }

    Cargo_Floor = (Enum_Cargo_Floor)tmp_buffer->floor;
    Cargo_Type = (Enum_Cargo_Type)tmp_buffer->type;

    x = tmp_buffer->x;
    y = tmp_buffer->y;
    
    memcpy(tel, tmp_buffer->tel, 11 * sizeof(uint8_t));
    memcpy(code, tmp_buffer->code, 4 * sizeof(uint8_t));
	
    Updata_Flag = 1;
    Cnt++;

    memcpy(&Pre_UART_Rx_Data,&Now_UART_Rx_Data,sizeof(Struct_ER08_UART_Data));

    // HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);
}

/**
 * @brief UART通信接收回调函数
 *
 * @param Rx_Data 接收的数据
 */
void Class_ER08::UART_RxCpltCallback(uint8_t *Rx_Data)
{

    Data_Process(); 
    
}