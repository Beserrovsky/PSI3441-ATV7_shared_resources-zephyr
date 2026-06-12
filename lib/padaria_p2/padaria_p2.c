#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "padaria_p2.h"

#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

/* Variavel compartilhada protegida por mutex */
static int saldo_vitrine;
static struct k_mutex vitrine_mutex;

K_THREAD_STACK_DEFINE(padeiro_p2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(cliente_p2_stack, STACK_SIZE);
static struct k_thread padeiro_p2_thread;
static struct k_thread cliente_p2_thread;

static void padeiro_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        k_mutex_lock(&vitrine_mutex, K_FOREVER);
        saldo_vitrine++;
        printk("[Padeiro]  produziu pao  -> saldo_vitrine = %d\n", saldo_vitrine);
        k_mutex_unlock(&vitrine_mutex);

        k_sleep(K_SECONDS(1));
    }
}

static void cliente_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        k_mutex_lock(&vitrine_mutex, K_FOREVER);
        saldo_vitrine--;
        printk("[Cliente]  retirou pao  -> saldo_vitrine = %d\n", saldo_vitrine);
        k_mutex_unlock(&vitrine_mutex);

        k_msleep(1500);
    }
}

void padaria_p2_start(void)
{
    saldo_vitrine = 0;
    k_mutex_init(&vitrine_mutex);

    printk("=== Padaria - Parte 2 (com mutex) ===\n");

    k_thread_create(&padeiro_p2_thread, padeiro_p2_stack, STACK_SIZE,
                    padeiro_thread, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&cliente_p2_thread, cliente_p2_stack, STACK_SIZE,
                    cliente_thread, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);
}
