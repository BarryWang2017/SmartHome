#include "dev_uart.h"

uint8_t sn;
uint16_t year;
uint8_t month;
uint8_t day;
uint8_t hour;
uint8_t minute;
uint8_t second;


DEV_GPIO_PTR led;
#define LED_PORT DW_GPIO_PORT_B
#define LED_MASK 0x1ff
#define LED_DIR  0x1ff


/*
函数功能：MCU向WiFi模组回复设备信息。
包括：通用串口协议版本号
	  业务协议版本号
	  硬件版本号
	  软件版本号
	  产品标识码（在机智云创建产品时分配的全网唯一标识码，从机智云获取）
	  设备属性
	  产品密钥（与产品标识码对应的产品密匙，从机智云获取）

|---------------|----------------|---------|----------------------------------------------------------------------|
|参数名         |类型            |方向     |说明                                                                  |
|---------------|----------------|---------|----------------------------------------------------------------------|
|esp8266_uart   |DEV_UART_PTR    |in       |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值        |
|---------------|----------------|---------|----------------------------------------------------------------------|
|product_key    |uint8_t         |in       |字符指针，传入机智云产品标识码                                          |
|---------------|----------------|---------|----------------------------------------------------------------------|
|product_secret |uint8_t         |in       |字符指针，传入机智云产品密钥                                          |
|---------------|----------------|---------|----------------------------------------------------------------------|
*/
extern void mcu2wifi_product_info(DEV_UART_PTR esp8266_uart, uint8_t *product_key, uint8_t *product_secret);
/*
函数功能：MCU向WiFi发送心跳包。
当WiFi模组超过55秒没有收到MCU的数据包，应向MCU发送心跳包。MCU收到心跳包后马上回复。
当WiFi模组连续3次没有收到MCU的心跳回复，进行报警。

|---------------|----------------|---------|----------------------------------------------------------------------|
|参数名         |类型            |方向     |说明                                                                  |
|---------------|----------------|---------|----------------------------------------------------------------------|
|esp8266_uart   |DEV_UART_PTR    |in       |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值        |
|---------------|----------------|---------|----------------------------------------------------------------------|
*/
extern void mcu2wifi_heartbeat(DEV_UART_PTR esp8266_uart);


/*
函数功能：MCU向WiFi发送指令，通知WiFi模组进入配置模式。
当WiFi模组收到进入配置模式的指令后，让设备进入对应的SoftAP或AirLink等OnBoarding配置方式。

|---------------|----------------|---------|----------------------------------------------------------------------|
|参数名         |类型            |方向     |说明                                                                  |
|---------------|----------------|---------|----------------------------------------------------------------------|
|esp8266_uart   |DEV_UART_PTR    |in       |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值        |
|---------------|----------------|---------|----------------------------------------------------------------------|
|mode           |uint8_t         |in       |配置模式，1为SoftAP方式，2为AirLink方式;                              |
|               |                |         |配置方式不合法时，默认进入AirLink配置方式，超时时间1分钟。            |
|               |                |         |softAp配置，超时时间5分钟                                             |
|---------------|----------------|---------|----------------------------------------------------------------------|
*/
extern void mcu2wifi_set_cfg_mode(DEV_UART_PTR esp8266_uart,uint8_t mode);


/*
函数功能：MCU向WiFi发送指令，重置WiFi模组。
重置的内容包括WiFi模组保存的局域网WiFi SSID和密码,DID,Passcode等信息。
重置后模组重启进入AirLink配置模式,超时时间5分钟。

|---------------|----------------|---------|----------------------------------------------------------------------|
|参数名         |类型            |方向     |说明                                                                  |
|---------------|----------------|---------|----------------------------------------------------------------------|
|esp8266_uart   |DEV_UART_PTR    |in       |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值        |
|---------------|----------------|---------|----------------------------------------------------------------------|
*/
extern void mcu2wifi_reset_wifi(DEV_UART_PTR esp8266_uart);


/*
函数功能：WiFi模组向MCU推送WiFi状态后，MCU向WiFi回复信息。
同时打印WiFi状态，通过LED灯显示相应的状态。
|---------------|----------------|---------|----------------------------------------------------------------------|
|参数名         |类型            |方向     |说明                                                                  |
|---------------|----------------|---------|----------------------------------------------------------------------|
|esp8266_uart   |DEV_UART_PTR    |in       |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值        |
|---------------|----------------|---------|----------------------------------------------------------------------|
|receive_data   |uint8_t         |in       |uint8_t类型的指针，用来传入WiFi发送给MCU的数据                        |
|---------------|----------------|---------|----------------------------------------------------------------------|
*/
extern void mcu2wifi_wifi_statu(DEV_UART_PTR esp8266_uart,uint8_t *receive_data);

/*
函数功能：MCU回应WiFi模组对应包序号的数据包非法。
请求WiFi模组重新发送。

|---------------|----------------|---------|----------------------------------------------------------------------|
|参数名         |类型            |方向     |说明                                                                  |
|---------------|----------------|---------|----------------------------------------------------------------------|
|esp8266_uart   |DEV_UART_PTR    |in       |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值        |
|---------------|----------------|---------|----------------------------------------------------------------------|
*/
extern void mcu2wifi_receive_error(DEV_UART_PTR esp8266_uart);
/* 
函数功能：MCU请求网络时间

|--------------|------------|---------|--------------------------------------------------------------|
|参数名        |类型        |方向     |说明                                                          |
|--------------|------------|---------|--------------------------------------------------------------|
|esp8266_uart  |DEV_UART_PTR|input    |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值|
|--------------|------------|---------|--------------------------------------------------------------|
*/
extern void mcu2wifi_request_time(DEV_UART_PTR esp8266_uart);
/* 
函数功能：WiFi模组回复网络时间

|--------------|------------|---------|--------------------------------------------------------------|
|参数名        |类型        |方向     |说明                                                          |
|--------------|------------|---------|--------------------------------------------------------------|
|receive_data  |uint8_t     |in       |uint8_t类型的指针，用来传入WiFi发送给MCU的数据                |
|--------------|------------|---------|--------------------------------------------------------------|
*/
extern void wifi2mcu_get_time(uint8_t *receive_data);
/* 
函数功能：mcu重启wifi模组

|--------------|-------------|---------|--------------------------------------------------------------|
|参数名        |类型         |方向     |说明                                                          |
|--------------|-------------|---------|--------------------------------------------------------------|
|esp8266_uart  |DEV_UART_PTR |input    |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值|
|--------------|-------------|---------|--------------------------------------------------------------|
*/
extern void mcu2wifi_restart_wifi(DEV_UART_PTR esp8266_uart);
/* 
函数功能：MCU请求wifi模组进入可绑定模式，可绑定时间由获取设备信息 函数中的”可绑定状态失效时间“确定。

|--------------|------------|---------|--------------------------------------------------------------|
|参数名        |类型        |方向     |说明                                                          |
|--------------|------------|---------|--------------------------------------------------------------|
|esp8266_uart  |DEV_UART_PTR|input    |DEV_UART类型的指针，传入前必须先使用uart_get_dev()函数进行赋值|
|--------------|------------|---------|--------------------------------------------------------------|
*/
extern void mcu2wifi_request_into_binding(DEV_UART_PTR esp8266_uart);