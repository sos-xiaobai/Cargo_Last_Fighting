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
void TIM6_Task1ms_PeriodElapsedCallback();
void TIM7_Task5ms_PeriodElapsedCallback();
void SPI1_IMU_Task_Callback(uint8_t* Tx_Buffer,uint8_t* Rx_Buffer,uint16_t Now_Tx_Length);

/* Function prototypes -------------------------------------------------------*/
void UART1_Esp32_Callback(uint8_t *Buffer, uint16_t Length)
{
    if (Buffer[0] == 0xA5 && Buffer[21] == 0x11) 
    {
        // 解析数据
        uint8_t command = Buffer[1];
        memcpy(&Chariot.Now_Cargo, &Buffer[2], 19);

        // 根据命令类型处理数据
        if (command == 0x01) 
        {
            // 添加货物 入库队列 入队列
            Fifo_Enqueue(&Chariot.Input_Cargo_FIFO, (const uint8_t*)&Chariot.Now_Cargo, sizeof(Struct_Cargo));
            Chariot.Cargo_List.Add_Cargo(Chariot.Now_Cargo.Cargo_Type, Chariot.Now_Cargo.Cargo_Floor, Chariot.Now_Cargo.Position_X, Chariot.Now_Cargo.Position_Y, Chariot.Now_Cargo.Phone_Number, Chariot.Now_Cargo.Code);
        } 
        else if (command == 0x02) 
        {
            // 删除货物 出库队列 入队列
            Fifo_Enqueue(&Chariot.Output_Cargo_FIFO, (const uint8_t*)&Chariot.Now_Cargo, sizeof(Struct_Cargo));
            Chariot.Cargo_List.Delete_Cargo(Chariot.Now_Cargo.Code);
        }
        // 清空缓冲区
        memset(Buffer, 0, 22);
    }
}

void UART2_Buluteeth_Callback(uint8_t *Buffer, uint16_t Length)
{
    // //对面小车出入库数据
    // if (Buffer[0] == 0xA5 && Buffer[2] == 0x11) 
    // {
    //     // 解析数据
    //     uint8_t command = Buffer[1];
    //     Struct_Cargo tmp_cargo;

    //     // 对面小车已经入库该货物
    //     if (command == 0x01) 
    //     {
    //         // 入库队列 出队列
    //         Fifo_Dequeue(&Chariot.Input_Cargo_FIFO, (uint8_t*)&tmp_cargo, sizeof(Struct_Cargo));
    //     } 
    //     // 对面小车已经出库该货物
    //     else if (command == 0x02) 
    //     {
    //         // 出库队列 出队列
    //         Fifo_Dequeue(&Chariot.Output_Cargo_FIFO, (uint8_t*)&tmp_cargo, sizeof(Struct_Cargo));
    //     }
    //     memset(Buffer, 0, 3);
    // }   
    //对面小车的状态
    if(Buffer[0] == 0xAA && Buffer[2] == 0x12)
    {
        // 解析数据
        uint8_t command = Buffer[1];
        // 对面机器人空闲
        if(command == 0x01)
        {
            Chariot.Set_Another_Chariot_Status(Chariot_Free_Status);
        }
        // 对面机器人忙碌
        else if(command == 0x02)
        {
            Chariot.Set_Another_Chariot_Status(Chariot_Busy_Status);
        }
        memset(Buffer, 0, 3);
    }
}

void UART5_K210_Callback(uint8_t *Buffer, uint16_t Length)
{
    Chariot.MiniPC.UART_RxCpltCallback(Buffer);
}



/**
 * @brief 初始化任务
 *
 */
 
uint16_t test_compare1,test_compare2,test_compare3,test_compare4;
uint8_t start_flag=0;
void Task_Init()
{  
    //DWT初始化
    DWT_Init(168);

    //定时器循环任务
    TIM_Init(&htim6, TIM6_Task1ms_PeriodElapsedCallback);
    TIM_Init(&htim7, TIM7_Task5ms_PeriodElapsedCallback);

    //陀螺仪SPI分配
    SPI_Init(&hspi1,SPI1_IMU_Task_Callback);

    //初始化串口接受中断函数
    UART_Init(&huart1, UART1_Esp32_Callback, 22);
    UART_Init(&huart2, UART2_Buluteeth_Callback, 3);
    UART_Init(&huart5, UART5_K210_Callback, 7);

    /********************************* 设备层初始化 *********************************/

     //设备层集成在交互层初始化中，没有显视地初始化

    /********************************* 交互层初始化 *********************************/

    FSM_Chariot.Chariot = &Chariot;
    FSM_Chariot.Init(5,0);
    FSM_Chariot.Chariot->Init(&huart2);  //串口2用于小车蓝牙通信

    /********************************* 使能调度时钟 *********************************/

   //启动定时器任务中断
   HAL_TIM_Base_Start_IT(&htim6);
   HAL_TIM_Base_Start_IT(&htim7);
}


/**
 * @brief TIM6任务回调函数
 *
 */
float astart_test_x = 1.0f;
float astart_test_y = 1.0f;
float _test_x = 0.0f;
float _test_y = -10.0f;
uint8_t test_flag = 0;
uint8_t Reset_flag = 0;
float change_flag = 10;
float k210_angle = 20;
void TIM6_Task1ms_PeriodElapsedCallback()
{	
   // HAL_GPIO_WritePin(WAKE_GPIO_Port, WAKE_Pin, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
   //任务状态机
   FSM_Chariot.Reload_TIM_Status_PeriodElapsedCallback();
    // if(change_flag == 0)
    // {
    //     if(Chariot.Astart.AStar_Calulate_CallBack(Chariot.Chassis.Get_Now_Position_X(), Chariot.Chassis.Get_Now_Position_Y(), -1,-1))
    //     change_flag = 1;
    // }
    // //底盘A*算法
    // if(change_flag==1)
    // {
        //  if(Chariot.Astart.AStar_Calulate_CallBack(Chariot.Chassis.Get_Now_Position_X(), Chariot.Chassis.Get_Now_Position_Y(), 1,1))
        //  change_flag = 0;
    // }
    // Chariot.Chassis.Set_Target_Position_X(Chariot.Astart.Get_Tmp_Target_X());
    // Chariot.Chassis.Set_Target_Position_Y(Chariot.Astart.Get_Tmp_Target_Y());

     //Chariot.Chassis.TIM_Position_X_Y_PID_K210_PeriodElapsedCallback(K210_Backward);
    // Chariot.Astart.AStar_Calulate_CallBack(-1.0f*Chariot.Chassis.Get_Now_Position_X(), Chariot.Chassis.Get_Now_Position_Y(), astart_test_x, astart_test_y);
    // Chariot.Chassis.Set_Target_Position_X(-1.0f*Chariot.Astart.Get_Tmp_Target_X());
    // Chariot.Chassis.Set_Target_Position_Y(Chariot.Astart.Get_Tmp_Target_Y());
    // Chariot.Chassis.TIM_Position_X_Y_PID_Encoder_PeriodElapsedCallback();

    // Chariot.Chassis.TIM_Position_X_Y_PID_K210_PeriodElapsedCallback(K210_Backward);
    // Chariot.MiniPC.Set_target_x(_test_x);
    // Chariot.MiniPC.Set_target_y(_test_y);

//    if(test_flag==1)
//    Chariot.Servo_Caculate(-0.32f, 0.0f, 180.0f);
//    if(test_flag==2)
//    Chariot.Servo_Caculate(0.07f, 0.1f, 0.0f);
//    if(test_flag==3)
//    Chariot.Servo_Caculate(0.2f, -0.05f, 0.0f);
//    if(test_flag==4)
//    Chariot.Servo_Caculate(0.2f, 0.17f, 27.0f);
////Chariot.Servo[1].Set_Angle(k210_angle);

      
//    Chariot.Servo[0].Set_Angle(CLOSED);
//    Chariot.Servo[5].Set_Angle(k210_angle);
     

    //底盘速度解算
    Chariot.Chassis.Speed_Resolution();
   // IMU任务
   FSM_Chariot.Chariot->Chassis.IMU.TIM_Calculate_PeriodElapsedCallback();    

   // 加速度积分计算位移
   Chariot.Chassis.TIM1ms_Chassis_Posture_PeriodElapsedCallback();

   // 编码器微分计算转速
    for (auto i = 0; i < 4; i++)
        Chariot.Chassis.Motor[i].TIM1ms_Motor_Data_PeriodElapsedCallback();
}


/**
 * @brief TIM7任务回调函数
 *
 */
float test_angle1 = 90.0f;
float test_angle2 = 80.0f;
float test_angle3 = 90.0f;
float test_angle4 = 80.0f;
static uint32_t cnt = 0;
float x =-0.32f;
float y = 0.0f;
float angle = 180.0f;
bool Success_Flag = 0;



float test_angle_1_ = 0.0f;
float test_angle_2_ = 0.0f;
float test_angle_3_ = 0.0f;
float test_angle_4_ = 0.0f;
float test_angle_5_ = 45.0f;
float test_angle_6_ = 83.0f;

float angle_test = -90;

void TIM7_Task5ms_PeriodElapsedCallback()
{
    /****************************** 交互层回调函数 1ms *****************************************/
    static uint16_t cnt=0;
    static uint16_t test_cnt=0;
    cnt++;
    test_cnt++;
    //90 -80  90 -85 初始
    //90 90 -55  60 放一层货架
    //90  80  90  80 中间
    //0.100000001  0.150000006  0
//     cnt++;
//     Chariot.Servo[0].Set_Angle(test_angle1);
//     Chariot.Servo[1].Set_Angle(-1.0f*ptheta[0]); //-1
//         if(cnt>150)
//     Chariot.Servo[2].Set_Angle(ptheta[1]);
//     Chariot.Servo[3].Set_Angle(ptheta[2]);
        //-0.32 0 180 初始位置
        //0.07 0.1  0  中间 
        //0.2 0.17 17 高层放置
        //0.2 -0.05 0  底层放置

//    Chariot.Servo[0].Set_Angle(test_angle_1_);
    // Chariot.Servo[1].Set_Angle(test_angle_2_); //负数
    // Chariot.Servo[2].Set_Angle(test_angle_3_); //负数
    // Chariot.Servo[3].Set_Angle(test_angle_4_);  
//    Chariot.Servo[4].Set_Angle(test_angle_5_);
//    Chariot.Servo[5].Set_Angle(test_angle_6_);   
    // Chariot.Servo[5].Set_Angle(K210_SHEET);
    
    // Chariot.Chassis.TIM_Position_X_Y_PID_K210_PeriodElapsedCallback(K210_Backward);
    // Chariot.Astart.AStar_Calulate_CallBack(Chariot.Chassis.Get_Now_Position_X(), Chariot.Chassis.Get_Now_Position_Y(), astart_test_x, astart_test_y);
    // Chariot.Chassis.Set_Target_Position_X(Chariot.Astart.Get_Tmp_Target_X());
    // Chariot.Chassis.Set_Target_Position_Y(Chariot.Astart.Get_Tmp_Target_Y());
    // Chariot.Chassis.TIM_Position_X_Y_PID_Encoder_PeriodElapsedCallback();

    //底盘yaw 角度环PID
    if(Chariot.Chassis.IMU.imu_start_flag==1)
     Chariot.Chassis.TIM_Position_Yaw_PID_Encoder_PeriodElapsedCallback();

    //Chariot.Chassis.TIM_Position_X_Y_PID_Encoder_PeriodElapsedCallback();
   
    //四电机速度环PID
   for(auto i = 0; i < 4; i++)
       Chariot.Chassis.Motor[i].TIM5ms_Motor_Calculate_PeriodElapsedCallback();
    
    /****************************** 驱动层回调函数 1ms *****************************************/ 
  
    // 给另一台车发送当前小车状态
    Chariot.UART_Transmit_Status();

    //k210状态
    if(cnt>100)
    {
	    Chariot.MiniPC.TIM_50ms_Alive_PeriodElapsedCallback();
	    cnt=0;
    }
}
uint8_t dddd=0;
/**
 * @brief 前台循环任务
 *
 */
void Task_Loop()
{
    HAL_UART_Transmit(&huart5,&dddd,1,50);
    //喂狗
    HAL_IWDG_Refresh(&hiwdg);
}



void SPI1_IMU_Task_Callback(uint8_t* Tx_Buffer,uint8_t* Rx_Buffer,uint16_t Now_Tx_Length)
{
    /*此SPI接收中断回调函数没用到，为空*/
}


/************************ COPYRIGHT(C) USTC-ROBOWALKER **************************/
