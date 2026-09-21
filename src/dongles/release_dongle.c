#include "codexion.h"

void	release_dongle(t_simulation *simulation, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = 0;
	dongle->available_at = get_time_ms()
		+ simulation->config.dongle_cooldown;
	pthread_cond_broadcast(&dongle->condition);
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongles(t_coder *coder)
{
	release_dongle(coder->simulation, coder->dongle_a);
	if (coder->dongle_b != coder->dongle_a)
		release_dongle(coder->simulation, coder->dongle_b);
}