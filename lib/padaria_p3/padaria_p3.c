#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "padaria_p3.h"

#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

#define VITRINE_CAPACIDADE 10

/* Variavel compartilhada protegida por mutex + sincronizada por semaforos */
static int saldo_vitrine;
static struct k_mutex vitrine_mutex;

/* sem_paes:   conta paes disponiveis na vitrine   (0..VITRINE_CAPACIDADE) */
/* sem_espaco: conta espacos livres na vitrine      (0..VITRINE_CAPACIDADE) */
static struct k_sem sem_paes;
static struct k_sem sem_espaco;

K_THREAD_STACK_DEFINE(padeiro_p3_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(cliente_p3_stack, STACK_SIZE);
static struct k_thread padeiro_p3_thread;
static struct k_thread cliente_p3_thread;

static void padeiro_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        /* Aguarda haver espaco livre na vitrine */
        k_sem_take(&sem_espaco, K_FOREVER);

        k_mutex_lock(&vitrine_mutex, K_FOREVER);
        saldo_vitrine++;
        printk("[Padeiro]  produziu pao  -> saldo_vitrine = %d\n", saldo_vitrine);
        k_mutex_unlock(&vitrine_mutex);

        /* Sinaliza que ha mais um pao disponivel */
        k_sem_give(&sem_paes);

        k_sleep(K_SECONDS(1));
    }
}

static void cliente_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        /* Aguarda haver pao disponivel na vitrine */
        k_sem_take(&sem_paes, K_FOREVER);

        k_mutex_lock(&vitrine_mutex, K_FOREVER);
        saldo_vitrine--;
        printk("[Cliente]  retirou pao  -> saldo_vitrine = %d\n", saldo_vitrine);
        k_mutex_unlock(&vitrine_mutex);

        /* Sinaliza que ha mais um espaco livre na vitrine */
        k_sem_give(&sem_espaco);

        k_msleep(1500);
    }
}

void padaria_p3_start(void)
{
    saldo_vitrine = 0;
    k_mutex_init(&vitrine_mutex);

    /* Vitrine comeca vazia: nenhum pao disponivel, capacidade toda livre */
    k_sem_init(&sem_paes, 0, VITRINE_CAPACIDADE);
    k_sem_init(&sem_espaco, VITRINE_CAPACIDADE, VITRINE_CAPACIDADE);

    printk("=== Padaria - Parte 3 (com semaforos, capacidade = %d) ===\n",
           VITRINE_CAPACIDADE);

    k_thread_create(&padeiro_p3_thread, padeiro_p3_stack, STACK_SIZE,
                    padeiro_thread, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&cliente_p3_thread, cliente_p3_stack, STACK_SIZE,
                    cliente_thread, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);
}
