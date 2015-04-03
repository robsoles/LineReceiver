#ifndef LINERECEIVER_H
#define LINERECEIVER_H

#include <Arduino.h>

float lineParse(const char* buf1, unsigned char* ptr);
unsigned char lineParse(const char* buf1, unsigned char* ptr, const char* cPtr);

class LineReceiver
{
  public:
    typedef void (*callback_t)(const char*);
    static const size_t default_size = 100;

    LineReceiver(size_t n = default_size, callback_t callback = nullptr) { init(n, callback); }
    LineReceiver() { init(default_size, nullptr); }

    bool push(char c);

    const char* get() const { return buf_; }
    size_t size() const { return size_; }
    size_t length() const { return p_ - buf_; }
    size_t space() const { return size() - length() - 1; }
    
    void backspace() { if(p_>buf_) --*p_=0; }
    void clear();
	
    void setOverflowCallback(callback_t callback) { eventOverflow_=callback; }
    void setCallback(callback_t callback) { callback_=callback; }

    bool overflowed();
    
    const char lastChar() { if(p_>buf_) return *(p_-1); else return 0; } // access to last character to enter buffer

    void pushTrueOverflow(bool n) { flags=(flags&~2)|(n<<1); } // makes push return true if buffer overflow occurs
    void pushTrueSpecialChars(bool n) { flags=(flags&~4)|(n<<2); } // makes push return true if a special character enters buffer
    void callbackSpecialChars(bool n) { flags=(flags&~24)|(n<<3); } // calls regular callback for special char if this is set, cancels overflowSpecialChars
    void overflowSpecialChars(bool n) { flags=(flags&~24)|(n<<4); } // calls eventOverflow callback for special char, cancels callbackSpecialChars

    ~LineReceiver() { free(buf_); }
    
  protected:
  private:
    void init(size_t n, callback_t callback);
    callback_t callback_, eventOverflow_;
    char* buf_;
    char* p_;
    size_t size_;
    uint8_t flags;
};


#endif // LINERECEIVER_H

