#include ""














//#include "codexion.h"

//static void coder_compile(t_coder *coder)
//{
//    pthread_mutex_lock(&coder->left_dongle->lock);
//    print_status(coder, "has taken a dongle");
//    pthread_mutex_lock(&coder->right_dongle->lock);
//    print_status(coder, "has taken a dongle");

//    pthread_mutex_lock(&coder->data->state_lock);
//    coder->last_compile_start_ms = get_current_time_ms();
//    pthread_mutex_unlock(&coder->data->state_lock);

//    print_status(coder, "is compiling");
//    my_usleep(coder->data->t_compile);

//    pthread_mutex_lock(&coder->data->state_lock);
//    coder->compiles_done++;
//    pthread_mutex_unlock(&coder->data->state_lock);

//    pthread_mutex_unlock(&coder->left_dongle->lock);
//    pthread_mutex_unlock(&coder->right_dongle->lock);
//}

//void    *coder_routine(void *arg)
//{
//    t_coder *coder;

//    coder = (t_coder *)arg;

//    // Anti-Deadlock: L-Coders z-zawjiyin (even) kay-tsennaw 1ms bach may-tza7mouch
//    if (coder->id % 2 == 0)
//        usleep(1000);

//    while (check_sim_active(coder->data) == true)
//    {
//        coder_compile(coder);

//        print_status(coder, "is refactoring");
//        my_usleep(coder->data->t_refactor);

//        print_status(coder, "is debugging");
//    }
//    return (NULL);
//}

//int start_simulation(t_data *data)
//{
//    int i;
//    int j;

//    data->start_time_ms = get_current_time_ms();
//    i = 0;
//    while (i < data->nb_coders)
//    {
//        data->coders[i].last_compile_start_ms = data->start_time_ms;

//        if (pthread_create(&data->coders[i].thread, NULL, coder_routine, &data->coders[i]) != 0)
//        {
//            pthread_mutex_lock(&data->state_lock);
//            data->sim_active = false;
//            pthread_mutex_unlock(&data->state_lock);
//            j = 0;
//            while (j < i)
//            {
//                pthread_join(data->coders[j].thread, NULL);
//                j++;
//            }
//            return (1);
//        }
//        i++;
//    }
//    return (0);
//}
