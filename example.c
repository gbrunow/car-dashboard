
/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

// Demais includes
#include "basic_io.h"
#include "hal.h"
#include "sm.h"

void task_StateMachine(void *pvParameters);
void task_SMBlink(void *pvParameters);
void task_SMBreak(void *pvParameters);
void task_Blink(void *pvParameters);

TaskHandle_t blinkLeftHandle = NULL;
TaskHandle_t blinkRightHandle = NULL;

// TODO: defina aqui as assinaturas dos estados da m�quina de estados
STATE(initialState);
STATE(turnedOnState);
STATE(alertState);
STATE(leftState);
STATE(rightState);

// TODO: crie aqui o c�digo dos estados da m�quina de estados
STATE(initialState){
	tuCommand command = getTurnCommand();

	if (FIRST){
		if (blinkLeftHandle != NULL){
			vTaskDelete(blinkLeftHandle);
			blinkLeftHandle = NULL;
		}
		if (blinkRightHandle != NULL){
			vTaskDelete(blinkRightHandle);
			blinkRightHandle = NULL;
		}

		TurnSignalLeft(0);
		TurnSignalRight(0);
	}
	else {

		if (command.Alert == 1){
			NEXT_STATE(alertState);
		}
		else if (command.Ignition == 1){
			NEXT_STATE(turnedOnState);
		}
	}

}

STATE(turnedOnState){

	if (FIRST){
		if (blinkLeftHandle != NULL){
			vTaskDelete(blinkLeftHandle);
			blinkLeftHandle = NULL;
		}
		if (blinkRightHandle != NULL){
			vTaskDelete(blinkRightHandle);
			blinkRightHandle = NULL;
		}

		TurnSignalLeft(0);
		TurnSignalRight(0);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Alert == 1){
			NEXT_STATE(alertState);
		}
		else if (command.Ignition == 1){
			switch (command.TurnCommands){
			case command_Left:
				NEXT_STATE(leftState);
				break;
			case command_Right:
				NEXT_STATE(rightState);
				break;
			default:
				//stay on this state since igntion is on
				break;
			}
		}
		else {
			NEXT_STATE(initialState);
		}
	}
}

STATE(alertState){
	if (FIRST){
		if (blinkLeftHandle != NULL){
			vTaskDelete(blinkLeftHandle);
			blinkLeftHandle = NULL;
		}
		if (blinkRightHandle != NULL){
			vTaskDelete(blinkRightHandle);
			blinkRightHandle = NULL;
		}

		TurnSignalLeft(0);
		TurnSignalRight(0);		

		xTaskCreate(task_Blink, "task blink left", 1000, ToggleTurnSignalLeft, 1, &blinkLeftHandle);
		xTaskCreate(task_Blink, "task blink right", 1000, ToggleTurnSignalRight, 1, &blinkRightHandle);
	}
	else {
		tuCommand command = getTurnCommand();
		if (command.Alert == 0){
			NEXT_STATE(initialState);			
		}
	}
}

STATE(leftState) {
	if (FIRST){
		if (blinkLeftHandle != NULL){
			vTaskDelete(blinkLeftHandle);
			blinkLeftHandle = NULL;
		}
		if (blinkRightHandle != NULL){
			vTaskDelete(blinkRightHandle);
			blinkRightHandle = NULL;
		}

		TurnSignalRight(0);
		xTaskCreate(task_Blink, "task blink left", 1000, ToggleTurnSignalLeft, 1, &blinkLeftHandle);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Alert == 1){
			NEXT_STATE(alertState);
		}
		else if (command.Ignition == 1){
			switch (command.TurnCommands)
			{
			case command_Right:
				NEXT_STATE(rightState);
				break;
			case command_None:
				NEXT_STATE(initialState);
				break;
			default:
				break;
			}
		}
		else {
			NEXT_STATE(initialState);
		}
	}
}

STATE(rightState) {
	if (FIRST){
		if (blinkLeftHandle != NULL){
			vTaskDelete(blinkLeftHandle);
			blinkLeftHandle = NULL;
		}
		if (blinkRightHandle != NULL){
			vTaskDelete(blinkRightHandle);
			blinkRightHandle = NULL;
		}

		TurnSignalLeft(0);
		xTaskCreate(task_Blink, "task blink right", 1000, ToggleTurnSignalRight, 1, &blinkRightHandle);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Alert == 1){
			NEXT_STATE(alertState);
		}
		else if (command.Ignition == 1){
			switch (command.TurnCommands)
			{
				case command_Left:
					NEXT_STATE(leftState);
					break;
				case command_None:
					NEXT_STATE(initialState);
					break;
				default:
					break;
			}
		} 
		else {
			NEXT_STATE(initialState);
		}
	}
}

STATE(breakOff);
STATE(breakOn);

STATE(breakOff){
	if (FIRST){
		BreakSignal(0);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Ignition == 1 && command.Break == 1){
			NEXT_STATE(breakOn);
		}
	}
}

STATE(breakOn){
	if (FIRST){
		BreakSignal(1);
	}
	else {
		tuCommand command = getTurnCommand();

		if (command.Ignition == 0 || command.Break == 0){
			NEXT_STATE(breakOff);
		}
	}
}

int main_(void) {
	InitHAL();
	xTaskCreate(task_StateMachine, "task state machine", 1000, NULL, 1, NULL);
	//xTaskCreate(task_SMBlink, "task blink state machine", 1000, NULL, 1, NULL);
	//xTaskCreate(task_SMBreak, "task break state machine", 1000, NULL, 1, NULL);

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

void task_StateMachine(void *pParam){
	StateMachine smBlink;
	StateMachine smBreak;
	INIT(smBlink, initialState);
	INIT(smBreak, breakOff);

	for (;;) {
		EXEC(smBlink);
		EXEC(smBreak);
	}
}

/*void task_SMBlink(void *pParam){
	StateMachine sm;
	INIT(sm, initialState);

	for (;;) {
		EXEC(sm);
	}
}


void task_SMBreak(void *pParam){
	StateMachine sm;
	INIT(sm, breakOff);

	for (;;) {
		EXEC(sm);
	}
}*/
void vApplicationIdleHook(void *pParam){
	// EXEC(smBlink);
}
