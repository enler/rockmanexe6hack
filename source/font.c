#include "exe6.h"

#define MAXCODENUM 150 - 11 - 27
#define VRAMFONTBUFFEROFFSET 0x6004000
#define FONTINROMOFFSET 0x8880000
#define CODELIST (CodeItem*)(0x6004000 + 150 * 64)

//为了确保vram的访问正常，使用位字段
typedef struct _codeItem
{
	u32 code:24;
	u32 index:8;
} CodeItem;

//从0x8044EEC跳过来
void InitCodeList()
{
	CodeItem * codeList = CODELIST;
	volatile CodeItem item;
	item.code = 0xffffff;
	item.index = 11;
	for(int i = 0;i < MAXCODENUM;i++)
	{
		*(codeList + i) = item;
		item.index++;
		if(item.index == 0x5E)
			item.index = 0x79;
	}
}

//使用数组结构，效率的瓶颈，如果是链表就没这个问题，可惜内存有限
void MoveCodeItemToStart(CodeItem * codeList,int index)
{
	if(index >= MAXCODENUM) return;
	CodeItem lastcode = codeList[index];
	for(int i = index - 1;i >= 0;i--)
	{
		codeList[i + 1] = codeList[i];
	}
	codeList[0] = lastcode;
}

int SearchCode(CodeItem * codeList,u32 code,int * lastUnusedIndex)
{
	int i;
	*lastUnusedIndex = -1;
	for(i = 0;i < MAXCODENUM;i++)
	{
		if(codeList[i].code == code)
		{
			return i;
		}
		else if(codeList[i].code == 0xffffff)
		{
			*lastUnusedIndex = i;
			return -1;
		}
	}
	*lastUnusedIndex = MAXCODENUM - 1;//使用索引表最后一个
	return -1;
}

void CopyGlyph(int indexInVramFont,int code)
{
	u8 * temp = &code;
	int indexInFont = getCodeIndex(&temp);//计算原始字库中的偏移
	if(indexInFont == -1) return;
	u32 * src = (u32*)(FONTINROMOFFSET + indexInFont * 0x40);
	u32 * dest = (u32*)(VRAMFONTBUFFEROFFSET + indexInVramFont * 0x40);
	for(int i = 0;i < 0x10;i++)
		*dest++ = *src++;//复制字模
}

int GetIndex(CodeItem * codeList,u32 code)
{
	int lastUnused;
	int index = SearchCode(codeList,code,&lastUnused);
	if(index == -1)
	{
		volatile CodeItem item = codeList[lastUnused];
		item.code = code;
		codeList[lastUnused] = item;
		MoveCodeItemToStart(codeList,lastUnused);//移动到最前面，LRU缓存算法的核心
		CopyGlyph(item.index,code);
		return item.index;
	}
	else
	{
		int glyphindex = codeList[index].index;
		MoveCodeItemToStart(codeList,index);
		return glyphindex;//获取字库在显存中的偏移
	}
}

int GetCode(u8 * str,int * length)
{
	*length = 0;
	int code = str[0];
	if(code >= 0xe6)
		return -1;
	else if(code == 0xe5)
	{
		int code1 = str[1];
		if(code1 < 0xb)
		{
			*length = 3;
			code = (str[2] << 16) | (code1 << 8) | code;
		}
		else if(code1 >= 0x5e && code1 <= 0x9a && code1 != 0x82)
		{
			*length = 2;
			code = (code1 << 8) | code;
		}
		else
			return -2;//表示为有用的0xe5
		
	}
	else if(code == 0xe4)
	{
		*length = 2;
		code = (str[1] << 8)| code;
	}
	else if(code >=0 && code <= 0xe3)
	{
		*length = 1;
	}
	return code;
}

int procMapGen(u8 * str,u16 * mapUP,u16 * mapDown,u16 mapdata,int max)
{
	int i = 0;
	for(; i < max; i++)
	{
		int len;
		int code;
		restart:
		code = GetCode(str,&len);
		if(code == -1)
			break;
		else if(code == -2)
		{
			str++;
			goto restart;//再来一次
		}
		str += len;
		int index = GetIndex(CODELIST,code);
		*mapUP++ = mapdata + (index * 2);
		*mapDown++ = mapdata + (index * 2 + 1);
	}
	int result = i;
	for(; i < max;i++)
	{
		*mapUP++ = 0;
		*mapDown++ = 0;
	}
	return result;
}

//由0x88d00c0跳转过去
void hook_sub8128100(u32 * regs)
{
	regs[0] = procMapGen(regs[0],regs[2],regs[3],regs[4],regs[5]);
	regs[2] += 16;
	regs[3] += 16;
}

//由0x88d0100跳转过去
void hook_sub812E21C(u32 * regs)
{
	procMapGen(regs[0],regs[2],regs[3],regs[6],8);
	regs[2] += 16;
	regs[3] += 16;
}