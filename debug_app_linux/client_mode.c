#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "General/types.h"
#include "mpers/Location.h"
#include "mpers/DynamicVitalsMsg.h"
#include "mpers/CriticalAlertMsg.h"
#include "mpers/HaloMsgHelperFunctions.h"
#include "mpers/HaloMessageTypes.h"
#include "UdpLib/halo_udp_comm.h"
#include "UdpLib/myHaloUdp.h"
#include "client_tests.h"

static uint8 runThreads  = 1;
static uint8 clientDebug = 0;
static HaloUdpUserData haloUdpUserData;

void client_myhalo_udp_msg_rcvd(void *data)
{
    //HaloMessage *msg = (HaloMessage *) data;
    HaloUdpEventData *rcvEventData = (HaloUdpEventData *) data;
    HaloMessage *msg = (HaloMessage *) rcvEventData->data;
    int msgLength = rcvEventData->dataLength;

    if (msg->commandType == ALL_DATA_DYNAMIC)
    {
        DynamicVitalsMsg dynamicVitalsMsg;

        //Read the structure correctly
        unpack_DynamicVitalsMsg(msg, msgLength, &dynamicVitalsMsg);

        if (clientDebug)
        {
            printf("Dynamic Vitals msg received!\n");
            printf("Step sample Rate: %d\n", dynamicVitalsMsg.stepData.sampleRate);
            printf("Activity sample Rate: %d\n", dynamicVitalsMsg.activityData.sampleRate);
            printf("SrcIp: 0x%x\n", rcvEventData->socketAddress.address);
            printf("SrcPort: %d\n", rcvEventData->socketAddress.port);
        }
    }
    else if (msg->commandType == CRITICAL_ALERT)
    {
        if (clientDebug)
        {
            CriticalAlertMsg *criticalAlertMsg = (CriticalAlertMsg *) msg;
            printf("Critical Alert msg Received!\n");
            printf("Critical Alert type: %d\n", criticalAlertMsg->criticalAlertType);
            printf("Location Size: %lu\n", sizeof(Location));
            printf("HaloMessage Size: %lu\n", sizeof(HaloMessage));
            printf("CriticalAlert Size: %lu\n", sizeof(CriticalAlertMsg));
        }
    }
}

void client_myhalo_udp_msg_sent(void *data)
{
    //HaloUdpEventData *rcvEventData = (HaloUdpEventData *) data;
    //HaloMessage *msg = (HaloMessage *) rcvEventData->data;
    HaloMessage *msg = (HaloMessage *) data;

    if (msg->commandType == ALL_DATA_DYNAMIC)
    {
        if (clientDebug)
        {
            printf("Dynamic Vitals msg sent successfully!\n");
        }
    }
    else if (msg->commandType == CRITICAL_ALERT)
    {
        if (clientDebug)
            printf("Critical Alert msg sent successfully!\n");
    }
}

void client_myhalo_udp_msg_dropped(void *data)
{
    HaloMessage *msg = (HaloMessage *) data;

    if (msg->commandType == ALL_DATA_DYNAMIC)
    {
        ////Set this up to never give up
        //halo_msg_send(data);

        if (clientDebug)
            printf("Dynamic Vitals msg dropped!\n");
    }
}

void * testThreadHandler(void *arg)
{
    while (runThreads)
    {
        printTestItemMenu(&packetTesting);
        getTestItemChoice(&packetTesting);
    }

    return NULL;
}

void run_client(unsigned char debug, char *hostname, char *port)
{
    uint8 actAsServer = 0;
    pthread_t testThr;

    clientDebug = debug;
    if (pthread_create(&testThr, NULL, &testThreadHandler, NULL))
    {
        printf("Could not create thread\n");
        exit(-1);
    }

    //Sets up Notifiers
    haloUdpUserData.actAsServer     = actAsServer;
    haloUdpUserData.debug           = debug;
    haloUdpUserData.hostname        = hostname;
    haloUdpUserData.port            = port;
    haloUdpUserData.msg_rx_received = client_myhalo_udp_msg_rcvd;
    haloUdpUserData.msg_tx_sent     = client_myhalo_udp_msg_sent;
    haloUdpUserData.msg_tx_dropped  = client_myhalo_udp_msg_dropped;

    halo_msg_init(&haloUdpUserData);

    while (runThreads)
    {
        halo_msg_tick();
        usleep(2000);
    }

    runThreads = 0;

    if (pthread_join(testThr, NULL))
    {
        printf("Could not join thread\n");
        exit(-1);
    }
}
