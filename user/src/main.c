#include "stm32f10x.h"
#include "led.h"
#include "timer.h"
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_COILS_START 0
#define REG_COILS_NREGS 16

#define REG_DISCRETE_START 10
#define REG_DISCRETE_NREGS 16

#define REG_HOLDING_START 20
#define REG_HOLDING_NREGS 5

#define REG_INPUT_START 30
#define REG_INPUT_NREGS 5

/* ----------------------- Static variables ---------------------------------*/
static uint16_t usRegCoilsStart = REG_COILS_START;
static uint8_t usRegCoilsBuf[2];

static uint16_t usRegDiscreteStart = REG_DISCRETE_START;
static uint8_t usRegDiscreteBuf[2];

static uint16_t usRegHoldingStart = REG_HOLDING_START;
static uint16_t usRegHoldingBuf[REG_HOLDING_NREGS]={0x0010,0x0011,0x0012,0x0013,0x0014};

static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS]={0x0000,0x0001,0x0002,0x0003,0x0004};

/*
 * Modbus������������
 * ��Ȧ--------RW---bit---O
 * ��ɢ����----RO---bit---I
 * ���ּĴ���--RW---bit---O
 * ����Ĵ���--RO---bit---I
 * RWֻ�������modbusЭ����ԣ�RO���������Ÿ����ݲ��ܱ��޸ģ�ֻ�ǲ���ͨ��modbusЭ���޸Ķ��ѣ����������������Ȼ�ǿɶ���д�ġ�
 * */

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    eMBErrorCode    eStatus;

    eStatus = eMBInit( MB_RTU, 0x01, 0, 115200, MB_PAR_EVEN );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

    for( ;; )
    {
        ( void )eMBPoll(  );

        /* Here we simply count the number of poll cycles. */
        usRegInputBuf[0]++;
    }
}

//����Ĵ���
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

//���ּĴ���
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;

	if( ( usAddress < REG_HOLDING_START )
	        || ( usAddress + usNRegs > REG_HOLDING_START + REG_HOLDING_NREGS ) )
		return MB_ENOREG;

	iRegIndex = ( int )( usAddress - usRegHoldingStart );

	while(usNRegs > 0)
	{
		if(eMode==MB_REG_READ)
		{
			*pucRegBuffer++ =
				( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
			*pucRegBuffer++ =
				( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
		}
		else if(eMode==MB_REG_WRITE)		//�߰�λ��ǰ���Ͱ�λ�ں�
		{
			usRegHoldingBuf[iRegIndex]=*pucRegBuffer;
			usRegHoldingBuf[iRegIndex]<<=8;
			pucRegBuffer++;
			usRegHoldingBuf[iRegIndex]|=*pucRegBuffer;
			pucRegBuffer++;
		}

		iRegIndex++;
		usNRegs--;
	}

    return eStatus;
}

//��Ȧ״̬
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int				iRegGroup;
	int				iRegPos;
	int             iRegIndex;

	if( ( usAddress < REG_COILS_START )
			|| ( usAddress + usNCoils > REG_COILS_START + REG_COILS_NREGS ) )
		return MB_ENOREG;

	iRegIndex=( int )( usAddress - usRegCoilsStart );


	while(usNCoils>0)
	{
		iRegGroup = iRegIndex/8;
		iRegPos = iRegIndex%8;


		iRegIndex++;
		usNCoils--;
	}

	if(eMode==MB_REG_READ)
	{

	}
	else if(eMode==MB_REG_WRITE)
	{

	}
    return MB_ENOREG;
}

//��ɢ����
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
