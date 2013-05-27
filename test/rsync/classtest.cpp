#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <string>

enum Type {
   TYPE_B,
   TYPE_C
};

class A {
public:
   
   A() : a_value(0) {};
   
   A(int a) : a_value(a) {};
   
   virtual void print() {
      printf("class A: a_value = %d\n", a_value);
   };
   
protected:
   
   int a_value;
};

class B : public A {
public:
   
   B() : A() {};
   
   B(int a, int b) : A(a), b_value(b) {};
   
   virtual void print() {
      printf("class B: a_value = %d, b_value = %d\n", a_value, b_value);
   };
   
protected:
   
   int b_value;
};

class C : public A {
public:
   
   C() : A() {};
   
   C(int a, int c) : A(a), c_value(c) {};
   
   virtual void print() {
      printf("class C: a_value = %d, c_value = %d\n", a_value, c_value);
   };
   
protected:
   
   int c_value;
};

void create(Type type, A* pInst);
void create(Type type, A** pInst);
void create(C** pInst);
A* create(Type type);

int main(int argc, char *argv[])
{
   A* l_pInst = NULL;
   
   printf("class test\n");

   printf("create(Type type, A* pInst)\n");
   create(TYPE_C, l_pInst);
   
   if (l_pInst) {
      l_pInst->print();
      
      delete l_pInst; l_pInst = NULL;
   }
   else {
      printf("Failed to create with create(Type type, A* pInst)\n");
   }
   
   
   printf("\ncreate(Type type, A** pInst)\n");
   create(TYPE_C, &l_pInst);
   
   if (l_pInst) {
      l_pInst->print();
      
      delete l_pInst; l_pInst = NULL;
   }
   else {
      printf("Failed to create with create(Type type, A** pInst)\n");
   }
   
   printf("\ncreate(C** pInst)\n");
   C* l_pCInst = NULL;
   create(&l_pCInst);
   
   if (l_pCInst) {
      l_pCInst->print();
      
      delete l_pCInst; l_pCInst = NULL;
   }
   else {
      printf("Failed to create with create(C** pInst)\n");
   }
   
   printf("\nA* create(Type type)\n");
   l_pInst = create(TYPE_C);
   
   if (l_pInst) {
      l_pInst->print();
      
      delete l_pInst; l_pInst = NULL;
   }
   else {
      printf("Failed to create with A* create(Type type)\n");
   }

   return 0;
}

void create(Type type, A* pInst)
{
   switch (type) {
      case TYPE_B:
         pInst = new B(10, 11);
         break;
         
      case TYPE_C:
         pInst = new C(10, 15);
         break;
         
      default:
         break;
   }
}

void create(Type type, A** pInst)
{
   switch (type) {
      case TYPE_B:
         *pInst = new B(10, 11);
         break;
         
      case TYPE_C:
         *pInst = new C(10, 15);
         break;
         
      default:
         break;
   }
}

void create(C** pInst)
{
   *pInst = static_cast<C*>(create(TYPE_C));
}

A* create(Type type)
{
   A* l_pInst = NULL;
   
   switch (type) {
      case TYPE_B:
         l_pInst = new B(10, 11);
         break;
         
      case TYPE_C:
         l_pInst = new C(10, 15);
         break;
         
      default:
         break;
   }
   
   return l_pInst;
}
