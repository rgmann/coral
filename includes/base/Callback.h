#ifndef CALLBACK_H
#define CALLBACK_H

namespace liber {

class Callback {
public:

  Callback() {};
  virtual ~Callback(){};

  virtual void* call(void* pUserParam) = 0;

};

// "Listener" alias for Emitter-Listener usage.
typedef Callback Listener;

}

#endif // CALLBACK_H

