typedef struct
{
	uint8_t Current;
	uint32_t Freq;
	uint32_t Pwper;
	uint32_t BurstDuration;
	uint8_t St_Flag;
}Stimulate;
extern short PCOut;

struct transport
{
	uint8_t ShtAddr_Source;
	uint8_t ShtAddr_Destination;
	uint8_t Port_Source;
	uint8_t Port_Destination;
	uint8_t seq;
	uint8_t type;
	uint8_t try_num;
	uint8_t lengthH;
	uint8_t lengthL;
};

typedef struct transport TRANSPORT;