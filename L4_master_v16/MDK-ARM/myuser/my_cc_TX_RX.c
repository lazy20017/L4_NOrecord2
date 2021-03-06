/*
CC1101通信业务逻辑部分

*/

#include "my_cc_TX_RX.h"

#define COM_CC1101_WOR_WakeUp {0X68,0X05,0XFE,0X01,0XA0,0XA3,0XC1,0X16}
uint8_t MY_CC1101_STATUS=0;  //标识CC1101当前状态，空闲默认00，TX 为01，RX为02，WOR为03
uint8_t MY_TX_RX_STEP=0;   //命令的步骤



uint8_t Cmd_CC1101_TX[70]=COM_CC1101_WOR_WakeUp;
uint8_t Cmd_CC1101_RX[70]= {0};
uint16_t my_cmd_cc1101_step=0;


/*
功能：发送CC1101数据
参数：
第一个参数命令存储器地址，
第二个参数命令字节数，
第三个参数，信道号
第四个目标的地址,
第五个参数，同步字
*/

void my_cmd_cc1101_tx(uint8_t *pt_cmd,uint8_t cmd_size,uint8_t tx_channel,uint8_t des_address,uint16_t des_SYNC_word)
{
    uint8_t i=0;

    // HAL_NVIC_DisableIRQ(EXTI9_5_IRQn); //关闭中断，不产生发射中断
    CC1101SetIdle(); //进入空闲模式
    CC1101WriteReg(CC1101_CHANNR,tx_channel); //变换信道
    CC1101SetSYNC(des_SYNC_word); //设置同步字


    //CC1101SendPacket_add( INT8U *txbuffer, INT8U size, TX_DATA_MODE mode,INT8U address)
    CC1101SendPacket_add(pt_cmd,cmd_size,ADDRESS_CHECK,des_address);  //发送数据

    //-----测试使用
    printf("TX-data:");
    for(i=0; i<cmd_size; i++)
    {
        printf("[%X]--",pt_cmd[i]);
    }
    printf("\n");

}





extern uint8_t my_CC1101_chip_address; //芯片地址，默认为0x05
extern uint8_t my_cc1101_dest_address; //发射数据目标地址
extern uint16_t my_CC1101_sync_word;  //CC1101同步字，默认为0x8799
extern uint8_t my_CC1101_change_channer;  //动态信道号



uint8_t my_temp=0;
//------测试使用-------------
int j=0;
void RF_SendPacket_cyc(void)
{
    INT8U i=0, length=0, buffer[65]= {0};



    // if ((0==COM_TimeOut) && (COM_RxCounter>0))
    {


        length = 4;

        //COM_RxCounter = 0;

        for (i=0; i<length; i++)
        {
            if(j>25) j=0;
            buffer[i] =j+0X41; //0x41
            j++;

        }
        buffer[i]='\n';
        // CC1101SendPacket(buffer, length+1, ADDRESS_CHECK);    // 发送数据
        CC1101SendPacket_add(buffer, length+1, ADDRESS_CHECK,my_cc1101_dest_address);
        //CC1101SetTRMode(RX_MODE);                           // 进入接收模式

        //DelayMs(250);


    }
}

/*
功能：发送WOR唤醒帧
*/

void my_cmd_cc1101_tx_WakeCmd(void)
{

    uint8_t cmd_lenth=0;
    my_temp++;
    if(MY_CC1101_STATUS==0x01 || MY_CC1101_STATUS== 0x02) return;
    else MY_CC1101_STATUS=0X01;


    //0X68,0X05,0XFE,0X01,0XA0,0XA3,0XC1,0X16
    Cmd_CC1101_TX[0]=0X68;
    Cmd_CC1101_TX[1]=0X05;
    Cmd_CC1101_TX[2]=0XFE;
    Cmd_CC1101_TX[3]=0X01;
    Cmd_CC1101_TX[4]=0XA0;
    Cmd_CC1101_TX[5]=0XA3;
    Cmd_CC1101_TX[6]=my_temp;//0XC1;
    Cmd_CC1101_TX[7]=0X16;

    my_CC1101_change_channer=0x02;
    my_CC1101_sync_word=0x0102;
    my_cc1101_dest_address=0xFE;
    cmd_lenth=8;

    //void my_cmd_cc1101_tx(uint8_t *pt_cmd,uint8_t cmd_size,uint8_t tx_channel,uint8_t des_address,uint16_t des_SYNC_word)
    my_cmd_cc1101_tx(Cmd_CC1101_TX,cmd_lenth,my_CC1101_change_channer,my_cc1101_dest_address,my_CC1101_sync_word);
    //printf("tx=%d\n",my_temp);


    MY_CC1101_STATUS=0x00;

}
/*
功能：发送周期数据
*/

void my_cmd_cc1101_tx_cyc_data(void)
{

    uint8_t cmd_lenth=0;
    my_temp++;

    if(MY_CC1101_STATUS==0x01 || MY_CC1101_STATUS== 0x02) return;
    else MY_CC1101_STATUS=0X01;


    //0X68,0X05,0XFE,0X01,0XA0,0XA3,0XC1,0X16
    Cmd_CC1101_TX[0]=0X68;
    Cmd_CC1101_TX[1]=0X05;
    Cmd_CC1101_TX[2]=0XFE;
    Cmd_CC1101_TX[3]=0X01;
    Cmd_CC1101_TX[4]=0XA0;
    Cmd_CC1101_TX[5]=0XA3;
    Cmd_CC1101_TX[6]=my_temp;//0XC1;
    Cmd_CC1101_TX[7]=0X16;

    my_CC1101_change_channer=0x02;
    my_CC1101_sync_word=0x0102;
    my_cc1101_dest_address=0xFE;
    cmd_lenth=8;

    //void my_cmd_cc1101_tx(uint8_t *pt_cmd,uint8_t cmd_size,uint8_t tx_channel,uint8_t des_address,uint16_t des_SYNC_word)
    my_cmd_cc1101_tx(Cmd_CC1101_TX,cmd_lenth,my_CC1101_change_channer,my_cc1101_dest_address,my_CC1101_sync_word);
    //printf("tx=%d\n",my_temp);


    MY_CC1101_STATUS=0x00;

}
/*
功能：发送突发遥信数据
*/

void my_cmd_cc1101_tx_ALM_single_data(void)
{
    my_cmd_cc1101_tx_cyc_data();  //测试使用，没有意义*********
}

/*
发送突发遥测数据
*/
void my_cmd_cc1101_tx_ALM_multi_data(void)
{
    my_cmd_cc1101_tx_cyc_data();  //测试使用，没有意义*********

}
/*
报警数据发送结束
*/

void my_cmd_cc1101_tx_ALM_end_data(void)
{

    my_cmd_cc1101_tx_cyc_data();  //测试使用，没有意义*********
}

/*
功能：发送OK帧
*/
void my_cmd_cc1101_tx_OK_fram(void)
{


}

/*
功能：响应查询参数，发送参数
*/

void my_cmd_cc1101_tx_config_data(void)
{


}
/*
功能：响应查询录播数据，发送录播数据
*/

void my_cmd_cc1101_tx_record_data(void)
{


}
/*
功能：响应查询录播数据，结束
*/

void my_cmd_cc1101_tx_record_end_data(void)
{


}








