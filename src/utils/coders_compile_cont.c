#include "codexion.h"

int compile_counter(t_coder *coder)
{
    long counter;
    int i;
    int j;
    int coder_c;

    j = 0;
    i = coder->simulation->config.coders;
    counter = coder->simulation->config.compile_required;
    while (j < i)
    {
        pthread_mutex_lock(&coder->simulation->coders[j].mutex);
        coder_c = coder->simulation->coders[j].compile_count;
        pthread_mutex_unlock(&coder->simulation->coders[j].mutex);
        if (coder_c < counter)
            return(0);
        else
            j++;
    }
    return(1);
}