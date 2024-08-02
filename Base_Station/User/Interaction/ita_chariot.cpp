/**
 * @file ita_chariot.cpp
 * @author yssickjgd (yssickjgd@mail.ustc.edu.cn)
 * @brief 人机交互控制逻辑
 * @version 0.1
 * @date 2023-08-29 0.1 23赛季定稿
 *
 * @copyright USTC-RoboWalker (c) 2022
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "ita_chariot.h"
#include "drv_math.h"
#include "tim.h"

/* Private macros ------------------------------------------------------------*/
#define  proport          72000 	//Tclk/(arr+1)=72000000/(1000)
 
//根据Tout= ((arr+1)*(psc+1))/Tclk推出psc值就是本句define定义的值，Tout为音调频率131Hz的倒数，Tclk=72MHz                                                
#define  L1       ((proport/262)-1)//低调　do 的频率
#define  L2       ((proport/296)-1)//低调　re 的频率
#define  L3       ((proport/330)-1)//低调　mi 的频率
#define  L4       ((proport/349)-1)//低调　fa 的频率
#define  L5       ((proport/392)-1)//低调　sol 的频率
#define  L6       ((proport/440)-1)//低调　la 的频率
#define  L7       ((proport/494)-1)//低调　si 的频率
                                               
#define  M1       ((proport/523)-1)//中调　do 的频率
#define  M2       ((proport/587)-1)//中调　re 的频率
#define  M3       ((proport/659)-1)//中调　mi 的频率
#define  M4       ((proport/699)-1)//中调　fa 的频率
#define  M5       ((proport/784)-1)//中调　sol的频率
#define  M6       ((proport/880)-1)//中调　la 的频率
#define  M7       ((proport/988)-1)//中调　si 的频率
 
#define  H1       ((proport/1048)-1)//高调　do 的频率
#define  H2       ((proport/1176)-1)//高调　re 的频率
#define  H3       ((proport/1320)-1)//高调　mi 的频率
#define  H4       ((proport/1480)-1)//高调　fa 的频率
#define  H5       ((proport/1640)-1)//高调　sol的频率
#define  H6       ((proport/1760)-1)//高调　la 的频率
#define  H7       ((proport/1976)-1)//高调　si 的频率
 
#define  Z0       0//
/* Private types -------------------------------------------------------------*/
uint16_t solitary_brave[]=
{
    M6,50,M7,50,H1,50,H2,50,M7,50,H1,50,H1,100,Z0,10,	//爱你孤身走暗巷
    H1,50,M7,50,H1,50,H2,50,M7,50,H1,50,H1,100,Z0,10, 	//爱你不跪的模样
    H1,50,H2,50,H3,50,H2,50,H3,50,H2,50,H3,100,H3,50,H3,50,H2,50,H3,100,H5,100,H3,100,Z0,10 //爱你对峙过绝望不肯哭一场
    
};

uint16_t wind_rise[]=
{	
  //前奏
  L7,25,M1,25,M2,25,M3,25,L3,50,M5,25,M3,25,M3,50,Z0,150, L7,25,M1,25,M2,25,M3,25,L2,50,M5,25,M3,25,M2,25,M3,25,M1,25,M2,25,L7,25,M1,25,L5,25,Z0,25, L7,25,M1,25,M2,25,M3,25,L3,50,M5,25,M3,75,Z0,150,L7,25,M1,25,M2,25,M3,25,L2,50,M5,25,M3,25,M2,25,M3,25,M1,25,M2,25,Z0,20,
  //这一路上走走停停 顺着少年漂流的痕迹
  M2,50,M2,50,M1,25,M2,50,M2,50,M1,25,M2,50,M3,50,M5,50,M3,50, M2,50,M2,50,M1,25,M2,50,M2,50,M1,25,M2,25,M3,25,M2,25,M1,25,L6,100,Z0,10,
  //迈出车站的前一刻 竟有些犹豫
  M2,50,M2,50,M1,25,M2,50,M2,50,M1,25,M2,50,M3,50,M5,50,M3,50, M2,50,M2,50,M3,25,M2,50,M1,50,M2,100,Z0,50,
  //不仅笑着这近乡情怯 仍无法避免
  M2,50,M2,50,M1,25,M2,50,M2,50,M1,25,M2,50,M3,50,M5,50,M3,50, M2,50,M2,50,M3,25,M2,50,M1,50,L6,100,Z0,10,
  //而长野的天 依旧那么暖 风吹起了从前
  M3,25,M2,25,M1,25,M2,25,M1,100, M3,25,M2,25,M1,25,M2,25,M1,50,M1,50, M5,25,M3,25,M2,25,M1,25,M2,25,M1,100,M1,150,Z0,30,
  //从前初识这世间 万般留恋 看着天边似在眼前 也甘愿赴汤蹈火去走它一遍
  M1,50,M2,50,M3,50,M1,50,M6,50,M5,25,M6,25,M6,50,M6,50, M1,25,M7,50,M6,25,M7,25,M7,100,Z0,5, M7,50,M6,25,M7,25,M7,50,M3,50,H1,25,H2,25,H1,25,M7,25,M6,50,M5,50, M6,50,M5,25,M6,25,M6,25,M5,25,M6,25,M5,25,M6,50,M5,25,M2,25,M2,25,M5,50,M5,50,M3,100,M3,100,Z0,25,
  //如今走过这世间 万般留恋 翻过岁月不同侧脸 措不及防闯入你的笑颜
  M1,50,M2,50,M3,50,M1,50,M6,50,M5,25,M6,25,M6,50,M6,50, M1,25,M7,50,M6,25,M7,25,M7,100,Z0,5, M7,50,M6,25,M7,25,M7,50,M3,50,H1,25,H2,25,H1,25,M7,25,M6,50,M5,50, M6,50,H3,25,H3,25,H3,50,M5,50,M6,50,H3,25,H3,25,H3,25,M5,50,M6,25,M6,100,M6,100,M6,100,Z0,25,
  //我曾难自拔于世界之大 也沉溺于其中梦话
  H1,50,H2,50,H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H2,25,H3,25,H3,50, H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,25,H3,50,H3,50,
  //不得真假 不做挣扎 不惧笑话
  H2,50,H1,25,M6,25,M6,25,H1,50, M6,25,H2,25,H1,25,M6,50,M6,25,H1,50,H1,50, H3,100,H3,25,H4,25,H3,50,H3,25,H2,50,H2,50,Z0,25,
  //我曾将青春翻涌成她 也曾指尖弹出盛夏 心之所动且就随缘去吧
  H1,50,H2,50,H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H2,50, H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H3,50,H3,50, H2,50,H1,25,M6,25,M6,25,H3,50,H3,50, H2,50,H1,25,M6,25,M6,25,H1,50,H1,50,H1,100,H1,100,Z0,10,
  //短短的路走走停停  也有了几分的距离
  M2,50,M2,50,M1,25,M2,50,M2,50,M1,25,M2,50,M3,50,M5,50,M3,50, M2,50,M2,50,M1,25,M2,50,M2,50,M1,25,M2,25,M3,25,M2,25,M1,25,L6,100,Z0,10,
  //不知抚摸的是故事还是段心情
  M2,50,M2,50,M1,25,M2,50,M2,50,M1,25,M2,50,M3,50,M5,50,M3,50, M2,50,M2,50,M3,25,M2,50,M1,50,M2,100,Z0,50,
  //也许期待的不过是与时间为敌
  M2,50,M2,50,M1,25,M2,50,M2,50,M1,25,M2,50,M3,50,M5,50,M3,50, M2,50,M2,50,M3,25,M2,50,M1,50,L6,100,Z0,10,
  //再次见到你 微凉晨光里 笑的很甜蜜
  M3,25,M2,25,M1,25,M2,25,M1,100, M3,25,M2,25,M1,25,M2,25,M1,50,M1,50, M5,25,M3,25,M2,25,M1,25,M2,25,M1,100,M1,150,Z0,30,
  //从前初识这世间 万般留恋 看着天边似在眼前 也甘愿赴汤蹈火去走它一遍
  M1,50,M2,50,M3,50,M1,50,M6,50,M5,25,M6,25,M6,50,M6,50, M1,25,M7,50,M6,25,M7,25,M7,100,Z0,5, M7,50,M6,25,M7,25,M7,50,M3,50,H1,25,H2,25,H1,25,M7,25,M6,50,M5,50, M6,50,M5,25,M6,25,M6,25,M5,25,M6,25,M5,25,M6,50,M5,25,M2,25,M2,25,M5,50,M5,50,M3,100,M3,100,Z0,25,
  //如今走过这世间 万般留恋 翻过岁月不同侧脸 措不及防闯入你的笑颜
  M1,50,M2,50,M3,50,M1,50,M6,50,M5,25,M6,25,M6,50,M6,50, M1,25,M7,50,M6,25,M7,25,M7,100,Z0,5, M7,50,M6,25,M7,25,M7,50,M3,50,H1,25,H2,25,H1,25,M7,25,M6,50,M5,50, M6,50,H3,25,H3,25,H3,50,M5,50,M6,50,H3,25,H3,25,H3,25,M5,50,M6,25,M6,100,M6,100,M6,100,Z0,25,
  //我曾难自拔于世界之大 也沉溺于其中梦话
  H1,50,H2,50,H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H2,25,H3,25,H3,50, H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,25,H3,50,H3,50,
  //不得真假 不做挣扎 不惧笑话
  H2,50,H1,25,M6,25,M6,25,H1,50, M6,25,H2,25,H1,25,M6,50,M6,25,H1,50,H1,50, H3,100,H3,25,H4,25,H3,50,H3,25,H2,50,H2,50,Z0,25,
  //我曾将青春翻涌成她 也曾指尖弹出盛夏 心之所动且就随缘去吧
  H1,50,H2,50,H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H2,50, H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H3,50,H3,50, H2,50,H1,25,M6,25,M6,25,H3,50,H3,50, H2,50,H1,25,M6,25,M6,25,H1,50,H1,50,H1,100,H1,100,Z0,10,
  //逆着光行走任风吹雨打吧
  M6,25,H3,50,H3,50,H2,50,H1,25,M6,25,M6,25,H3,50,H2,50,H1,25,M6,25,M6,25,H1,50,H1,50,H1,50,H1,100,Z0,25,
  //晚风吹起你鬓间的白发 抚平回忆留下的疤
  H1,50,H2,50,H2,25,H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H2,25, H3,25,H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,25,H3,50,H3,50,Z0,5,
  //你的眼中明暗交杂 一笑生花 暮色遮住你蹒跚的步伐
  H2,50,H1,25,M6,25,M6,25,H1,50,M6,25,H2,25,H1,25,M6,50,M6,50, H1,50,H1,50,H3,100,H3,25,H4,25,H3,50,H3,25,H2,50,H2,50,H1,100,H2,100,H3,100,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H2,50,
  //走进床头藏起的画 画中的你(低着头说话)
  H3,50,H6,25,H5,25,H5,50,H6,25,H5,25,H5,50,H6,25,H5,25,H3,100,H3,50,H2,50,H1,25,M6,25,M6,25,H3,50,H3,50,H2,50,H1,25,M6,25,M6,25,H1,100,H1,50,Z0,5,
  //我仍 了 以爱之名你还愿意吗（衔接头有问题）
  H1,50,H2,50,H1,50,H1,100,M6,25,H3,50,H3,50,H2,50,H1,25,H6,25,H6,25,H3,50,H3,50,H2,50,H1,25,H6,25,H6,25,H1,50,H1,50,H1,100,Z0,5
};

/* Private variables ---------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/**
* NAME: void Solitary_brave(void)
* FUNCTION : Solitary_brave
*/
/**
* NAME: void Solitary_brave(void)
* FUNCTION : Solitary_brave
*/
bool Sing_Songs(uint16_t Music[],uint16_t Length)
{
    static bool Change_Tone_Flag = 1;
    static uint32_t i = 0;
    static uint32_t CNT = 0;
    static uint32_t length = Length;

    if(Change_Tone_Flag)
    {
        __HAL_TIM_SET_PRESCALER(&htim3, Music[i*2]);
        Change_Tone_Flag = 0;
    }
    else
    {
        CNT++;
        if(CNT>=5*Music[i*2+1])
        {
            i++;
            CNT = 0;
            Change_Tone_Flag = 1;
        }
    }
    if(i>=(length/2)) 
    {
        i = 0;
        return 1;
    }
    return 0;
}

/**
 * @brief 控制交互端初始化
 *
 */
void Class_Chariot::Init()
{
    ER08.Init(&huart3);
    Tjc011.Init(&huart2);
    Cargo_List.Init();
    UART_Manage_Object =  &UART5_Manage_Object;
    Now_Cargo_Number = 0;
    Sim_Send_Flag = 1;
    Last_Sim_Send_Flag = 1;
    Control_Status = Chariot_Disable_Status; //控制状态
}

uint8_t Class_Chariot::Get_Cargo_Data()
{
    if(Cargo_List.Exist_Cargo((uint8_t *)ER08.Get_code())!=NULL)
    {
        return 0;
    }
    Cargo_List.Add_Cargo(ER08.Get_Cargo_Type(), ER08.Get_Cargo_Floor(), ER08.Get_x(), ER08.Get_y(), (uint8_t *)ER08.Get_tel(), (uint8_t *)ER08.Get_code());
    Now_Cargo.Cargo_Type = ER08.Get_Cargo_Type();
    Now_Cargo.Cargo_Floor = ER08.Get_Cargo_Floor();
    Now_Cargo.Position_X = ER08.Get_x();
    Now_Cargo.Position_Y = ER08.Get_y();
    memcpy(Now_Cargo.Code, ER08.Get_code(), 4 * sizeof(uint8_t));
    memcpy(Now_Cargo.Phone_Number, ER08.Get_tel(), 11 * sizeof(uint8_t));
    Now_Cargo_Number++;
    return 1;
}

uint8_t Class_Chariot::Jundge_Cargo()
{
    Struct_Cargo *tmp = Cargo_List.Exist_Cargo((uint8_t *)Tjc011.Get_Input_Number());
    if(tmp!=NULL)
    {
        memcpy(&Now_Cargo, tmp, sizeof(Struct_Cargo));
        return 1;
    }
    return 0;
}

void Class_Chariot::Output_Cargo()
{
    Cargo_List.Delete_Cargo(Now_Cargo.Code);
    Now_Cargo_Number--;
    if(Now_Cargo_Number < 0)
    {
        Now_Cargo_Number = 0;
    }
}


void Class_Chariot::UART_Send_Add_Cargo()
{
    uint8_t send_buf_len = 22;
    uint8_t tmp_send_buf[22] = {0};
    //帧头
    tmp_send_buf[0] = 0xA5;
    // 1代表添加 2代表删除
    tmp_send_buf[1] = 0x01;
    //数据 只截取前19个字节
    memcpy(&tmp_send_buf[2],&Now_Cargo,send_buf_len-3);
    //帧尾
    tmp_send_buf[send_buf_len-1] = 0x11; 
    //保存发送数据
    memcpy(UART_Manage_Object->Tx_Buffer,tmp_send_buf,send_buf_len);
    UART_Manage_Object->Tx_Length = send_buf_len;
    //发送
    HAL_UART_Transmit(UART_Manage_Object->UART_Handler,UART_Manage_Object->Tx_Buffer,UART_Manage_Object->Tx_Length,10);
}

void Class_Chariot::UART_Send_Delete_Cargo()
{
    uint8_t send_buf_len = 22;
    uint8_t tmp_send_buf[22] = {0};
    //帧头
    tmp_send_buf[0] = 0xA5;
    // 1代表添加 2代表删除
    tmp_send_buf[1] = 0x02;
    //数据 只截取前19个字节
    // LittleEndian_To_BigEndian((uint8_t*)&Now_Cargo,tmp_data,17);
    memcpy(&tmp_send_buf[2],&Now_Cargo,send_buf_len-3);
    //帧尾
    tmp_send_buf[send_buf_len-1] = 0x11; 
    //保存发送数据
    memcpy(UART_Manage_Object->Tx_Buffer,tmp_send_buf,send_buf_len);
    UART_Manage_Object->Tx_Length = send_buf_len;
    //发送
    HAL_UART_Transmit(UART_Manage_Object->UART_Handler,UART_Manage_Object->Tx_Buffer,UART_Manage_Object->Tx_Length,10);
}


/**
 * @brief 计算回调函数
 *
 */
void Class_Chariot::TIM_Calculate_PeriodElapsedCallback()
{
}

/**
 * @brief 在线判断回调函数
 *
 */
void Class_Chariot::TIM1msMod50_Alive_PeriodElapsedCallback()
{
  
}



/**
 * @brief 有限状态机控制回调函数
 *
 */
uint16_t prescaler = 1;
uint16_t pwm_duty_pulse = 500;
bool Sing_Flag = 1;
bool tets_fla=0;
uint8_t temp_data[13] = {0};
void Class_FSM_Chariot_Control::Reload_TIM_Status_PeriodElapsedCallback()
{
   if(Chariot->Last_Sim_Send_Flag != Chariot->Sim_Send_Flag)
   {
       Sing_Flag = 0;
   }
   if(Chariot->Sim_Send_Flag == 0 && !Sing_Flag)
   {
     Sing_Flag = Sing_Songs(wind_rise,18);
   }
   else if(Chariot->Sim_Send_Flag == 1 && !Sing_Flag) 
   {
     Sing_Flag = Sing_Songs(solitary_brave,10);
   }
   if(Sing_Flag)
   __HAL_TIM_SET_PRESCALER(&htim3, 1);
   Chariot->Last_Sim_Send_Flag = Chariot->Sim_Send_Flag;

    //配置PSC预分频值
//    __HAL_TIM_SET_PRESCALER(&htim3, prescaler);
//    //配置PWM频率 ARR
//    __HAL_TIM_SetAutoreload(&htim3, (uint16_t)pwm_freq_arr);
//    //配置PWM占空比
//    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, (uint16_t)pwm_duty_pulse);

    Status[Now_Status_Serial].Time++;
    switch (Now_Status_Serial)
    {
        case 0:
        // 获取货物信息 如果更新就获取信息 
        if (Chariot->ER08.Updata_Flag)
        {          
            //如果货物信息不重复 添加当前货物信息入链表  更新Now_Cargo
            if(Chariot->Get_Cargo_Data())
            {
                // 发送短信
                if(Chariot->Sim_Send_Flag==1)
                Chariot->SIM900A.Sim900a_Send_Data((char *)Chariot->Now_Cargo.Code, (char *)Chariot->Now_Cargo.Phone_Number);
                // 给esp32发送添加货物信息
                Chariot->UART_Send_Add_Cargo();
                sprintf(temp_data, "click m0,1\xff\xff\xff", 13);
                HAL_HalfDuplex_EnableTransmitter(&huart3);
                HAL_UART_Transmit(&huart3, temp_data, 13, 100);
            }
            Chariot->ER08.Updata_Flag = 0;
        }

        // 获取取件码信息 如果更新就获取信息 
        if (Chariot->Tjc011.Input_data_Flag)
        {
            // 如果取件码正确 更新Now_Cargo
            if (Chariot->Jundge_Cargo()) 
            {
                // 给esp32发送删除货物信息
                Chariot->UART_Send_Delete_Cargo();
                Chariot->Output_Cargo();
                
            }
            Chariot->Tjc011.Input_data_Flag = 0;
        }

    }
}

/************************ COPYRIGHT(C) USTC-ROBOWALKER **************************/
