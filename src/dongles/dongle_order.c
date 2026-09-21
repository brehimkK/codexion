
#include "codexion.h"

void	get_dongle_order(t_coder *coder, t_dongle **first,
		t_dongle **second)
{
	if (coder->dongle_a->id <= coder->dongle_b->id)
	{
		*first = coder->dongle_a;
		*second = coder->dongle_b;
	}
	else
	{
		*first = coder->dongle_b;
		*second = coder->dongle_a;
	}
}