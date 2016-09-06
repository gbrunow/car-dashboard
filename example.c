
/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

// Demais includes
#include "basic_io.h"
#include "hal.h"
#include "sm.h"

// Esta ser� a sua m�quina de estados
StateMachine smBlink;

// TODO: defina aqui as assinaturas dos estados da m�quina de estados
STATE(initialState);
STATE(alertState);
STATE(leftState);
STATE(rightState);

// TODO: crie aqui o c�digo dos estados da m�quina de estados
STATE(initialState){
	tuCommand command = getTurnCommand();

	if (FIRST){
		TurnSignalLeft(0);
		TurnSignalRight(0);
	}

	if (command.Alert == 1){
		NEXT_STATE(alertState);
	}
	else if (command.TurnCommands == command_Left){
		NEXT_STATE(leftState);
	}
	else if (command.TurnCommands == command_Right){
		NEXT_STATE(rightState);
	}

}
STATE(alertState){
	if (FIRST){
		TurnSignalLeft(0);
		TurnSignalRight(0);
	}

	tuCommand command = getTurnCommand();
	if (command.Alert == 0){
		NEXT_STATE(initialState);
	}
}

STATE(leftState) {
	if (FIRST){
		TurnSignalRight(0);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Alert == 1){
			NEXT_STATE(alertState);
		}
		else if (command.TurnCommands == command_Right){
			NEXT_STATE(rightState);
		}
		else if (command.TurnCommands == command_None){
			NEXT_STATE(initialState);
		}
	}

}

STATE(rightState) {
	if (FIRST){
		TurnSignalLeft(0);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Alert == 1){
			NEXT_STATE(alertState);
		}
		else if (command.TurnCommands == command_Left){
			NEXT_STATE(leftState);
		}
		else if (command.TurnCommands == command_None){
			NEXT_STATE(initialState);
		}
	}
}

void task_StateMachine(void *pvParameters);
void task_BlinkLeft(void *pvParameters);
void task_BlinkRight(void *pvParameters);

int main_(void) {

	xTaskCreate(task_StateMachine, "task state machine", 1000, NULL, 1, NULL);
	xTaskCreate(task_BlinkLeft, "task blink left", 1000, NULL, 1, NULL);
	xTaskCreate(task_BlinkRight, "task blink right", 1000, NULL, 1, NULL);

	vTaskStartScheduler();

	for (;;);
	return 0;
}

// TODO: implemente aqui os codigos das Tasks
void task_BlinkLeft(void *pParam) {

	for (;;) {
		if (COMPARE(smBlink, alertState) || COMPARE(smBlik, leftState)){
			ToggleTurnSignalLeft();
		}
		vTaskDelay(333);
	}
}

void task_BlinkRight(void *pParam) {
	for (;;) {
		if (COMPARE(smBlink, alertState) || COMPARE(smBlik, rightState)){
			ToggleTurnSignalRight();
		}
		vTaskDelay(333);
	}
}

void task_StateMachine(void *pParam) {
	InitHAL();
	INIT(smBlink, initialState);

	for (;;) {
		EXEC(smBlink);
	}
}
