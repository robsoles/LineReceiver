#ifndef LINERECEIVER_H
#define LINERECEIVER_H

#include <Arduino.h>

class LineReceiver
{
  public:
    typedef void (*callback_t)(const char*);
    static const size_t default_size = 100;

    LineReceiver(size_t n = default_size, callback_t callback = nullptr)
    {
      init(n, callback);
    }

    LineReceiver()
    {
      init(default_size, nullptr);
    }

    bool push(char c)
    {
      if (c != '\n')
      {
        if (p_ < &buf_[size_-1])
        {
          *p_ = c;
          if(++p_>&buf_[size_-2]) flags|=1; // set the overflow flag now because next char in throws it anyway.
        }
        else
        { // 
          flags|=1; // set the overflowed flag again - pedantic perhaps?
          if(eventOverflow_!=nullptr) eventOverflow_(buf_); // give them a look if they have set overflowCallback(...);
          reset();
          terminate();
        }
      }
      else // line ends
      {
        terminate();
        reset();
        if(callback_ != nullptr)
        {
          callback_(buf_);
          terminate();
        }
        return true;
      }
      return false;
    }

    const char* get() const
    {
      return buf_;
    }

    size_t size() const
    {
      return size_;
    }

    size_t length() const
    {
      return p_ - buf_;
    }

    size_t space() const
    {
      return size() - length() - 1;
    }
	
    void overflowCallback(callback_t eventOverflow)
    {
      eventOverflow_=eventOverflow;
    }

    bool overflowed(void)
    {
      if(!flags&1) return false;
      flags&=~1; // clear the flag.
      return true; // maybe an alternative callback can be offered for overflow event?
    }


    ~LineReceiver()
    {
      free(buf_);
    }
  protected:
  private:
    void init(size_t n, callback_t callback)
    {
      buf_ = (char*)malloc(n);
      size_ = n;
      callback_ = callback;
      reset();
      terminate();
    }

    void reset() // could also be public
    {
      p_ = buf_;
      if(flags&1) flags&=~1; // clear the overflowed flag.
    }

    void terminate()
    {
      *p_ = 0;
    }

    callback_t callback_, eventOverflow_;
    char* buf_;
    char* p_;
    size_t size_;
    uint8_t flags;
};


#endif // LINERECEIVER_H

