/*
 * transport_usart1.c
 *
 *  Created on: 2012-9-21
 *      Author: YangZhiyong
 */


#include "service/launcher.h"
#include "transport/transport_if.h"
#include "hal_usart.h"


static enum TRANSPORT_Status status;


#define TP_USART1_FLAG_RECVDONE      0x01
#define TP_USART1_FLAG_SENDDONE      0x02
#define TP_USART1_FLAG_ERROR         0x80

APP_FLAG_DEFINE(TP_USART1_Flags, 0)

static void (*RecvDone_Handler)(void) = 0;
static void (*SendDone_Handler)(void) = 0;


static enum TRANSPORT_Status getStatus(void)
{
    return status;
}



static uint32_t recv(uint8_t* buffer, uint32_t max_length)
{
    OS_ERR error;
    uint32_t recv_length;

    recv_length = USART1_Recv(buffer, max_length);

    if( USART1_DataAvailable() == 0 )
    {
        OSFlagPost(     &TP_USART1_Flags,
                        TP_USART1_FLAG_RECVDONE,
                        OS_OPT_POST_FLAG_CLR,
                        &error);
    }

    if( status == TRANSPORT_Status_RecvDone )
    {
        status = TRANSPORT_Status_Idle;
    }

    return recv_length;
}






static void onRecvDone(void)
{
    OS_ERR error;

    if( RecvDone_Handler != 0 )
        RecvDone_Handler();

    OSFlagPost(     &TP_USART1_Flags,
                    TP_USART1_FLAG_RECVDONE,
                    OS_OPT_POST_FLAG_SET,
                    &error);

    status = TRANSPORT_Status_RecvDone;
}



static uint32_t send(uint8_t* buffer, uint32_t length)
{
    OS_ERR error;

    OSFlagPost(     &TP_USART1_Flags,
                    TP_USART1_FLAG_SENDDONE,
                    OS_OPT_POST_FLAG_CLR,
                    &error);

    status = TRANSPORT_Status_SendBusy;

    return USART1_Send(buffer, length);
}



static uint32_t flush(void)
{
    return USART1_Flush();
}


static void onSendDone(void)
{
    OS_ERR error;

    if( SendDone_Handler != 0 )
        SendDone_Handler();

    OSFlagPost(     &TP_USART1_Flags,
                    TP_USART1_FLAG_SENDDONE,
                    OS_OPT_POST_FLAG_SET,
                    &error);

    status = TRANSPORT_Status_SendDone;
}



static uint8_t init(void* config)
{
    (void)config;

    USART1_SetRecvTimeoutISR(onRecvDone);
    USART1_SetSendDoneISR(onSendDone);
    status = TRANSPORT_Status_Idle;

    return true;
}



static void setEventHandler(enum TRANSPORT_Event event, void (*handler)(void) )
{
    switch( event )
    {
    case TRANSPORT_Event_RecvDone:
        RecvDone_Handler = handler;
        break;

    case TRANSPORT_Event_SendDone:
        SendDone_Handler = handler;
        break;

    case TRANSPORT_Event_Error:

        break;

    default:
        break;
    }
}



static enum TRANSPORT_Event waitEventTrigger(enum TRANSPORT_Event event, uint32_t timeout)
{
    OS_ERR error;
    OS_FLAGS flags = 0;
    enum TRANSPORT_Event ret_event;
    uint32_t tick_timeout;

    tick_timeout = (timeout*OSCfg_TickRate_Hz)/1000;
    if( (timeout != 0) && (tick_timeout == 0) )
    {
        tick_timeout = 1;
    }

    switch( event )
    {
    case TRANSPORT_Event_RecvDone:
        flags = OSFlagPend( &TP_USART1_Flags,
                        TP_USART1_FLAG_RECVDONE,
                        tick_timeout,
                        OS_OPT_PEND_FLAG_SET_AND,
                        (void*)0,
                        &error  );
        break;

    case TRANSPORT_Event_SendDone:
        flags = OSFlagPend( &TP_USART1_Flags,
                        TP_USART1_FLAG_SENDDONE,
                        tick_timeout,
                        OS_OPT_PEND_FLAG_SET_AND,
                        (void*)0,
                        &error  );
        break;

    case TRANSPORT_Event_Error:
        flags = OSFlagPend( &TP_USART1_Flags,
                        TP_USART1_FLAG_ERROR,
                        tick_timeout,
                        OS_OPT_PEND_FLAG_SET_AND,
                        (void*)0,
                        &error  );
        break;

    default:
        break;
    }

    if( flags == 0 )
    {
        ret_event = TRANSPORT_Event_Timeout;
    }
    else
    {
        ret_event = event;
    }

    return ret_event;
}



const struct TRANSPORT_IF TP_USART1 =
{
                .name       = "USART1",
                .init       = init,
                .send       = send,
                .recv       = recv,
                .flush      = flush,
                .status     = getStatus,
                .setEventHandler    = setEventHandler,
                .waitEventTrigger   = waitEventTrigger,
};



