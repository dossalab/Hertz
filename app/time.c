#include "time.h"

float global_time_counter;

void update_counter(float spent)
{
	global_time_counter += spent;
}
