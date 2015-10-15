#include <stdio.h>
#include "Log.h"
#include "OtherLogUser.h"


void OtherLogUser::printAnInt(int anInt)
{
  coral::log::status("anInt = %d\n", anInt);
}

