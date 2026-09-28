#include "codexion.h"

int	coder_is_finished(t_coder *coder)
{
	int	count;

	pthread_mutex_lock(&coder->mutex);
	count = coder->compile_count;
	pthread_mutex_unlock(&coder->mutex);
	if (count >= coder->simulation->config.compile_required)
		return (1);
	return (0);
}

int	finished_coders(t_simulation *simulation)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < simulation->config.coders)
	{
		if (coder_is_finished(&simulation->coders[i]))
			j++;
		i++;
	}
	if (j == i)
		return (1);
	return (0);
}