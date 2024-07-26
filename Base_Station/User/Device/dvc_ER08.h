#include "drv_uart.h"
#include "dvc_cargo.h"

/**
 * @brief 源数据
 *
 */
struct Struct_ER08_UART_Data
{
    uint8_t type;
    uint8_t sign1;
    uint8_t floor;
    uint8_t sign2;
    uint8_t x;
    uint8_t y;
    uint8_t sign3;
    char tel[11];
    uint8_t sign4;
    char code[4];
} __attribute__((packed));

class Class_ER08
{
public:
    void Init(UART_HandleTypeDef *huart);
    void UART_RxCpltCallback(uint8_t *Rx_Data);
    inline Enum_Cargo_Type Get_Cargo_Type(void);
    inline Enum_Cargo_Floor Get_Cargo_Floor(void);
    inline uint8_t Get_x(void);
    inline uint8_t Get_y(void);
    inline char *Get_tel(void);
    inline char *Get_code(void);
    Struct_UART_Manage_Object *UART_Manage_Object;
    int8_t Updata_Flag;

protected:
    // 绑定的UART


    Struct_ER08_UART_Data Now_UART_Rx_Data;
    Struct_ER08_UART_Data Pre_UART_Rx_Data;

    Enum_Cargo_Type Cargo_Type;
    Enum_Cargo_Floor Cargo_Floor;

    uint8_t x;
    uint8_t y;
    
    char tel[11];
    char code[4];
    void Data_Process();
};


Enum_Cargo_Type Class_ER08::Get_Cargo_Type()
{
    return (Cargo_Type);
}


Enum_Cargo_Floor Class_ER08::Get_Cargo_Floor()
{
    return (Cargo_Floor);
}

uint8_t Class_ER08::Get_x()
{
    return (x);
}


uint8_t Class_ER08::Get_y()
{
    return (y);
}

char *Class_ER08::Get_tel()
{
    return (tel);
}


char *Class_ER08::Get_code()
{
    return (code);
}

