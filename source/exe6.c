#include "exe6.h"

int getCodeIndex(u8 ** textadrhandle)
{
	u8 * textadr = *textadrhandle;
	int index;
	if(textadr[0] == 0xe5)
	{
		if(textadr[1] >= 0x5e && textadr[1] <= 0x9a && textadr[1] != 0x80 &&textadr[1] != 0x82 && textadr[1] != 0x8a && textadr[1] != 0x98)
		{
			index = 457 + textadr[1] - 0x5e;
			if(textadr[1] > 0x80 && textadr[1] < 0x82)
				index--;
			if(textadr[1] > 0x82 && textadr[1] < 0x8a)
				index-=2;
			else if(textadr[1] > 0x8a && textadr[1] < 0x98)
				index-=3;
			else if(textadr[1] > 0x98)
				index-=4;
			textadr+=2;
		}
		else if(textadr[1] < 0xb)
		{
			index = 457 + 0x39 + textadr[1] * 0xe4 + textadr[2];
			textadr+=3;
		}
		else
		{
			textadr++;
			index = -1;
		}
	}
	else if(textadr[0] >= 0 && textadr[0] <= 0xe3)
	{
		index = textadr[0];
		textadr++;
	}
	else if(textadr[0] == 0xe4)
	{
		index = 0xe4 + textadr[1];
		textadr+=2;
	}
	*textadrhandle = textadr;
	return index;
}

//对应0x8042060这个函数指针，由0x88d0000跳转过去
void hook_sub80420CC(u32 * regs)
{
	regs[1] = getCodeIndex((u8**)(regs + 4));
	if((int)regs[1] != -1)
	{
		regs[0] = 0;
		regs[6] = *(u32*)(regs[5] + 0x7c);
#ifdef VER_RED
		callOrignalFunc(regs,0x3006F64 + 1);
#endif

#ifdef VER_BLUE
		callOrignalFunc(regs,0x3006F60 + 1);//蓝版是0x3006F61
#endif
	}
	regs[0] = 2;
}

//对应0x81D7018这个函数指针，由0x88d0040跳转过去
void hook_sub3006C38(u32 * regs)
{
	regs[1] = getCodeIndex((u8**)regs);
	if((int)regs[1] != -1)
	{
		callOrignalFunc(regs,0x3006C18 + 1);//红蓝一致
	}
	regs[3] = 1;
	regs[9]++;
}

//对应0x8047134这个函数指针的，由0x88d0080跳转过去
void hook_sub3006C38_2(u32 * regs)
{
	regs[1] = getCodeIndex((u8**)regs);
	if((int)regs[1] != -1)
	{
#ifdef VER_RED
		callOrignalFunc(regs,0x8047210 + 1);//蓝版是80471E0 + 1
#endif

#ifdef VER_BLUE
		callOrignalFunc(regs,0x80471E0 + 1);
#endif
	}
	regs[3] = 1;
}

void replaceTextPos(u32 * regs, int regIndex)
{
	if (regs[regIndex])
	{
		u16 * payload = (u16*)regs[regIndex];
		if (*payload != 0xFFFF) return;
		while(*payload == 0xFFFF)
			payload++;
		regs[regIndex] = *payload | (*(payload + 1) << 16);
	}
}

void hook_LoadText(u32 * regs)
{
	replaceTextPos(regs, 0);
	callOrignalFunc(regs, ori_loadText);
}

void copyRegs(u32 * dest,u32 * src)
{
	dest[0] = src[5];
	dest[1] = src[6];
	dest[2] = src[7];
	dest[3] = src[8];
	dest[4] = src[9];
	dest[5] = src[10];
	dest[6] = src[11];
	dest[7] = src[12];
	dest[8] = src[0];
	dest[9] = src[1];
	dest[10] = src[2];
	dest[11] = src[3];
	dest[12] = src[4];
}

void restoreRegs(u32 * dest,u32 * src)
{
	dest[0] = src[8];
	dest[1] = src[9];
	dest[2] = src[10];
	dest[3] = src[11];
	dest[4] = src[12];
	dest[5] = src[0];
	dest[6] = src[1];
	dest[7] = src[2];
	dest[8] = src[3];
	dest[9] = src[4];
	dest[10] = src[5];
	dest[11] = src[6];
	dest[12] = src[7];
}

/*
void funcTest(u32 * regs)
{
	callOrignalFunc(regs,0x3006c39);
}*/