#ifndef _DVC_CARGO_H
#define _DVC_CARGO_H

#include "main.h"
#include <string.h>


/*
* 货物类型 大/小 Ascii码值
*/
typedef enum Enum_Cargo_Type 
{
    Cargo_Type_Samll = 48,
    Cargo_Type_Big,
}Enum_Cargo_Type;


/*
* 货物位置 层数 Ascii码值
*/
typedef enum Enum_Cargo_Floor 
{
    Cargo_First_Floor = 49,
    Cargo_Second_Floor,
}Enum_Cargo_Floor;


struct Struct_Cargo
{
    Enum_Cargo_Type Cargo_Type;
    Enum_Cargo_Floor Cargo_Floor;
    uint8_t Position_X;
    uint8_t Position_Y;
    uint8_t Phone_Number[11];
    uint8_t Code[4];
    Struct_Cargo* Next_Cargo;
} __attribute__((packed));


/**
 * @brief 货物列表类
 * 
 * 该类用于管理货物列表，包括初始化列表、添加货物、删除货物以及查找货物等功能。
 */
class Class_Cargo_List
{
public:

    Struct_Cargo* First_Cargo;

    void Init();
    void Add_Cargo(Enum_Cargo_Type Cargo_Type, Enum_Cargo_Floor Cargo_Floor, uint8_t Position_X, uint8_t Position_Y, uint8_t* Phone_Number, uint8_t* Code);
    void Delete_Cargo(uint8_t Code[4]);
    Struct_Cargo* Exist_Cargo(uint8_t Code[4]);
};

#endif
