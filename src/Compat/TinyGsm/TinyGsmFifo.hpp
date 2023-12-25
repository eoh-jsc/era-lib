#ifndef TinyGsmFifo_h
#define TinyGsmFifo_h

template <class T, unsigned N>
class TinyGsmFifo
{
public:
    TinyGsmFifo()
    {
        this->clear();
    }

    void clear()
    {
        this->_r = 0;
        this->_w = 0;
    }

    // writing thread/context API
    //-------------------------------------------------------------

    bool writeable(void) {
        return (this->free() > 0);
    }

    int free(void) {
        int s = (this->_r - this->_w);
        if (s <= 0) {
            s += N;
        }
        return (s - 1);
    }

    bool put(const T& c) {
        int i = this->_w;
        int j = i;
        i = this->_inc(i);
        if (i == this->_r) { // !writeable()
            return false;
        }
        this->_b[j] = c;
        this->_w = i;
        return true;
    }

    int put(const T* p, int n, bool t = false) {
        int c = n;
        while (c) {
            int f;
            while ((f = this->free()) == 0) { // wait for space
                if (!t) {
                    return (n - c); // no more space and not blocking
                }
                /* nothing / just wait */;
            }
            // check free space
            if (c < f) {
                f = c;
            }
            int w = this->_w;
            int m = (N - w);
            // check wrap
            if (f > m) {
                f = m;
            }
            memcpy(&this->_b[w], p, f);
            this->_w = this->_inc(w, f);
            c -= f;
            p += f;
        }
        return (n - c);
    }

    // reading thread/context API
    // --------------------------------------------------------

    bool readable(void) {
        return (this->_r != this->_w);
    }

    size_t size(void) {
        int s = (this->_w - this->_r);
        if (s < 0) {
            s += N;
        }
        return s;
    }

    bool get(T* p) {
        int r = this->_r;
        if (r == this->_w) { // !readable()
            return false;
        }
        *p = this->_b[r];
        this->_r = this->_inc(r);
        return true;
    }

    int get(T* p, int n, bool t = false) {
        int c = n;
        while (c) {
            int f;
            for (;;) { // wait for data
                f = this->size();
                if (f) {
                    break;        // free space
                }
                if (!t) {
                    return (n - c); // no space and not blocking
                }
                /* nothing / just wait */;
            }
            // check available data
            if (c < f) {
                f = c;
            }
            int r = this->_r;
            int m = (N - r);
            // check wrap
            if (f > m) {
                f = m;
            }
            memcpy(p, &this->_b[r], f);
            this->_r = this->_inc(r, f);
            c -= f;
            p += f;
        }
        return (n - c);
    }

    uint8_t peek() {
        return this->_b[this->_r];
    }

private:
    int _inc(int i, int n = 1) {
        return ((i + n) % N);
    }

    T _b[N];
    volatile int _w;
    volatile int _r;
};

#endif
