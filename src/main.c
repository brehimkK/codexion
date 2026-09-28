#include "codexion.h"

void	set_coder_last_c(t_simulation *simulation, long time)
{
	int	i;

	i = 0;
	while (i < simulation->config.coders)
	{
		simulation->coders[i].last_compile = time;
		i++;
	}
}

int	main(int ac, char **av)
{
	t_simulation	simulation;

	if (!ft_parse(ac, av, &simulation.config))
		return (0);
	if (!init_simulation(&simulation, &simulation.config))
		return (0);
	simulation.start_time = get_time_ms();
	set_coder_last_c(&simulation, simulation.start_time);
	simulation.running = 1;
    if (!init_coders(&simulation))
        return(0);
	pthread_create(&simulation.monitor, NULL,&simulation);
	for (int i = 0; i < simulation.config.coders-1; i++)
	{
		pthread_join(simulation.coders[i].thread, NULL);
	}
	pthread_join(simulation.monitor, NULL);
    
}