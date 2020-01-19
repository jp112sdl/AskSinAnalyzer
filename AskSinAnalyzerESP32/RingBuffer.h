//- -----------------------------------------------------------------------------------------------------------------------
// 2020-01-19 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

template<class TYPE,int SIZE>
class RingStack {
  TYPE  _buffer[SIZE];
  TYPE* _current;
  int   _count;

  TYPE& first () {
    return _buffer[0];
  }

  TYPE& last () {
    return _buffer[SIZE-1];
  }

public:
  RingStack () {
    _current = &first();
    _count = 0;
  }

  int size () const { return SIZE; }

  int count () const { return _count; }

  bool shift () {
    if( _count < SIZE ) _count++;
    _current--;
    if( _current < _buffer ) _current = &last();
    return _count == SIZE;
  }

  bool shift (const TYPE& data) {
    bool result = shift();
    *_current = data;
    return result;
  }

  TYPE& operator [] (int index) {
    index = std::min(index,_count-1);
    return *(_buffer + ((_current - _buffer + index) % SIZE));
  }

  const TYPE& operator [] (int index) const {
    index = std::min(index,_count-1);
    return *(_buffer + ((_current - _buffer + index) % SIZE));
  }
};


#endif /* RINGBUFFER_H_ */
