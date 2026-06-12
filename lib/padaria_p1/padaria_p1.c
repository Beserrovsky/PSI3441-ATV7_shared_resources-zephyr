#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "padaria_p1.h"

#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

/* Variavel compartilhada SEM protecao (mutex/semaforo) de propósito */
static int saldo_vitrine;

K_THREAD_STACK_DEFINE(padeiro_p1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(cliente_p1_stack, STACK_SIZE);
static struct k_thread padeiro_p1_thread;
static struct k_thread cliente_p1_thread;

static void padeiro_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        saldo_vitrine++;
        printk("[Padeiro]  produziu pao  -> saldo_vitrine = %d\n", saldo_vitrine);
        k_sleep(K_SECONDS(1));
    }
}

static void cliente_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        saldo_vitrine--;
        printk("[Cliente]  retirou pao  -> saldo_vitrine = %d\n", saldo_vitrine);
        k_msleep(1500);
    }
}

void padaria_p1_start(void)
{
    saldo_vitrine = 0;

    printk("=== Padaria - Parte 1 (sem sincronizacao) ===\n");

    k_thread_create(&padeiro_p1_thread, padeiro_p1_stack, STACK_SIZE,
                    padeiro_thread, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&cliente_p1_thread, cliente_p1_stack, STACK_SIZE,
                    cliente_thread, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);
}
