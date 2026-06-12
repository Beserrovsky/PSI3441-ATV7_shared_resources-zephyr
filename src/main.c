#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <padaria_p1.h>
#include <padaria_p2.h>
#include <padaria_p3.h>

/*
 * Selecione qual parte da atividade executar:
 *   1 -> Parte 1: padeiro e cliente sem sincronizacao
 *   2 -> Parte 2: padeiro e cliente com mutex
 *   3 -> Parte 3: padeiro e cliente com semaforos (capacidade da vitrine)
 */
#define PADARIA_PARTE 1

int main(void)
{
#if PADARIA_PARTE == 1
    padaria_p1_start();
#elif PADARIA_PARTE == 2
    padaria_p2_start();
#elif PADARIA_PARTE == 3
    padaria_p3_start();
#else
#error "PADARIA_PARTE deve ser 1, 2 ou 3"
#endif

    return 0;
}
