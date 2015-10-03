#include <stdio.h>
#include "Log.h"
#include "OtherLogUser.h"


void OtherLogUser::printAnInt(int anInt)
{
  liber::log::status("anInt = %d\n", anInt);
}

