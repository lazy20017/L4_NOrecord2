#include "stdio.h"
#include "usart.h"
#include "my_usart.h"
#include "stdarg.h"
#include "string.h"




//Retargets the C library printf function to the UART.
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}






/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART1_printf()调用
 */
char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*
 * 函数名：USART_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无
 * 调用  ：外部调用
 *         典型应用USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *            		 USART1_printf( USART1, "\r\n %d \r\n", i );
 *            		 USART1_printf( USART1, "\r\n %s \r\n", j );
 */
void USART_printf(UART_HandleTypeDef* USARTx, uint8_t *Data,...)
{
    const char *s=0;
    int d;
    char buf[16];
    int16_t n=0;
    uint8_t status=0;
    uint8_t  my_0A=0X0A;
    uint8_t  my_0D=0X0D;

    va_list ap;
    va_start(ap, Data);
    ////////////


    if(*Data==0x10) {
        n=6;
        status=1;
    }
    else if(*Data==0x68&& *(Data+3)==0x68) {
        n=(*(Data+1)+6);
        status=1;
    }
    else if(*Data==0x67&& *(Data+5)==0x67) {
        n=(*(Data+1)+((uint16_t)(*(Data+2))<<8)+6);
        status=1;
    }

    else {
        status=0;
    }

    //while(HAL_UART_GetState(USARTx) !=HAL_UART_STATE_READY );

    if(status==1)
    {
        HAL_UART_Transmit(USARTx,Data,n,1000);
    }


    /////////////

    //while( HAL_UART_GetState(USARTx) !=HAL_UART_STATE_READY );
    while ( status==0&&*Data != 0)     // 判断是否到达字符串结束符，正常传输，不是101协议字符串
    {
        if (*Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //回车符
                //USART_SendData(USARTx, 0x0d);
                HAL_UART_Transmit(USARTx,&my_0D,1,1000);
                Data ++;
                break;

            case 'n':							          //换行符
                //USART_SendData(USARTx, 0x0a);
                HAL_UART_Transmit(USARTx,&my_0A,1,1000);
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {   //
            switch ( *++Data )
            {
            case 's':										  //字符串
                s = va_arg(ap, const char *);
                HAL_UART_Transmit(USARTx,(uint8_t *)s,strlen(s),1000);
                Data++;
                break;

            case 'd':										//十进制
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                s=buf;
                HAL_UART_Transmit(USARTx,(uint8_t *)s,strlen(s),1000);
                Data++;
                break;
            case 'c':										//字符
                HAL_UART_Transmit(USARTx,(uint8_t *)s,1,1000);
                //while( HAL_UART_GetState(USARTx) !=HAL_UART_STATE_READY );
                Data++;
                break;

            default:
                Data++;
                break;
            }
        } /* end of else if */
        else HAL_UART_Transmit(USARTx,Data++,1,1000);

        //while( HAL_UART_GetState(USARTx) !=HAL_UART_STATE_READY );
    }

    //------------
}



/*

功能：自定义USART_printf输出%f参数，只能用一个%f，
例如：USART_printf1(&huart2,"x=%f\n",1.23456789);
*/
void USART_printf1(UART_HandleTypeDef* USARTx, uint8_t *Data,double my_data)
{
    char my_buf[10];


    sprintf(my_buf,"%lf",my_data);
    while (*Data != '\0')
    {
        if(*Data=='%')
            USART_printf(USARTx,"%s",my_buf);
        else if(*Data=='f') ;
        else	HAL_UART_Transmit(USARTx,(uint8_t *)Data,1,1000);

        Data++;
    }

}
