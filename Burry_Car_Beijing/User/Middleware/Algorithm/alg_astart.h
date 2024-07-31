#pragma once

#include "math.h"

#define Block_Cost 1e6  //障碍物代价 尽量大

enum Enum_AStar_Node_Choice
{
    AStar_Node_Right_Up_Choice = 0,
    AStar_Node_Up_Choice,
    AStar_Node_Left_Up_Choice,
    AStar_Node_Left_Choice,
    AStar_Node_Left_Down_Choice,
    AStar_Node_Down_Choice,
    AStar_Node_Right_Down_Choice,
    AStar_Node_Right_Choice,
};


class Class_AStar_Node
{

    protected:
    float Target_X;
    float Target_Y;
    float Location_X;
    float Location_Y;
    float Distance;
    float Cost;

    //地图x和y最大范围 单位/m ±x和±y
    float X_Distance;
    float Y_Distance;

    
    public:

    void Init(float __x_distance, float __y_distance)
    {
        this->X_Distance = __x_distance;
        this->Y_Distance = __y_distance;
    }

    bool operator <= (const Class_AStar_Node &__node) const
    {
        return (this->Cost <= __node.Cost);
    }

    bool operator = (const Class_AStar_Node &__node)
    {
        this->Target_X = __node.Target_X;
        this->Target_Y = __node.Target_Y;
        this->Location_X = __node.Location_X;
        this->Location_Y = __node.Location_Y;
        this->Distance = __node.Distance;
        this->Cost = __node.Cost;
        this->X_Distance = __node.X_Distance;
        this->Y_Distance = __node.Y_Distance;
        return true;
    }

    bool operator == (const Class_AStar_Node &__node) const
    {
        return (this->Location_X == __node.Location_X && this->Location_Y == __node.Location_Y);
    }

    bool operator != (const Class_AStar_Node &__node) const
    {
        return (this->Location_X != __node.Location_X || this->Location_Y != __node.Location_Y);
    }

    void Calulate_Distance()
    {
        this->Distance = abs(this->Target_X - this->Location_X) + abs(this->Target_Y - this->Location_Y);
    }

    void Calulate_Cost()
    {
        if (fabs(this->Location_X) > this->X_Distance || fabs(this->Location_Y) > this->Y_Distance)
        {
            this->Cost = Block_Cost;
        }
        else
        {
            this->Calulate_Distance();
            this->Cost = this->Distance;
        }
    }

    inline float Get_Distance()
    {
        return (this->Distance);
    }

    inline void Set_Cost(float __cost)
    {
        this->Cost = __cost;
    }

    inline float Get_Cost()
    {
        return (this->Cost);
    }

    inline void Set_Target(float __target_x, float __target_y)
    {
        this->Target_X = __target_x;
        this->Target_Y = __target_y;
    }

    inline float Get_Location_X()
    {
        return (this->Location_X);
    }

    inline float Get_Location_Y()
    {
        return (this->Location_Y);
    }

    inline void Set_Location(float __location_x, float __location_y)
    {
        this->Location_X = __location_x;
        this->Location_Y = __location_y;
    }
};


class Class_Astart
{
    public:
        void Init(float __step,float __x_distance, float __y_distance);
        
        bool AStar_Calulate_CallBack(float __start_x, float __start_y, float __end_x, float __end_y);

        inline float Get_Tmp_Target_X(){return (this->Tmp_target_X);}
        inline float Get_Tmp_Target_Y(){return (this->Tmp_target_Y);}

    protected:

        //九宫格周围的的节点
        Class_AStar_Node AStar_Node_Right;
        Class_AStar_Node AStar_Node_Right_Up;
        Class_AStar_Node AStar_Node_Up; 
        Class_AStar_Node AStar_Node_Left_Up;
        Class_AStar_Node AStar_Node_Left;
        Class_AStar_Node AStar_Node_Left_Down;
        Class_AStar_Node AStar_Node_Down;
        Class_AStar_Node AStar_Node_Right_Down;
        
        //最优节点的方向选择
        Enum_AStar_Node_Choice AStar_Node_Choice = AStar_Node_Right_Up_Choice;

        //最终的最优节点
        Class_AStar_Node AStar_Next_Node;
        Class_AStar_Node Pre_AStar_Node;

        //地图x和y最大范围 单位/m
        float X_Distance;
        float Y_Distance;

        //步长 每个周期遍历的步长 单位/m
        float Step;

        //起点和终点
        float Start_X;
        float Start_Y;
        float End_X;
        float End_Y;

        //这个周期的目标点
        float Tmp_target_X;
        float Tmp_target_Y;

        //找到上下左右中的最优节点
        Class_AStar_Node Find_Best_Node();

};