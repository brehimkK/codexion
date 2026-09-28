#include "codexion.h"

void *coder_routine(void *arg)
{
    int c_id;
    t_coder *coder;
    long current_time;
    long time_to_re;
    long time_to_de;
    long time_to_co;

    coder = arg;
    time_to_co =coder->simulation->config.time_to_compile;
    time_to_de =coder->simulation->config.time_to_debug;
    time_to_re = coder->simulation->config.time_to_refactor;
    while (is_running(coder->simulation))
    {
        if (!take_dongles(coder))
            return (NULL);
        pthread_mutex_lock(&coder->mutex);
        coder->last_compile = get_time_ms();
        current_time = coder->last_compile - coder->simulation->start_time;
        pthread_mutex_unlock(&coder->mutex);
        
        pthread_mutex_lock(&coder->simulation->log_mutex);
        c_id = coder->id;
        printf("%ld %d is compiling\n" , current_time, c_id);
        pthread_mutex_unlock(&coder->simulation->log_mutex);
        safe_sleep(coder->simulation,time_to_co);
        if(!is_running(coder->simulation))
        {
            release_dongles(coder);
            return(NULL);
                
        }
        pthread_mutex_lock(&coder->mutex);
        coder->compile_count++;
        pthread_mutex_unlock(&coder->mutex);
        release_dongles(coder);
        current_time = get_time_ms() - coder->simulation->start_time;
        pthread_mutex_lock(&coder->simulation->log_mutex);
        printf("%ld %d is debugging\n" , current_time, c_id);
        pthread_mutex_unlock(&coder->simulation->log_mutex);
        safe_sleep(coder->simulation, time_to_de);
        if(!is_running(coder->simulation))
            return(NULL);
        current_time = get_time_ms() - coder->simulation->start_time;
        pthread_mutex_lock(&coder->simulation->log_mutex);
        printf("%ld %d is refactoring\n" , current_time, c_id);
        pthread_mutex_unlock(&coder->simulation->log_mutex);
        
        safe_sleep(coder->simulation,time_to_re);
        if(!is_running(coder->simulation))
            return(NULL);
        if (coder_is_finished(coder))
        {
            pthread_mutex_lock(&coder->simulation->finished_mutex);
            coder->simulation->finished_coders++;
            pthread_mutex_unlock(&coder->simulation->finished_mutex);
	        return (NULL);
        }
    }
    return(NULL);
}