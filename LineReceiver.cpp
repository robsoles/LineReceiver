
#include "LineReceiver.h"

// define some macros for use in flags
#define OVERFLOWED 1
#define PUSHTRUEOVERFLOW 2
#define PUSHTRUESPECIAL 4
#define CALLBACKSPECIAL 8
#define OVERFLOWSPECIAL 16



void LineReceiver::init(size_t n, callback_t callback)
{
  p_=buf_ = (char*)malloc(n);
  size_ = n;
  callback_ = callback;
  *p_=0;
}

bool LineReceiver::push(char c)
{
  *p_++=c;
  *p_=0;
  if(c=='\n') {
    if(callback_!=nullptr) callback_(buf_);
    p_=buf_;
    return true;
  } else if((c<' ')&&(flags&(CALLBACKSPECIAL|OVERFLOWSPECIAL|PUSHTRUESPECIAL))) {
    if(flags&CALLBACKSPECIAL) callback_(buf_);
    if(flags&OVERFLOWSPECIAL) eventOverflow_(buf_);
    if(flags&PUSHTRUESPECIAL) return true;
  }
  if(p_==&buf_[size_-1]) {
    flags|=OVERFLOWED; // set overflow flag for the handler (if any)
    if(eventOverflow_!=nullptr) {
      eventOverflow_(buf_);
      flags&=~OVERFLOWED; // only reset because they have a handler
    }
    p_=buf_;
    if(flags&PUSHTRUEOVERFLOW) return true;
  }
  return false;
}
  
bool LineReceiver::overflowed()
{
  if(!flags&OVERFLOWED) return false;
  flags&=~OVERFLOWED; // clear the flag.
  return true;
}

void LineReceiver::clear()
{
  p_=buf_;
  *p_=0;
  flags&=~OVERFLOWED;
}


/**********************************************************************************/



float strHex(const char* buf, unsigned char* ptr)
{
	unsigned char go=1;
	unsigned char breaker=0;
	float tmp=0;
	uint8_t tmp1=0;
	while((*buf<48)&&(*buf!=0))
	{
		buf++;
		*ptr+=1;
	}
	if((*buf=='0')&&(*(buf+1)=='x'))
	{
		buf+=2;
		*ptr+=2;
	}
	if(*buf==0) return 0;
	while(go!=0&&(++breaker)) {
		tmp1=(uint8_t)*buf;
		if((tmp1>64)&&(tmp1<71)) tmp1-=7;
		if((tmp1>96)&&(tmp1<103)) tmp1-=39;
		if((tmp1>47)&&(tmp1<64)) {
			tmp=(tmp*16)+tmp1-48;
			buf++;
			*ptr+=1;
		} else {
			go=0;
		}
	}
	return tmp;
}

float lineParse(const char* buf, unsigned char* ptr)
{
	unsigned char go=1;
	unsigned char breaker=0;
	float tmp=0;
	float frac=0;
	float sign1=1;

	while(go!=0&&(++breaker)) {
		if(*buf!=0) {
			if((*buf<33)||(*buf=='=')||(*buf==',')||(*buf==';')||(*buf==':')) {
				buf++;
				*ptr+=1;
			} else {
				go=0;
			}
		} else {
			go=0;
		}
	}

	if(((*buf==0)||(*buf<48)||(*buf>57))&&(*buf!='-')) {
		return 0;
	}
	if(*buf=='-')
	{
		sign1=-1;
		buf++;
		*ptr+=1;
	}
	if((*buf=='0')&&(*(buf+1)=='x')) {
		return sign1*strHex(buf,ptr);
	}
	go=1;
	while(go!=0&&(++breaker)) {
		if((*buf>47)&&(*buf<58)) {
			if(frac)
			{
				tmp=tmp+(((*buf)-48)/frac);
				frac*=10;
			} else {
				tmp=(tmp*10)+((*buf)-48);
			}
			buf++;
			*ptr+=1;
		} else if(*buf=='.') {
			if(frac) return sign1*tmp; // second decimal point in a number is bullshit.
			frac=10;
			buf++;
			*ptr+=1;
		} else {
			go=0;
		}
	}
	return sign1*tmp;
}

unsigned char lineParse(const char* buf1, unsigned char* ptr, const char* cPtr)
{
	const char * sPtr;
	const char * tPtr;
	unsigned char count=0,cnt2,go=1;
	while(go)
	{
		count++;
		sPtr=cPtr;
		while(*cPtr!=0) {
			cPtr++;
		}
		cPtr++;
		tPtr=buf1;
		cnt2=0;
		while((*sPtr==*tPtr)&&(*tPtr!=0))
		{
			cnt2++;
			sPtr++;
			tPtr++;
		}
		if(*sPtr==0)
		{
			*ptr+=cnt2;
			return count;
		}
		go=*cPtr;
	}
	return 0;
}

