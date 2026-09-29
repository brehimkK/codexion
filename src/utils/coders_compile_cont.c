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
