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
          p_++;
        }
        else
        { // overflow: discard, reset line MAYBE ADD AN OVERFLOW FLAG?
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
    }

    void terminate()
    {
      *p_ = 0;
    }

    callback_t callback_;
    char* buf_;
    char* p_;
    size_t size_;
};