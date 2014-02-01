#ifndef CALLBACK_H
#define CALLBACK_H

namespace liber {

class Callback {
public:

  Callback() {};
  virtual ~Callback();

  virtual void call(void* pUserParam) = 0;

};

}

#endif // CALLBACK_H

