
/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

// Demais includes
#include "basic_io.h"
#include "hal.h"
#include "sm.h"

void task_StateMachine(void *pvParameters);
void task_BlinkLeft(void *pvParameters);
void task_BlinkRight(void *pvParameters);
void task_Blink(void *pvParameters);

TaskHandle_t blinkLeftHandle;
TaskHandle_t blinkRightHandle;

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
		xTaskCreate(task_Blink, "task blink left", 1000, ToggleTurnSignalLeft, 1, &blinkLeftHandle);
		xTaskCreate(task_Blink, "task blink right", 1000, ToggleTurnSignalRight, 1, &blinkRightHandle);
	}

	tuCommand command = getTurnCommand();
	if (command.Alert == 0){
		NEXT_STATE(initialState);
		vTaskDelete(blinkLeftHandle);
		vTaskDelete(blinkRightHandle);
	}
}

STATE(leftState) {
	if (FIRST){
		TurnSignalRight(0);
		xTaskCreate(task_Blink, "task blink left", 1000, ToggleTurnSignalLeft, 1, &blinkLeftHandle);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Alert == 1){
			vTaskDelete(blinkLeftHandle);
			NEXT_STATE(alertState);
		}
		else if (command.TurnCommands == command_Right){
			vTaskDelete(blinkLeftHandle);
			NEXT_STATE(rightState);
		}
		else if (command.TurnCommands == command_None){
			vTaskDelete(blinkLeftHandle);
			NEXT_STATE(initialState);
		}
	}

}

STATE(rightState) {
	if (FIRST){
		TurnSignalLeft(0);
		xTaskCreate(task_Blink, "task blink right", 1000, ToggleTurnSignalRight, 1, &blinkRightHandle);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Alert == 1){
			vTaskDelete(blinkRightHandle);
			NEXT_STATE(alertState);
		}
		else if (command.TurnCommands == command_Left){
			vTaskDelete(blinkRightHandle);
			NEXT_STATE(leftState);
		}
		else if (command.TurnCommands == command_None){
			vTaskDelete(blinkRightHandle);
			NEXT_STATE(initialState);
		}
	}
}


int main_(void) {

	InitHAL();
	INIT(smBlink, initialState);

	vTaskStartScheduler();

	for (;;);
	return 0;
}

void task_Blink(void (*blinkFunction)(void)){
	for (;;){
		blinkFunction();
		vTaskDelay(333);
	}
}


void vApplicationIdleHook(void *pParam){
	EXEC(smBlink);
}
