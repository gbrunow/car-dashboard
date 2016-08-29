
/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

// Demais includes
#include "basic_io.h"
#include "hal.h"
#include "sm.h"

// Esta será a sua máquina de estados
StateMachine smBlink;

// TODO: defina aqui as assinaturas dos estados da máquina de estados
	STATE(...);
	STATE(...);

// TODO: crie aqui o código dos estados da máquina de estados
	STATE(...) {
		// TODO: adicione aqui o código de um dos estados
	}

	STATE(...) {
		// TODO: adicione aqui o código de um dos estados
	}

// TODO: implemente aqui os codigos das Tasks
void task_BlinkLeft(void *pParam) {
	// TODO: adicione aqui o código da task
}

void task_BlinkRight(void *pParam) {
	// TODO: adicione aqui o código da task
}

void task_StateMachine(void *pParam) {
	// TODO: adicione aqui o código da task que executa a máquina de estados
}

int main_(void)
{
	// TODO: adicione aqui o código que executa ao iniciar
	return 0;
}

