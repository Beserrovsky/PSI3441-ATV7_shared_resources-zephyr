# Relatório — Compartilhamento de Recursos: Produtor e Consumidor (Padaria)

## Nome
Felipe Beserra de Oliveira

---

## Número USP
13683702

---

## Respostas, comentários e análises

### Descrição da Atividade

O experimento simula uma padaria utilizando duas threads do Zephyr RTOS:

- **Padeiro (Thread 1):** produz um pão a cada 1 segundo e o coloca na vitrine.
- **Cliente (Thread 2):** tenta retirar um pão da vitrine a cada 1,5 segundos.

A vitrine é representada pela variável compartilhada `saldo_vitrine`:

- Produção: `saldo_vitrine++`
- Consumo: `saldo_vitrine--`

A atividade é dividida em três partes, cada uma implementada como uma biblioteca
separada em `lib/`, selecionável em `src/main.c` através da constante `PADARIA_PARTE`:

| Parte | Biblioteca | Mecanismo |
|---|---|---|
| 1 | `lib/padaria_p1` | Sem sincronização |
| 2 | `lib/padaria_p2` | Mutex |
| 3 | `lib/padaria_p3` | Semáforos (mutex + 2 semáforos) |

---

### Parte 1 — Sem sincronização

**Regras:**
- O padeiro produz um pão a cada 1 segundo (`saldo_vitrine++`).
- O cliente tenta retirar um pão a cada 1,5 segundos (`saldo_vitrine--`).
- Nenhum mutex, semáforo ou fila é utilizado.

**Perguntas de observação** *(responder após executar `PADARIA_PARTE 1` na placa)*:

1. O saldo da vitrine permanece consistente?
   - _A preencher._
2. O saldo pode se tornar negativo?
   - _A preencher._
3. O comportamento muda ao alterar os tempos das threads?
   - _A preencher._
4. O comportamento muda ao alterar as prioridades das threads?
   - _A preencher._
5. Quais problemas podem ocorrer quando duas threads acessam a mesma variável simultaneamente?
   - _A preencher._

---

### Parte 2 — Utilizando Mutex

Um mutex (`vitrine_mutex`) protege todo acesso (leitura e escrita) à variável
`saldo_vitrine`, garantindo que o incremento/decremento seja uma operação atômica.

**Perguntas de observação** *(responder após executar `PADARIA_PARTE 2` na placa)*:

1. O mutex eliminou os problemas observados na Parte 1?
   - _A preencher._
2. O mutex impede que o cliente retire pão de uma vitrine vazia?
   - _A preencher._
3. Qual é exatamente o recurso protegido pelo mutex?
   - _A preencher._
4. Qual é a principal função de um mutex neste problema?
   - _A preencher._

---

### Parte 3 — Utilizando Semáforos

A vitrine passa a ter capacidade máxima de **10 pães**. Dois semáforos de contagem
controlam a disponibilidade do recurso, além do mutex que protege `saldo_vitrine`:

- `sem_paes` (inicial 0, máx. 10): número de pães disponíveis. O cliente faz
  `k_sem_take(&sem_paes)` antes de retirar — bloqueia se não houver pão.
- `sem_espaco` (inicial 10, máx. 10): número de vagas livres na vitrine. O padeiro
  faz `k_sem_take(&sem_espaco)` antes de produzir — bloqueia se a vitrine estiver cheia.

**Perguntas de observação** *(responder após executar `PADARIA_PARTE 3` na placa)*:

1. O saldo pode se tornar negativo?
   - _A preencher._
2. O saldo pode ultrapassar a capacidade da vitrine?
   - _A preencher._
3. Qual problema foi resolvido pelos semáforos que não era resolvido apenas pelo mutex?
   - _A preencher._
4. Qual é o papel de cada semáforo utilizado?
   - _A preencher._

---

### Comparação

1. Qual a diferença entre proteger um recurso e controlar sua disponibilidade?
   - _A preencher._
2. Em qual parte da atividade o mutex foi suficiente?
   - _A preencher._
3. Em qual parte os semáforos se mostraram mais adequados?
   - _A preencher._
4. Se fosse necessário implementar uma padaria real, você utilizaria mutex, semáforos ou ambos? Justifique.
   - _A preencher._

---

## Código (main.c)

```c
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
```

### Código da Parte 1 — `lib/padaria_p1/padaria_p1.c` (sem sincronização)

```c
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
```

### Código da Parte 2 — `lib/padaria_p2/padaria_p2.c` (com mutex)

```c
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
```

### Código da Parte 3 — `lib/padaria_p3/padaria_p3.c` (com semáforos)

```c
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
```

---

## Repositório

```text
https://github.com/Beserrovsky/Atividade-7
```
