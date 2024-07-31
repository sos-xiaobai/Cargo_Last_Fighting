#include "alg_astart.h"
#include "main.h"

void Class_Astart::Init(float __step,float __x_distance, float __y_distance)
{
    Step = __step;
    X_Distance = __x_distance;
    Y_Distance = __y_distance;
    AStar_Node_Right.Init(X_Distance, Y_Distance);
    AStar_Node_Up.Init(X_Distance, Y_Distance);
    AStar_Node_Left.Init(X_Distance, Y_Distance);
    AStar_Node_Down.Init(X_Distance, Y_Distance);
    AStar_Node_Right_Up.Init(X_Distance, Y_Distance);
    AStar_Node_Right_Down.Init(X_Distance, Y_Distance);
    AStar_Node_Left_Up.Init(X_Distance, Y_Distance);
    AStar_Node_Left_Down.Init(X_Distance, Y_Distance);
}


bool Class_Astart::AStar_Calulate_CallBack(float __start_x, float __start_y, float __end_x, float __end_y)
{
    Start_X = __start_x;
    Start_Y = __start_y;
    End_X = __end_x;
    End_Y = __end_y;
    if(fabs(Start_X-End_X)<0.01 && fabs(Start_Y-End_Y)<0.01)
    {
        Tmp_target_X = Start_X;
        Tmp_target_Y = Start_Y;
        return true;
    }

    //初始化九宫格节点的目标位置
    AStar_Node_Right.Set_Target(End_X, End_Y);
    AStar_Node_Right_Up.Set_Target(End_X, End_Y);
    AStar_Node_Up.Set_Target(End_X, End_Y);
    AStar_Node_Left_Up.Set_Target(End_X, End_Y);
    AStar_Node_Left.Set_Target(End_X, End_Y);
    AStar_Node_Left_Down.Set_Target(End_X, End_Y);
    AStar_Node_Down.Set_Target(End_X, End_Y);
    AStar_Node_Right_Down.Set_Target(End_X, End_Y);
    
    //初始化九宫格节点的当前位置
    AStar_Node_Right.Set_Location(Start_X+Step, Start_Y);
    AStar_Node_Up.Set_Location(Start_X, Start_Y+Step);
    AStar_Node_Left.Set_Location(Start_X-Step, Start_Y);
    AStar_Node_Down.Set_Location(Start_X, Start_Y-Step);
    AStar_Node_Right_Up.Set_Location(Start_X+Step, Start_Y+Step);
    AStar_Node_Right_Down.Set_Location(Start_X+Step, Start_Y-Step);
    AStar_Node_Left_Up.Set_Location(Start_X-Step, Start_Y+Step);
    AStar_Node_Left_Down.Set_Location(Start_X-Step, Start_Y-Step);

    //计算九宫格节点的代价
    AStar_Node_Right.Calulate_Cost();
    AStar_Node_Up.Calulate_Cost();
    AStar_Node_Left.Calulate_Cost();
    AStar_Node_Down.Calulate_Cost();
    AStar_Node_Right_Up.Calulate_Cost();
    AStar_Node_Right_Down.Calulate_Cost();
    AStar_Node_Left_Up.Calulate_Cost();
    AStar_Node_Left_Down.Calulate_Cost();

    //关闭斜对角的路径
    AStar_Node_Left_Down.Set_Cost(Block_Cost);
    AStar_Node_Left_Up.Set_Cost(Block_Cost);
    AStar_Node_Right_Down.Set_Cost(Block_Cost);
    AStar_Node_Right_Up.Set_Cost(Block_Cost);

    //计算上下左右四个节点的代价 遇到障碍物则代价为Block_Cost
   if(HAL_GPIO_ReadPin(RIGHT_GPIO_Port, RIGHT_Pin)==GPIO_PIN_RESET)
   {
       AStar_Node_Right.Set_Cost(Block_Cost);
   }
   if(HAL_GPIO_ReadPin(UP_GPIO_Port, UP_Pin)==GPIO_PIN_RESET)
   {
       AStar_Node_Up.Set_Cost(Block_Cost);
   }
   if(HAL_GPIO_ReadPin(LEFT_GPIO_Port, LEFT_Pin)==GPIO_PIN_RESET)
   {
       AStar_Node_Left.Set_Cost(Block_Cost);
   }
   if(HAL_GPIO_ReadPin(DOWN_GPIO_Port, DOWN_Pin)==GPIO_PIN_RESET)
   {
       AStar_Node_Down.Set_Cost(Block_Cost);
   }

    //找到最优代价节点
    AStar_Next_Node = Find_Best_Node();

    //如果最优代价节点的代价为Block_Cost 则说明无法到达终点 让其停在原点
    if(AStar_Next_Node.Get_Cost() == Block_Cost)
    {
        Tmp_target_X = Start_X;
        Tmp_target_Y = Start_Y;
        //return false;
    }
    //否则将最优代价节点的位置作为目标点输出
    else
    {
        Tmp_target_X = AStar_Next_Node.Get_Location_X();
        Tmp_target_Y = AStar_Next_Node.Get_Location_Y();
        //return true;
    }
    return false;
}

 Class_AStar_Node Class_Astart::Find_Best_Node()
{
    Class_AStar_Node __tmp_node;
    __tmp_node = AStar_Node_Right_Up;
    if(AStar_Node_Up.Get_Cost() <= __tmp_node.Get_Cost())
    {
        if(AStar_Node_Choice!=AStar_Node_Down_Choice)
        {
            __tmp_node = AStar_Node_Up;
            AStar_Node_Choice = AStar_Node_Up_Choice;
        }
    }
    if(AStar_Node_Left_Up.Get_Cost() <= __tmp_node.Get_Cost())
    {
        if(AStar_Node_Choice!=AStar_Node_Right_Down_Choice)
        {
            __tmp_node = AStar_Node_Left_Up;
            AStar_Node_Choice = AStar_Node_Left_Up_Choice;
        }
    }
    if(AStar_Node_Left.Get_Cost() <= __tmp_node.Get_Cost())
    {
        if(AStar_Node_Choice!=AStar_Node_Right_Choice)
        {
            __tmp_node = AStar_Node_Left;
            AStar_Node_Choice = AStar_Node_Left_Choice;
        }
    }
    if(AStar_Node_Left_Down.Get_Cost() <= __tmp_node.Get_Cost())
    {
        if(AStar_Node_Choice!=AStar_Node_Right_Up_Choice)
        {
            __tmp_node = AStar_Node_Left_Down;
            AStar_Node_Choice = AStar_Node_Left_Down_Choice;
        }
    }
    if(AStar_Node_Down.Get_Cost() <= __tmp_node.Get_Cost())
    {
        if(AStar_Node_Choice!=AStar_Node_Up_Choice)
        {
            __tmp_node = AStar_Node_Down;
            AStar_Node_Choice = AStar_Node_Down_Choice;
        }
    }
    if(AStar_Node_Right_Down.Get_Cost() <= __tmp_node.Get_Cost())
    {
        if(AStar_Node_Choice!=AStar_Node_Left_Up_Choice)
        {
            __tmp_node = AStar_Node_Right_Down;
            AStar_Node_Choice = AStar_Node_Right_Down_Choice;
        }
    }
    if(AStar_Node_Right.Get_Cost() <= __tmp_node.Get_Cost())
    {
        if(AStar_Node_Choice!=AStar_Node_Left_Choice)
        {
            __tmp_node = AStar_Node_Right;
            AStar_Node_Choice = AStar_Node_Right_Choice;
        }
    }
    if(__tmp_node == AStar_Node_Right_Up)
    {
        AStar_Node_Choice = AStar_Node_Right_Up_Choice;
    }
    return __tmp_node;
} 