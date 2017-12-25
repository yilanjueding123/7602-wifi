#include "drv_l1_system.h"

static INT32S iRAM_Delay(INT32U cnt)
{
	INT32U i;
	
	for (i=0; i<cnt;++i)
	{
		__asm {NOP};
	}
	return 0;
}

////////////////
// ICE �� clock ���ӷ��O SYSCLK
// ICE �ní�w SYSCLK / ICE_CLK > 7
// ��� SYSCLK=144MHz,  ICE LCK=4MHz
// �i�J SLOW mode�A SYSCLK=12MHz�A�ҥH ICE clk ����W�L 1.7M Hz
// ICE ����1.3M Hz �H�K�b���W�ɭ����C
// ����~�� XTAL 12M
void system_clk_ext_XLAT_12M(void)
{
	R_SYSTEM_MISC_CTRL1 |= 0x01;
	R_SYSTEM_CKGEN_CTRL |= 0x14C;

	{
		INT32U i;
		for (i=0;i<0xF000;++i) {
			R_RANDOM0 = i;		// delay for XLAT clock stable
		}
		
	}
	
	R_SYSTEM_CLK_CTRL &= 0x3fff;		//enter SLOW mode

	while ( (R_SYSTEM_POWER_STATE&0xF)!=1 ) {
		//DBG_PRINT("wait goint to SLOW mode\r\n");
		;		
	}
	
	R_SYSTEM_CKGEN_CTRL |= 0x21;		
	R_SYSTEM_CLK_CTRL |= 0x8000;		//enter FAST mode again

	
	while ( (R_SYSTEM_POWER_STATE&0xF)!=2 ) {
		//DBG_PRINT("wait coming back to FAST mode\r\n");
		;		
	}

	R_SYSTEM_CTRL |= 0x00000902;  // josephhsieh@140519 sensor �e�ݩT�w��48MHz
	R_SYSTEM_CTRL &= ~0x1000;	//clear SEL30K
	R_SYSTEM_CKGEN_CTRL |= 0x1F;
}

void setting_by_iRAM(void)
{
	__asm { NOP };
}

void system_clk_alter(INT32U SysClk, INT32U SDramPara)
{	// switch system clock in TFT Vblanking  (ISR)
	SysClk |= (R_SYSTEM_PLLEN & (~0x1F));
	R_MEM_SDRAM_CTRL0 = SDramPara;	
	R_SYSTEM_PLLEN =  SysClk;
	
	iRAM_Delay(16);
}

