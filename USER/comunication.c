#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f10x.h"
#include "main.h"
#include "nd_os.h"

void normal_Sym_Join_Req(void);
void EEG_response(void);

uint8_t Mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
uint32_t LineSum = 0;
uint8_t SN[2] = {0xAB,0xCD};
uint8_t join_Flag;
float Data[500] = {0x00};
Stimulate StimulateData;
int join_F = 0;
extern short PCOut;
unsigned char *mem_unpack;
ND_OS_MSG *msg_unpack;
ND_OS_ERR err=NONE;


typedef struct
{
	uint8_t Source;
	uint8_t destination;
}ShortAddress;

typedef struct
{
	uint8_t Source;
	uint8_t destination;
}Port;

typedef struct
{
	uint8_t seq;
	uint8_t type;
	uint8_t try_num;
	uint8_t lengthH;
	uint8_t lengthL;
}ALHeader;
Port InPort;
Port OutPort;
ShortAddress OutAddress;
ALHeader OutALHeader;
uint8_t * pHeader;
uint8_t * pEnd;
int test111=0;
typedef struct
{
	uint8_t checksumH;
	uint8_t checksumL;
}checksum;

ALHeader InALHeader;
void ExtrData(uint8_t * ExData)
{
	ND_OS_ERR err = NONE;
	switch(InPort.destination)
  {
		case 0:
    {
			if(InALHeader.lengthL==0)
			{
			  normal_Sym_Join_Req();
					//join_F = 1;
			}
			else
      {
				memcpy(&OutAddress, ExData,2);
				//join_F = 1;
			}
						
		}break;
		case 1:
    {
			
			memcpy(&LineSum, ExData,sizeof(LineSum));
		}break;
		case 2:
    {
			memcpy(&(StimulateData.Current), ExData,sizeof(StimulateData.Current));
			ExData++;
			memcpy(&(StimulateData.Freq), ExData,sizeof(StimulateData.Freq));
			ExData = ExData+sizeof(StimulateData.Freq);
			memcpy(&(StimulateData.Pwper), ExData,sizeof(StimulateData.Pwper));
			ExData = ExData+sizeof(StimulateData.Pwper);
			memcpy(&(StimulateData.BurstDuration), ExData,sizeof(StimulateData.BurstDuration));
			ExData = ExData+sizeof(StimulateData.BurstDuration);
			StimulateData.St_Flag = *ExData;			
			
			memcpy(mem_unpack,&StimulateData,sizeof(StimulateData));
			
			//msg_unpack -> MsgSize = (NBRMSG)sizeof(StimulateData);
			
			ND_OS_MSG *msg = ND_OSMsgGet(Msg_Pool_Table,&err);
			
			msg -> MsgPtr = msg_unpack ->MsgPtr;
			msg -> MsgSize = (NBRMSG)sizeof(StimulateData);
			
			msg_unpack -> MsgPtr = NULL;
			
			ND_OSTaskQPost(Stimulate_Q,
                     ND_OS_QUEUE_TYPE_STIMULATE,
                     (ND_OS_MSG *)msg,
               			(ND_OS_ERR *)&err);
			
			
		}break;
		case 3:
    {
			OutPort.destination = 3;
			OutPort.Source = 3;
			if((*ExData) == 1)
      {
				EEG_response();	  	
			}
      else
      {
				
			}
			
			

			//memcpy(&StimulateData, ExData,sizeof(StimulateData));
		}break;
		case 4:
    {
     if((*ExData) == 1)
      {
				ND_OSTaskSemPost(Charge_Sem,
                         ND_OS_SEM_TYPE_CHARGE,
                         (ND_OS_ERR *)&err);
			}
			//memcpy(&StimulateData, ExData,sizeof(StimulateData));
		}break;
	}
}
	


void DLLLayer_pack(uint8_t * p_DLL)
{
	uint8_t MIC;
	uint16_t len;
	
	ND_OS_ERR err = NONE;
	
	uint8_t *mem_addr =(uint8_t *)ND_OSMemGet(Mem_Table_One,&err);
	
	 if(mem_addr == NULL )
	 {
	    while(1);
	 }
	
	if(join_Flag == 0)
  {
		p_DLL = p_DLL - 6;
		
		memcpy(p_DLL, Mac,6);
		pHeader = p_DLL;
   	memcpy(pEnd , &MIC, 1);
    pEnd = pEnd + 1;
	}
  else
  {
   // p_DLL = p_DLL - 1;
		//*p_DLL = 0x01;
		pHeader = p_DLL;
		 memcpy(pEnd , &MIC, 1);
	   pEnd = pEnd + 1;
		
	}
	  p_DLL = p_DLL - 1;
	  *p_DLL = join_Flag;
		p_DLL = p_DLL - 1;
		*p_DLL = 0xFF;	
	  pHeader = p_DLL;
	
	 len = pEnd - pHeader;
	
	 uint32_t i=1;
	 uint32_t j=0;
	
	for(;i<len;i++)
	{
	  if(*(pHeader+i) == 0xFF)
		{
		  *(mem_addr+i+j) =0xFE;
			
			 j++;
			
			*(mem_addr+i+j) =0x0D;
		}
		else if(*(pHeader+i) == 0xFE)
		{
		  *(mem_addr+i+j) =0xFE;
			
			j++;
			
			*(mem_addr+i+j) = 0x0E;
		}
		else
		{
		  *(mem_addr+i+j) = *(pHeader+i);
		}
	}
	
	len = len + j;
	
	*(mem_addr) = 0xFF;
	*(mem_addr+len) = 0xFF;
	
	len++;
		   
		/**pEnd = 0xFF;
	  pEnd = pEnd + 1;
	len =pEnd - pHeader; */
	UsartSend(mem_addr, len);
	
	ND_OSMemPut(Mem_Table_One,(MEM_ADDR *)mem_addr,&err);
}


void NetLayer_pack(uint8_t * p_NL)
{
	uint8_t MIC;
	p_NL = p_NL - sizeof(OutAddress);	
	pHeader = p_NL;
	memcpy(p_NL, &OutAddress,sizeof(OutAddress));
	memcpy(pEnd , &MIC, 1);
	pEnd = pEnd + 1;	
	DLLLayer_pack(p_NL);
}

void TLLayer_pack(uint8_t * p_TL)
{
	uint8_t MIC;
	  p_TL = p_TL - sizeof(Port);
	  memcpy(p_TL, &OutPort,sizeof(Port));
	  pHeader = p_TL;
	  memcpy(pEnd , &MIC, 1);
	  pEnd = pEnd + 1;
	  NetLayer_pack(p_TL);
}

void ALLayper_pack(uint8_t * p_AL)
{
	checksum ALchecksum;
	ALchecksum.checksumH = 0;
	ALchecksum.checksumL = 0;
    p_AL = p_AL - sizeof(ALHeader);
		memcpy(p_AL, &OutALHeader,sizeof(ALHeader));
	  pHeader = p_AL;
	  memcpy(pEnd , &ALchecksum , 2);
	  pEnd = pEnd + 2;
	  TLLayer_pack(p_AL);
	  
}



void ALLayer_unpack(uint8_t * p_AL)
{
	
	test111 = sizeof(ALHeader);
	memcpy(&InALHeader, p_AL,sizeof(ALHeader));
	p_AL = p_AL + sizeof(ALHeader);
	ExtrData(p_AL);
}

void TLLayer_unpack(uint8_t* p_TL)
{
	
	memcpy(&InPort, p_TL,sizeof(Port));	
	p_TL = p_TL + sizeof(Port);
	ALLayer_unpack(p_TL);
}

void NetLayer_unpack(uint8_t* p_Net)
{
	
	
	ShortAddress InshortAddress;
	memcpy(&InshortAddress, p_Net,sizeof(ShortAddress));
	
	p_Net = p_Net + sizeof(ShortAddress);
	TLLayer_unpack(p_Net);
}

void Dll_unpack(ND_OS_MSG *msg)
{
	unsigned char *p_dll = (unsigned char *)msg -> MsgPtr;
	
	if(NULL == p_dll)
	{
	   return;
	}
	mem_unpack = p_dll;
	msg_unpack = msg;
	
	int i = 0;
	int recFlag;
	
	recFlag = 0;
	recFlag = *p_dll;
join_Flag = recFlag;
	join_F = recFlag;
	p_dll++;
	if(recFlag == 0 && join_F == 0)
  {
		for(i = 0 ; i < 6; i++)
   {
		 if((*p_dll)!=Mac[i])
     {
			break;
		 }
		 p_dll++;
	 }
	 	if(i == 6)
    {
		  NetLayer_unpack(p_dll);
	  }
	}
	else if(join_F == 1)
  {
		NetLayer_unpack(p_dll);
	}
  

}

void EEG_response(void)
{
	uint16_t *p_out;
	uint16_t *p_store;
  OutALHeader.seq = 0;
	OutALHeader.type = 8;
	OutALHeader.try_num = 0;
	OutALHeader.lengthH = 0;
	OutALHeader.lengthL = 200;
  //p_out = (uint16_t *)malloc(sizeof(uint8_t)*100);
	p_out = (uint16_t *)mem_unpack;
	p_store = p_out;
	p_out = p_out + 15;
	pHeader = (uint8_t *)p_out;
	memcpy(p_out, Mem_tail,2);
	/*if(Mem_tail > Mem_head)
	{
		uint32_t i;
		EEG_ADDR *addr;
		addr = Mem_tail;
		
		for(i=0;i < SAMPLING_FREQUENCY;i++)
		{
		  *(p_out+i) = *addr;
			
			addr ++;
			
			if(addr == End_buf)
			{
			  addr = Head_buf;
			}
		}
	}
	else 
	{
	  uint32_t i;
		EEG_ADDR *addr;
		addr = Mem_tail;
		
		for(i=0;i < SAMPLING_FREQUENCY;i++)
		{
		  *(p_out+i) = *addr;
			
			addr --;
			
			if(addr == Head_buf-1)
			{
			  addr = End_buf;
				addr--;
			}
		}
	}*/
	
	//pEnd = (uint8_t *)(p_out + SAMPLING_FREQUENCY);
	pEnd = (uint8_t *)(p_out+1);
	ALLayper_pack((uint8_t *)p_out);
	//free(p_store);
}

void normal_Sym_Join_Req(void)
{
	uint8_t * p_out;
	uint8_t *p_store;
	OutALHeader.seq = 0;
	OutALHeader.type = 2;
	OutALHeader.try_num = 0;
	OutALHeader.lengthH = 0;
	OutALHeader.lengthL = 2;
	OutPort.destination = 0;
	OutPort.Source = 0;
	OutAddress.destination = 0;
	OutAddress.Source = 0;
	
	p_out = (uint8_t *)malloc(sizeof(uint8_t)*100);
	p_store = p_out;
	p_out = p_out + 30;
	pHeader = p_out;
  memcpy(p_out, SN,2);
	
	pEnd = p_out + 2;
	ALLayper_pack(p_out);
	free(p_store);
}