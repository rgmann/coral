#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

class Object {
public:

  Object()
  {
    std::cout << "Object::Object()" << std::endl;
  };
  Object(const std::string& name) : mName(name)
  {
    std::cout << "Object::Object(" << name << ")" << std::endl;
  };
  Object(const Object& other)
  {
    mName = other.mName;
    std::cout << "Object::Object(Object(" << mName << "))" << std::endl;
  };

  std::string& name() { return mName; };

  bool operator== (const std::string& name)
  {
    return (mName == name);
  };

private:

  std::string mName;
};

void print_name(Object* pObj)
{
  printf("print_name(%p)\n", pObj);
  std::cout << "pObj->name() = " << pObj->name() << std::endl;
}

int main(int argc, char *argv[])
{
  std::vector<Object> objects;

  objects.push_back(Object("Red"));

  std::vector<Object>::iterator it = objects.begin();
  std::cout << "objects.size() = " << objects.size() << std::endl;
  for (; it != objects.end(); ++it)
  {
    print_name(&(*it));
  }

  if (objects.back() == "Red")
    printf("&back() = %p\n", &objects.back());

  return 0;
}

