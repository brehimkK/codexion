#include "codexion.h"

void *coder_routine(void *arg)
{
    int c_id;
    t_coder *coder;
    long current_time;
    
    coder = arg;
    while (is_running(coder->simulation))
    {
        if (!take_dongles(coder))
            return (NULL);
        pthread_mutex_lock(&coder->mutex);
        coder->last_compile = get_time_ms() - coder->simulation->start_time;
        current_time = coder->last_compile;
        pthread_mutex_unlock(&coder->mutex);
        
        pthread_mutex_lock(&coder->simulation->log_mutex);
        c_id = coder->id;
        pthread_mutex_unlock(&coder->simulation->log_mutex);
        safe_sleep(coder->simulation->config.time_to_compile);
        if(!is_running(coder->simulation))
        {
            release_dongles(coder);
            return(NULL);
                
        }
        pthread_mutex_lock(&coder->mutex);
        coder->compile_count++;
        pthread_mutex_unlock(&coder->mutex);
        release_dongles(coder);
        if(compile_counter(coder))
        {
            stop_simulation(coder->simulation);
            wake_all(coder->simulation);
            return(NULL);
        }
        current_time = get_time_ms() - coder->simulation->start_time;
        pthread_mutex_lock(&coder->simulation->log_mutex);
        printf("%ld %d is debugging\n" , current_time, c_id);
        pthread_mutex_unlock(&coder->simulation->log_mutex);
        safe_sleep(coder->simulation->config.time_to_debug);
        if(!is_running(coder->simulation))
            return(NULL);
        current_time = get_time_ms() - coder->simulation->start_time;
        pthread_mutex_lock(&coder->simulation->log_mutex);
        printf("%ld %d is refactoring\n" , current_time, c_id);
        pthread_mutex_unlock(&coder->simulation->log_mutex);
        safe_sleep(coder->simulation->config.time_to_refactor);
        if(!is_running(coder->simulation))
            return(NULL);
    }
    return(NULL);
}