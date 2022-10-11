#include "include/chatroom.h"

using namespace std;

string color(int code)
{
	return colors[code % NUM_COLORS];
}
