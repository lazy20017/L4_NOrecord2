/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "my_led.h"
#include "my_101.h"
//#include "my_cc1101.h"
#include "my_OS.h"
#include "bsp_iap.h"
#include "my_DAC.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osThreadId myTask08Handle;
osMessageQId myQueue01Handle;
osMessageQId myQueue02Handle;
osTimerId myTimer01Handle;
osMutexId myMutex01Handle;

/* USER CODE BEGIN Variables */
extern uint16_t my_wave_write_add;

extern uint16_t my_dianliu_exit_add;
//extern uint16_t my_Time_Cyc_exit_add;
extern uint8_t my_DC_AC_status;
extern uint8_t my_CC1101_Frame_status;
extern uint8_t my_CC1101_COM_Fram_buf[];
extern uint16_t  my_CC1101_all_step;
extern uint16_t my_PWR_all_step;


extern uint8_t my_IT_status;
extern uint8_t my_IT_Count;
extern  uint8_t my_Fault_Current_End_Status ; //
extern  uint8_t my_Fault_E_Fild_End_Status ; //
extern TIM_HandleTypeDef htim6;
extern float ADC1_Filer_value_buf[];
extern float ADC2_Filer_value_buf_2[ADC2_COLM][3];

uint16_t   my_os_count1 = 0;
uint8_t 	my_exit_status_count = 0;

EventGroupHandle_t xCreatedEventGroup = NULL;
EventGroupHandle_t xCreatedEventGroup2 = NULL;

uint8_t my_use_cyc_rec_data_status = 0; //�Ƿ��������¼�����ݷ��ͣ�1Ϊ���ͣ�0Ϊ������

uint8_t my_use_alarm_rec_data_status = 0; //�Ƿ��������¼�����ݷ��ͣ�1Ϊ���ͣ�0Ϊ������
uint8_t my_use_alarm_rec_data_status_Efild = 0;

uint16_t my_que1_wait_time = 1000; //����1�ĵȴ�ʱ��

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void StartTask08(void const * argument);
void Callback01(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Pre/Post sleep processing prototypes */
void PreSleepProcessing(uint32_t *ulExpectedIdleTime);
void PostSleepProcessing(uint32_t *ulExpectedIdleTime);

/* Hook prototypes */

/* USER CODE BEGIN PREPOSTSLEEP */
__weak void PreSleepProcessing(uint32_t *ulExpectedIdleTime)
{
    /* place for user code */
}

__weak void PostSleepProcessing(uint32_t *ulExpectedIdleTime)
{
    /* place for user code */
}
/* USER CODE END PREPOSTSLEEP */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Create the mutex(es) */
    /* definition and creation of myMutex01 */
    osMutexDef(myMutex01);
    myMutex01Handle = osMutexCreate(osMutex(myMutex01));

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* Create the timer(s) */
    /* definition and creation of myTimer01 */
    osTimerDef(myTimer01, Callback01);
    myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    osTimerStart(myTimer01Handle, 1000); //Ĭ�����õ����ȼ���6��
    //	osTimerStart(myTimer02Handle, 240);
    /* USER CODE END RTOS_TIMERS */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 256);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* definition and creation of myTask02 */
    osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 256);
    myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

    /* definition and creation of myTask03 */
    osThreadDef(myTask03, StartTask03, osPriorityNormal, 0, 256);
    myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

    /* definition and creation of myTask04 */
    osThreadDef(myTask04, StartTask04, osPriorityAboveNormal, 0, 256);
    myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

    /* definition and creation of myTask08 */
    osThreadDef(myTask08, StartTask08, osPriorityIdle, 0, 384);
    myTask08Handle = osThreadCreate(osThread(myTask08), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* Create the queue(s) */
    /* definition and creation of myQueue01 */
    osMessageQDef(myQueue01, 3, uint16_t);
    myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

    /* definition and creation of myQueue02 */
    osMessageQDef(myQueue02, 3, uint16_t);
    myQueue02Handle = osMessageCreate(osMessageQ(myQueue02), NULL);

    /* USER CODE BEGIN RTOS_QUEUES */

    xCreatedEventGroup = xEventGroupCreate();
    xCreatedEventGroup2 = xEventGroupCreate();
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

    /* USER CODE BEGIN StartDefaultTask */
    //����ӵ����ָ�����
    __HAL_GPIO_EXTI_CLEAR_IT(PIN_CC_IRQ);
    HAL_NVIC_EnableIRQ(EXIT_CC_IRQ_GD0); //�����жϣ�����CC1101���ݲ���

    /* Infinite loop */
    for(;;)
    {
        if(my_CC1101_all_step == 0)
        {
            //==CC1101
            my_fun_CC1101_init_resume();
            if(my_CC1101_Sleep_status == 1)
            {
                CC1101SetSleep();
            }

            //====����	==
            my_fun_usart_init_resume(); //����ӵ����ָ�����
        }

        if( my_DTU_send_faile_count >= 3)
        {
            my_fun_CC1101_init_reum();
        }

        osDelay(33000);
    }
    /* USER CODE END StartDefaultTask */
}

/* StartTask02 function */
void StartTask02(void const * argument)
{
    /* USER CODE BEGIN StartTask02 */
    /* Infinite loop */
    //CC1101���ʹ�������

    BaseType_t my_result = 0;
    uint16_t my_step = 0;
    for(;;)
    {
        //==CC1101 �������ݻ��ڣ���Ӧ����1

        my_result = xQueueReceive(myQueue01Handle, &my_step, my_que1_wait_time); //xQueuePeek
        //my_result = xQueueReceive(myQueue01Handle, &my_step, 10000); //xQueuePeek
        if(my_result == pdPASS)
        {
            printf("=========CC1101 CC_T_QU1 IS send=[%X]--------\r\n", my_step);
        }
        else
        {
            my_step = 0X00;

            //printf("QH1 error re = %d, step=%d \r\n",my_result,my_step);

        }




        //====0 DTU ��������--���Ͳ���
        my_fun_CC1101_time_dialog_tx2(my_step, 0xF000, 0xF100, 0, my_fun_CC1101_test1,3);
        my_fun_CC1101_time_dialog_tx2(my_step, 0xF200, 0xF300, 0, my_fun_CC1101_test1,3);
        my_fun_CC1101_time_dialog_tx2(my_step, 0xF400, 0xF500, 0, my_fun_CC1101_test1,3);
        my_fun_CC1101_time_dialog_tx2(my_step, 0xF600, 0xF700, 0, my_fun_CC1101_test1,3);
        my_fun_CC1101_time_dialog_tx2(my_step, 0xF800, 0xF900, 1, my_fun_CC1101_test1,3);


        //=====������������֡�������������
        my_fun_CC1101_time_dialog_tx2(my_step, 0x0000, 0x00E0, 0, my_fun_TX_CC1101_heart,3);  //����
        //==�����������
        my_fun_CC1101_time_dialog_tx2(my_step, 	0x0000, 0x00E1, 0, 	my_fun_TX_CC1101_config,3);  //�����������
        my_fun_CC1101_time_dialog_tx2(my_step,	0x0000,	0X00E2,	1,	my_fun_TX_CC1101_config2,3); //ȷ�����ò���


        //=====2 ������������
        my_fun_CC1101_time_dialog_tx2(my_step, 0x0000, 0x0001, 0, my_fun_TX_CC1101_test0,3);//ң��
        my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0040, 0, my_fun_TX_CC1101_test1,3);//DC����7������
        my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0041, 0, my_fun_TX_CC1101_test2,3);//AC��Чֵ��3���������������糡���벨
        my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0042, 0, my_fun_TX_CC1101_test3,3);//AC12T
//				if(my_use_cyc_rec_data_status==1)
//        my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0043, 0, my_fun_TX_CC1101_test4);//¼��

        //====1 ���ͱ�������
        my_fun_CC1101_time_dialog_tx2(my_step, 0x0000, 0x0002, 0, my_fun_TX_CC1101_test0,2); //ң��
        my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0050, 0, my_fun_TX_CC1101_test1,2); //ң��DC
        my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0051, 0, my_fun_TX_CC1101_test2,2); //ң��AC�����ϵ������糡���벨����Чֵ����3��ֵ
        my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0052, 0, my_fun_TX_CC1101_test3,2); //ң��12TAC
        
				if(my_use_alarm_rec_data_status == 1)
            my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0053, 0, my_fun_TX_CC1101_test4,2); //¼��_����
        if(my_use_alarm_rec_data_status_Efild == 1)
            my_fun_CC1101_time_dialog_tx2(my_step, 0x2000, 0x0054, 0, my_fun_TX_CC1101_test5,2); //¼��_�糡

        //======3 �������ò���



        //LED2_TOGGLE;
        //osDelay(20000);
    }
    /* USER CODE END StartTask02 */
}

/* StartTask03 function */
void StartTask03(void const * argument)
{
    /* USER CODE BEGIN StartTask03 */
    //CC1101���մ�������

    /* Infinite loop */
    BaseType_t my_result;
    uint16_t my_step;
    for(;;)
    {
        //CC1101�������ݻ��ڣ���Ӧ����2
        my_result = xQueueReceive(myQueue02Handle, &my_step, 2000); //xQueuePeek,����2��Ӧ��M35���ն���
        if(my_result == pdPASS)
        {
            printf("-------CC1101 CC_R_QU2 receive=[%X]\r\n", my_step);
        }
        else
        {
            //printf("CC1101 not receive step = %d\r\n",my_step);
            my_step = 0X0000;
        }


        //====DTU��������--���ղ���--

        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0000, 0xF000, 0xF100, 0, my_fun_write_update_data_to_FLASH);
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0000, 0xF200, 0xF300, 0, my_fun_write_update_data_to_FLASH);
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0000, 0xF400, 0xF500, 0, my_fun_write_update_data_to_FLASH);
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0000, 0xF600, 0xF700, 0, my_fun_write_update_data_to_FLASH);
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0000, 0xF800, 0xF900, 0, my_fun_write_update_data_to_FLASH);
        //=====����֡
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x00E0, 0x2000, 0x0000, 1, my_fun_RX_CC1101_text0_RX_OK);
        //==�����������
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x00E1, 0xE100, 0x00E2, 0, my_fun_RX_CC1101_text0_RX_OK);//���յ�������������
        //======���ڽ��ղ���===
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0001, 0x2000, 0x0040, 0, my_fun_RX_CC1101_text0_RX_OK);
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0040, 0x2000, 0x0041, 0, my_fun_RX_CC1101_text0_RX_OK);
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0041, 0x2000, 0x0042, 0, my_fun_RX_CC1101_text0_RX_OK);
//				if(my_use_cyc_rec_data_status==1)
//				{
//				my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0042, 0x2000, 0x0043, 0, my_fun_RX_CC1101_text0_RX_OK);
//				my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0043, 0x2000, 0x0000, 1, my_fun_RX_CC1101_text0_RX_OK);
//				}
//				else
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0042, 0x2000, 0x0000, 1, my_fun_RX_CC1101_text0_RX_OK);


        //======  ��������  ==
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0002, 0x2000, 0x0050, 0, my_fun_RX_CC1101_text0_RX_OK);
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0050, 0x2000, 0x0051, 0, my_fun_RX_CC1101_text0_RX_OK);
        my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0051, 0x2000, 0x0052, 0, my_fun_RX_CC1101_text0_RX_OK);
        if(my_use_alarm_rec_data_status == 1 && my_use_alarm_rec_data_status_Efild == 0)                         //���͵���¼������
        {
            my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0052, 0x2000, 0x0053, 0, my_fun_RX_CC1101_text0_RX_OK);
            my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0053, 0x2000, 0x0000, 1, my_fun_RX_CC1101_text0_RX_OK);
        }
        else if(my_use_alarm_rec_data_status == 1 && my_use_alarm_rec_data_status_Efild == 1)              //���͵��������糡¼������
        {
            my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0052, 0x2000, 0x0053, 0, my_fun_RX_CC1101_text0_RX_OK);
            my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0053, 0x2000, 0x0054, 0, my_fun_RX_CC1101_text0_RX_OK);
            my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0054, 0x2000, 0x0000, 1, my_fun_RX_CC1101_text0_RX_OK);

        }
        else
            my_fun_CC1101_time_dialog_rx2(&myQueue01Handle, my_step, 0x0052, 0x2000, 0x0000, 1, my_fun_RX_CC1101_text0_RX_OK);  //������¼������


        //LED4_TOGGLE
        //osDelay(1000);
    }
    /* USER CODE END StartTask03 */
}

/* StartTask04 function */
void StartTask04(void const * argument)
{
    /* USER CODE BEGIN StartTask04 */
    /* Infinite loop */
    //CC1101�����жϴ���


    uint8_t my_status = 0;
    uint8_t temp8 = 0;
    uint16_t my_step = 0;
    for(;;)
    {
        //CC1101Э��֡��⣬��Ӧ��־��1����0000 1000����λ1��������֡������״̬�����ն���2

        //=========
        EventBits_t	uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* �¼���־���� */
                             0x08,            /* �ȴ�bit0��bit1������ */
                             pdTRUE,             /* �˳�ǰbit0��bit1�������������bit0��bit1�������òű�ʾ���˳���*/
                             pdTRUE,             /* ����ΪpdTRUE��ʾ�ȴ�bit1��bit0��������*/
                             10000); 	 /* �ȴ��ӳ�ʱ�� */

        if((uxBits & 0x08) == 0x08)
        {
            my_status = 1; //ͨ����־�飬��ý��յ����ݵı�־
            //printf("cc1101 receive data !\r\n");
        }
        else
            my_status = 0;
        //=========

        //��CC101���յ��������ƶ�����������
        if(my_status == 1)
        {
            if(RF_RecvHandler_intrrupt_get_data_to_buf() > 0)
                my_status = 1;
            else
                my_status = 0;

        }
        //============

        //�Ի���������֡����
        if(my_status == 1)
        {

            //Э����������һ֡����
            my_101frame_analyse(4, 1, CC1101_CRC_check);
            //=========ȡ������===
            if(my_CC1101_Frame_status > 0) //������������
            {
#if Debug_Usart_out_CC1101_Get_cmd==1
                my_fun_display_fram_16(4);  //����ʹ�ã���ʾ���յ�������
#endif
                my_CC1101_Frame_status = 0;

                if(my_CC1101_COM_Fram_buf[0] == 0x10)
                {
                    my_status = 0x10;
                    temp8 = my_CC1101_COM_Fram_buf[1]; //������Ϊ0X20������OK֡
                }
                else if (my_CC1101_COM_Fram_buf[0] == 0x68) //��֡�Ĺ�����
                {
                    my_status = 0x68;
                    temp8 = my_CC1101_COM_Fram_buf[6]; //
                }
                else
                {
                    my_status = 0;
                    temp8 = 0;
                }
            }
            //====�������������===
            if(temp8 != 0x00)
            {
                printf("CC1101 now_step=[%X], get_step= [%X]\r\n", my_CC1101_all_step, temp8);

            }
            //==1=OK֡�Ի�����
            if( temp8 == 0x20 && my_CC1101_all_step != 0)
            {
                my_step = 0X2000;
                xQueueSend(myQueue02Handle, &my_step, 100);
            }
            //===2====


            //==3 ====�����Ի�����
            else if( temp8 == 0xF0)
            {
                //my_CC1101_all_step=0XF000; //�Ի����̵Ŀ�ʼ״̬
                my_step = 0XF000;
                xQueueSend(myQueue02Handle, &my_step, 100);
            }
            else if( temp8 == 0xF2)
            {
                my_step = 0XF200;
                xQueueSend(myQueue02Handle, &my_step, 100);
            }
            else if( temp8 == 0xF4)
            {
                my_CC1101_all_step = 0XF400;
                my_step = 0XF400;
                xQueueSend(myQueue02Handle, &my_step, 100);
            }
            else if( temp8 == 0xF6)
            {
                my_step = 0XF600;
                xQueueSend(myQueue02Handle, &my_step, 100);
            }
            else if( temp8 == 0xF8)
            {
                my_step = 0XF800;
                xQueueSend(myQueue02Handle, &my_step, 100);
            }
            //��������
            else if(temp8 == 0X3F)
            {
                my_step = 0XE100;
                xQueueSend(myQueue02Handle, &my_step, 100);

            }

            //===

            //===============

            temp8 = 0;
            my_CC1101_Frame_status = 0;
            my_status = 0;

        }
        //osDelay(1);
    }
    /* USER CODE END StartTask04 */
}

/* StartTask08 function */
void StartTask08(void const * argument)
{
    /* USER CODE BEGIN StartTask08 */
    /* Infinite loop */
		
		//STM32�жϴ�������·���ӵأ���ʱ�ж�
	
		//�˲��ֳ�����Ҫ�������У��жϵĴ������̣����������жϣ�Ӳ���жϺͶ�ʱ�жϣ����ж�ʱ�ж�ʱ��ʱ�����д����������ݣ�������Чֵ��������
	
	
    uint8_t my_status = 0;
    uint8_t temp8 = 0;
    uint16_t my_step = 0;
    HAL_NVIC_EnableIRQ(EXIT_dianliu_EXTI_IRQn); //��·�жϿ���@@@
		if(my_use_Jiedi_exit_status==1)
    HAL_NVIC_EnableIRQ(EXIT_jiedi_EXTI_IRQn); //�ӵ��жϿ���
		else
		HAL_NVIC_DisableIRQ(EXIT_jiedi_EXTI_IRQn);
		
    HAL_TIM_Base_Start_IT(&htim6);  //����tim6��ʱ��,1s
    float my_temp_double = 0;
    for(;;)
    {

        EventBits_t	uxBits = xEventGroupWaitBits(xCreatedEventGroup2, /* �¼���־���� */
                             0x03,            /* �ȴ�bit0������ */
                             pdTRUE,             /* �˳�ǰbit0��bit1�������������bit0��bit1�������òű�ʾ���˳���*/
                             pdFALSE,             /* ����ΪpdTRUE��ʾ�ȴ�bit1��bit0��������*/
                             100); 	 /* �ȴ��ӳ�ʱ�� */

        if((uxBits & 0x01) == 0x01) //��·�жϣ���־
        {
            my_status = 1; //ͨ����־�飬��ý��յ����ݵı�־
            //printf("cc1101 receive data my_status1=%d\r\n",my_status);
        }
        else if((uxBits & 0x02) == 0x02)
        {
            my_status = 2;
						 //printf("cc1101 receive data my_status2=%d\r\n",my_status);
        }
        else
            my_status = 0;
        //======�жϴ���===
        if(my_status == 1)
        {
            //�����ж��жϣ����������оݣ��оݻ��ȫ���ó����ۺ󣬽���Ի���Ϣ���ͻ���
            //�����ͬ��¼�����������̷���ͬ���źţ�Ȼ��ʼ�Լ�¼�����д���֤
            temp8 = my_fun_current_exit_just(); //1Ϊ��ʾ¼�������ˣ�0Ϊû�н���
        }

        if(my_IT_status == 1)
        {
            temp8 = my_fun_current_exit_just(); //1Ϊ��ʾ¼�������ˣ�0Ϊû�н���

            if(temp8 == 1 && (my_Fault_Current_End_Status != 0 || my_Fault_E_Fild_End_Status != 0)) //������������
            {
								
								
							
                //�����жϺ󣬰Ѷ�Ӧ��DACֵ�趨Ϊ600A
                my_temp_double = ADC2_Filer_value_buf_2[0][1];
                ADC2_Filer_value_buf_2[0][1] = 600;
                my_ADC_Count_old = my_ADC_Count - 1;
                my_fun_Set_DAC_I_ref();//DAת��
                ADC2_Filer_value_buf_2[0][1] = my_temp_double;
								osDelay(3);

								//��ͬ����������������ʱ����
								
								osDelay((my_CC1101_chip_address-1)*30*1000);
                //�жϴ��������ˣ��ָ��жϿ���
                HAL_NVIC_ClearPendingIRQ(EXIT_dianliu_EXTI_IRQn);
                __HAL_GPIO_EXTI_CLEAR_FLAG(EXIT_dianliu_Pin);
                __HAL_GPIO_EXTI_CLEAR_IT(EXIT_dianliu_Pin);

                HAL_NVIC_ClearPendingIRQ(EXIT_jiedi_EXTI_IRQn);
                __HAL_GPIO_EXTI_CLEAR_FLAG(EXIT_jiedi_Pin);
                __HAL_GPIO_EXTI_CLEAR_IT(EXIT_jiedi_Pin);


               HAL_NVIC_EnableIRQ(EXIT_dianliu_EXTI_IRQn); //��·�жϿ���@@@
               		if(my_use_Jiedi_exit_status==1)
								HAL_NVIC_EnableIRQ(EXIT_jiedi_EXTI_IRQn); //�ӵ��жϿ���
								else
								HAL_NVIC_DisableIRQ(EXIT_jiedi_EXTI_IRQn);

                //�������ݷ��ͻ���@@@@@@@
								printf("interrrupt is alarm\n");
								temp8=0;
                my_zsq_ALarm_send_status = 1; //��������״̬Ϊ1����������
                my_step = 0X0002; //0X0200
                xQueueSend(myQueue01Handle, &my_step, 100);
            }

            else if(temp8 == 1 && my_Fault_Current_End_Status == 0 && my_Fault_E_Fild_End_Status == 0) //�жϵ���û�б���
            {
							
							
                my_temp_double = ADC2_Filer_value_buf_2[0][1];
                ADC2_Filer_value_buf_2[0][1] = 600;
                my_ADC_Count_old = my_ADC_Count - 1; //�趨�仯���������Խ����趨DAC
                my_fun_Set_DAC_I_ref();//DAת��
                ADC2_Filer_value_buf_2[0][1] = my_temp_double;
								osDelay(3);
								//��ͬ����������������ʱ����
								osDelay((my_CC1101_chip_address-1)*30*1000);
                //�жϴ��������ˣ��ָ��жϿ���
                HAL_NVIC_ClearPendingIRQ(EXIT_dianliu_EXTI_IRQn);
                __HAL_GPIO_EXTI_CLEAR_FLAG(EXIT_dianliu_Pin);
                __HAL_GPIO_EXTI_CLEAR_IT(EXIT_dianliu_Pin);

                HAL_NVIC_ClearPendingIRQ(EXIT_jiedi_EXTI_IRQn);
                __HAL_GPIO_EXTI_CLEAR_FLAG(EXIT_jiedi_Pin);
                __HAL_GPIO_EXTI_CLEAR_IT(EXIT_jiedi_Pin);


                HAL_NVIC_EnableIRQ(EXIT_dianliu_EXTI_IRQn); //��·�жϿ���@@@@@
                if(my_use_Jiedi_exit_status==1)
								HAL_NVIC_EnableIRQ(EXIT_jiedi_EXTI_IRQn); //�ӵ��жϿ���
								else
								HAL_NVIC_DisableIRQ(EXIT_jiedi_EXTI_IRQn);


                //�趨ģ�ⱨ������
                my_Fault_Current_End_Status = 0XF2;  //�жϣ����ǲ��Ǳ���,�����ڻ�����·�ָ�����

                //�������ݷ��ͻ���@@@@@@@
								printf("interrrupt is alarm--NO\n");
								temp8=0;
                my_zsq_ALarm_send_status = 1; //��������״̬Ϊ1����������
                my_step = 0X0002; //0X0200
                xQueueSend(myQueue01Handle, &my_step, 100);
            }


        }
        //=====���ڴ���===�˲������������ж�¼�����ݽ�����Чֵ����
        if(my_status == 2)
        {

            //��ʱһ��ʱ�����¼�����㣬�������ڷ���ʱ�䵽���м���
            my_adc_1_convert(); //����ֱ��ֵ��ADC1��7��������
#if USE_olde_12T_data_cyc==0
            my_fun_wave_rec();  //1�����嵽2�����壬�ȴ�ʱ�䣬��������ֵ����12T���
#else
            my_fun_wave1_to_wave2_old_data();//��ʷ����
#endif

            my_adc2_convert2(0); //����12�����ڵ�ÿ�����ڵ�3����������12�����ڵ��ۼ�ƽ��ֵ
            //my_Time_Cyc_exit_Status=0;

        }



        //osDelay(1000);
    }
    /* USER CODE END StartTask08 */
}

/* Callback01 function */
void Callback01(void const * argument)
{
    /* USER CODE BEGIN Callback01 */

    my_os_count1++; //1���ӣ��ж�һ��



#if ADC_CYC_data==1  //�������ڲ���
    if(my_os_count1 % (30) == 0 && my_os_count1 != 0)
    {
        //my_fun_give_Queue(&myQueue01Handle, 0X0001); //������������

    }

#endif

#if DAC_auto_change_on==1
    if(my_os_count1 % (my_DAC_cyc_time) == 0 || my_os_count1 == 1)
    {

        if(my_CC1101_all_step == 00 && my_E_Field_exit_Status == 0 && my_Current_exit_Status == 0 && my_Time_Cyc_exit_Status == 0)
        {
            printf("\n==DAC Vref CC1101_all_step=[%X],A_EXIT_status=%d,E_exit_status=%d,cyc_exit_status=%d\n", my_CC1101_all_step, my_E_Field_exit_Status, my_Current_exit_Status, my_Time_Cyc_exit_Status);
            //printf("=====DAC Vref====\n");
            my_fun_Set_DAC_I_ref();//DAת��
            my_fun_get_Line_stop_Efild(); //��̬�ж�@@@@@@@@@@@@@@@@@@2
            printf("----after DAC CC1101_STEP=[%X]---\n", my_CC1101_all_step);
        }
    }
#endif


#if OS_heap_high_water_data==1
    if(my_os_count1 % (437) == 0 && my_os_count1 != 0)
    {
        if(my_CC1101_all_step == 00 && my_E_Field_exit_Status == 0 && my_Current_exit_Status == 0 && my_Time_Cyc_exit_Status == 0)
        {
            my_fun_task_heap_value();
        }

    }
#endif

    if(my_os_count1 % (13) == 0 )
    {
        printf("\n==ZSQ[%d]--yongsai control[%d]===\n",my_CC1101_chip_address, my_os_count1);
        my_adc_1_convert_dis(0); //��ʾֱ��ֵ
        //==CC1101�͹��Ŀ��Ʋ���
        if(ADC1_Filer_value_buf[6] >= (float)4.0)
        {
            my_CC1101_Sleep_status = 0;
            if(my_CC1101_all_step == 0x00 && my_DTU_send_faile_count >= 3) //my_DTU_send_faile_count��ʾCC1101��DTUͨ��ʧ�ܶ��
            {
                CC1101SetSleep();
            }

        }
        else
        {
            my_CC1101_Sleep_status = 1;
            if(my_CC1101_all_step == 0x00 && my_DTU_send_faile_count >= 3)
            {
                CC1101SetSleep();
            }

        }

        //===ӵ���������ָ�ԭʼ״̬
        if(my_E_Field_exit_Status == 1 || my_Current_exit_Status == 1 || my_Time_Cyc_exit_Status == 1)
        {

            my_exit_status_count++;
        }
        else
        {

            my_exit_status_count = 0;
        }
        if(my_exit_status_count >= 3)
        {
            my_E_Field_exit_Status = 0; //
            my_Current_exit_Status = 0;
            my_exit_status_count = 0;
            my_Time_Cyc_exit_Status = 0;
            my_CC1101_all_step = 0;
            my_sys_start_status = 0;
        }
        //=============

        //﮵�س�����
        if(ADC1_Filer_value_buf[6] > (float)4.2)
        {
            //EN25505_OFF;  //�ض�﮵�س���·

        }
        else
        {
            //EN25505_ON;  //��﮵�ع����·

        }

        //����ȡ���������
        if(my_os_count1 % 3607 == 0)
        {
            CT_to_BQ25505_ON;  //CT��BQ25���
            HAL_Delay(500);
            CT_to_BQ25505_OFF;
            HAL_Delay(500);
            CT_to_BQ25505_ON;
        }
        //25505оƬ�Ĺ�������
        if(my_os_count1 % (3673) == 0 )
        {

            EN25505_OFF;
            HAL_Delay(1000);
            EN25505_ON;//BQ25505������Ĭ�ϣ�

        }
				//���Ϸŵ����
				if(ADC2_Filer_value_buf_2[0][1]>300 && my_Line_short_status==1)
				{
					CT_Source_short_ON; //��·�ŵ� 10���ӣ����10����
					
					if(my_Line_short_count>600) //��·�ŵ�10����
					{
						my_Line_short_status=0;
						my_Line_short_count=0;
					}
					
				}
				else if(ADC2_Filer_value_buf_2[0][1]>300 && my_Line_short_status==0)
				{
					CT_Source_short_OFF;//����·�ŵ� 
				
					if(my_Line_short_count>600) //����·�����10����
					{
						my_Line_short_status=1;
						my_Line_short_count=0;
					}
					
					
				}
				if(ADC2_Filer_value_buf_2[0][1]<280)
				{
					CT_Source_short_OFF;//����·�ŵ�
					my_Line_short_status=0;
				}
       //====================

        //��ʼ��״̬����
        if(my_sys_start_status == 1)
        {

            printf("==start init [%d]==\n", my_sys_start_status);
            BaseType_t xResult;
            BaseType_t xHigherPriorityTaskWoken = pdFAIL;
            my_Time_Cyc_exit_Status = 1;
            my_dianliu_exit_add = my_wave_write_add; //��ǰ¼����ַ
            my_Time_Cyc_exit_add = my_wave_write_add;
            xResult =	xEventGroupSetBitsFromISR(xCreatedEventGroup2, 0X02, &xHigherPriorityTaskWoken); //�����ж��¼�
            if(xResult != pdFAIL)
            {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
            HAL_Delay(3000); //�ȴ�¼��

            my_tim6_count = 0;
            my_fun_give_Queue(&myQueue01Handle, 0X0001); //������������
            //my_sys_start_status = 0;//@@@@@ָʾ����������ʼ���ı�־��

        }




    }

    //���ڱ���
		#if USER_simulate_alarm_status==1
    if(my_os_count1 % (61*6 + (my_CC1101_chip_address-1) * 15) == 0 && my_zsq_ALarm_send_status == 0 && my_os_count1 != 0)
    {
        printf("===send simulation alarm data--1!!!\n");
        fun_wave2_to_wave3(); //���������е��������ݷ��뵽���������У�������ֵ
        my_cyc_alarm_status = 1;
        my_zsq_ALarm_send_status = 1;
				my_Fault_Current_End_Status=0XF0;
        my_fun_give_Queue(&myQueue01Handle, 0X0002); //����ģ�ⱨ��

        //LED2_TOGGLE;
    }
		#endif 

    //�ظ����ͱ�������
    if(my_os_count1 % (38+ (my_CC1101_chip_address-1) * 15) == 0 && my_CC1101_all_step == 0x00 && my_zsq_ALarm_send_status == 1 && my_os_count1 != 0)
    {
        printf("==cyc send alarm data!!!--2\n");
        my_fun_give_Queue(&myQueue01Handle, 0X0002); //���ͱ���
    }

//		//���ò�������
//		if(my_os_count1 % (23) ==0 && my_CC1101_all_step==0x00)
//		{
//			printf("==send config parameter��my_os_count1=%d !!!--2\n",my_os_count1);
//			my_fun_give_Queue(&myQueue01Handle, 0X00E1); //���ͱ���
//		}



    /* USER CODE END Callback01 */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/