/* 
* bayang_protocol.cpp
*
* Created: 15.01.2018 11:07:08
* Author: USER
*/


#include "bayang_protocol.h"
//объявление функций системного таймера

#define DEBUG

static uint8_t Bayang_rf_chan = 0;
static uint8_t Bayang_rf_channels[BAYANG_RF_NUM_CHANNELS] = {0,};
static uint8_t Bayang_rx_tx_addr[BAYANG_ADDRESS_LENGTH];
char aux[AUXNUMBER];
uint32_t lastRxTime;
bool timingFail;
uint8_t skipChannel;
static uint8_t* pPacket,* pTransmitterId;

void Bayang_init(uint8_t* p_packet,uint8_t* p_trans_id)
{
	pPacket = p_packet;
	pTransmitterId = p_trans_id;
    HW_DELAY_MS(BAYANG_INITIAL_WAIT);
    const uint8_t bind_address[] = {0,0,0,0,0};
    NRF24L01_Initialize();
    HW_DELAY_MS(20);
    NRF24L01_SetTxRxMode(RX_EN);
    HW_DELAY_MS(20);
    XN297_SetTXAddr(bind_address, BAYANG_ADDRESS_LENGTH);
    XN297_SetRXAddr(bind_address, BAYANG_ADDRESS_LENGTH);
    HW_DELAY_MS(20);
    NRF24L01_FlushTx();
    HW_DELAY_MS(20);
    NRF24L01_FlushRx();
    HW_DELAY_MS(20);
    NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);      // No Auto Acknowldgement on all data pipes
	
    NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x01); //enable pipe 0
    NRF24L01_WriteReg(NRF24L01_11_RX_PW_P0, BAYANG_PACKET_SIZE); // rx pipe 0 set data size
    NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03);   // address size
    NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x00); // no retransmits
    NRF24L01_SetBitrate(NRF24L01_BR_1M);             // 1Mbps
    NRF24L01_SetPower(RF_POWER);                   // set power amp power
    NRF24L01_Activate(0x73);                         // Activate feature register
    HW_DELAY_MS(20);
    NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x00);      // Disable dynamic payload length on all pipes
    NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x01);
    NRF24L01_Activate(0x73);
    HW_DELAY_MS(150);
    XN297_Configure(_BV(NRF24L01_00_EN_CRC) | _BV(NRF24L01_00_CRCO) | _BV(NRF24L01_00_PWR_UP) | _BV(NRF24L01_00_PRIM_RX));
    NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);
    HW_DELAY_MS(20);
    NRF24L01_FlushRx();
#ifdef DEBUG    
DEBUGPRINT("\nBayang_init done");  
DEBUGPRINT("\n----------------\n"); 
DEBUGPRINT("Status: " );
     uint8_t status = NRF24L01_ReadReg(NRF24L01_07_STATUS);
DEBUGPRINT(status );
DEBUGPRINT('\n');
DEBUGPRINT("0x0F Addr: " );
	status = NRF24L01_ReadReg(NRF24L01_0F_RX_ADDR_P5);
DEBUGPRINT(status  ); 
DEBUGPRINT('\n');
#endif    
}

static int nonBindPackets = 0;
static int wrongBindPackets = 0;
static int badPackets = 0;
static int bindPackets = 0;

void Bayang_bind()
{
#ifdef DEBUG
	DEBUGPRINT("Bayang_bind starting\n");
 
#endif      

    bindPackets = 0;
    uint8_t bind_packet[BAYANG_PACKET_SIZE] = {0};
    uint32_t timeout;
    uint32_t consecutiveNoData = 0;

    NRF24L01_WriteReg(NRF24L01_05_RF_CH, BAYANG_RF_BIND_CHANNEL);
	HW_DELAY_MS(15);

    while(bindPackets < 1200 && consecutiveNoData < 1000) {
        timeout = 0;        

        while(timeout < 10) 
        {
            HW_DELAY_MS(1);
			timeout++;
            if(NRF24L01_ReadReg(NRF24L01_07_STATUS) & _BV(NRF24L01_07_RX_DR)) 
            { 
                // data received from tx
				//DEBUGPRINT("rec pacet\n");
                consecutiveNoData = 0;
				
                XN297_ReadPayload(pPacket, BAYANG_PACKET_SIZE);               
                NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);
                NRF24L01_FlushRx();

                if (checkPacket(pPacket))
                {
                  if(pPacket[0] == 0xA4)
                  {
                    if (0 == bindPackets)
                    {
                      memcpy(bind_packet, pPacket, BAYANG_PACKET_SIZE);
                      bindPackets++;
                    }
                    else
                    {
                      if (0 == memcmp(bind_packet, pPacket, BAYANG_PACKET_SIZE))
                      {
                        bindPackets++;  
                      }
                      else
                      {
                        wrongBindPackets++;  
                      }
                    }
                  }
                  else
                  {
                    nonBindPackets++;  
                  }
                }
                else
                {
                  badPackets++;
                }
                break;
            }
            else
            {
              if (bindPackets > 0) //bind procedure has been initiated at this point.
              {
                consecutiveNoData++;
                if(consecutiveNoData > 1000) break; //communication ended, exit bind loop            
              }
            }
        }
    }
    
#ifdef DEBUG
//     Serial.println();
DEBUGPRINT("\nnonBindPackets: ");
DEBUGPRINT(nonBindPackets);
DEBUGPRINT("\nwrongBindPackets: ");
DEBUGPRINT(wrongBindPackets);
DEBUGPRINT("\nbadPackets: ");
DEBUGPRINT(badPackets);
DEBUGPRINT("\nbindPackets: ");
DEBUGPRINT(bindPackets);
#endif

    memcpy(Bayang_rx_tx_addr, &pPacket[1], 5);
    memcpy(Bayang_rf_channels, &pPacket[6], 4);
    //TODO: what use of trId? why not reading pPacket[12] also?
    pTransmitterId[0] = pPacket[10];
    pTransmitterId[1] = pPacket[11];

    //TODO: not sure we need that
    XN297_SetTXAddr(Bayang_rx_tx_addr, BAYANG_ADDRESS_LENGTH);
    XN297_SetRXAddr(Bayang_rx_tx_addr, BAYANG_ADDRESS_LENGTH);

    setNextChannel();
}

static int normalPacketsRec = 0;

void Bayang_recv_packet(T_CONTROLDATA* data)
{
	static uint32_t delta;
    data->throttle = data->yaw = data->pitch = data->roll = 0;
    
    if(NRF24L01_ReadReg(NRF24L01_07_STATUS) & _BV(NRF24L01_07_RX_DR)) 
    { 
      // data received from tx
       uint32_t recTime = micros();
      int sum = 0;
      XN297_ReadPayload(pPacket, BAYANG_PACKET_SIZE);
	  NRF24L01_FlushRx();
      if (checkPacket(pPacket))
      {
        if (pPacket[0] == 0xA5)
        {
          normalPacketsRec++;
          //TODO: control checksum OK
		  //TODO: возможно можно просто преобразовать пакет в структуру
		  
          data->roll = (pPacket[4] & 0x0003) * 256 + pPacket[5];
          data->pitch = (pPacket[6] & 0x0003) * 256 + pPacket[7];
          data->yaw = (pPacket[10] & 0x0003) * 256 + pPacket[11];
          data->throttle = (pPacket[8] & 0x0003) * 256 + pPacket[9];
  
          data->trims[0] = pPacket[6] >> 2;
          data->trims[1] = pPacket[4] >> 2;
		  // throttle and yaw trims
          data->trims[2] = pPacket[8] >> 2; 
          data->trims[3] = pPacket[10] >> 2;
  
//           aux[CH_INV] = (pPacket[3] & 0x80)?1:0; // inverted flag
//           aux[CH_FLIP] = (pPacket[2] & 0x08) ? 1 : 0;
//           data-> aux1 = aux[CH_EXPERT] = (pPacket[1] == 0xfa) ? 1 : 0;
//           aux[CH_HEADFREE] = (pPacket[2] & 0x02) ? 1 : 0;
//           aux[CH_RTH] = (pPacket[2] & 0x01) ? 1 : 0; // rth channel
	      data->ch_inv = (pPacket[3] & 0x80)?1:0; // inverted flag
          data->ch_flip = (pPacket[2] & 0x08) ? 1 : 0;
          data->ch_expert = (pPacket[1] == 0xfa) ? 1 : 0;
          data->ch_headfree = (pPacket[2] & 0x02) ? 1 : 0;
          data->ch_rth = (pPacket[2] & 0x01) ? 1 : 0; // rth channel	  
		  
  
          //all good
          lastRxTime = recTime;
          timingFail = 0;
          skipChannel = 0;
        }
        //else check if this is bind packet from the same transmitter and rebind ??? is this technical possible?
        //as bind works on channel 0, but now we are not on this channel
      }
      
      setNextChannel();

      if (skipChannel < 5)
      {
        delta = recTime - lastRxTime ;
        if (!timingFail && delta > 500 && (delta - 250)/3000 >= (skipChannel + 1) ) 
        {
          setNextChannel();
          skipChannel++;
        }
      }
/*
 * if too much time there is no data, switch to channel 0 and rebind? with same tranmitter id only?
      // sequence period 12000*/  

       
   }  

}

void setNextChannel(void) 
{
  normalPacketsRec = 0;
  Bayang_rf_chan++;
  Bayang_rf_chan %= sizeof(Bayang_rf_channels);
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, Bayang_rf_channels[Bayang_rf_chan]);
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);
  NRF24L01_FlushRx();  
}

bool checkPacket(uint8_t *packet)
{
  int a = 0;
  for(int i=0; i<14; i++) 
  {
    a += packet[i];
  }

  return (a & 0xFF) == packet[14];
}

