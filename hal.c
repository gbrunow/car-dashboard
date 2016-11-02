/*
Camada de Abstracao do Hardware (HAL) para execucao no Visual Studio
*/

#include "hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <conio.h>
#include "queue.h"

// IMPORTANTE: fique a vontade para criar outros metodos e variaveis

// Variaveis da Camada de Abstracao do Hardware (HAL)
boolean pinTurnSignal_LEFT;		// Representa o estado do pino de saida do microcontrolador ligado ao sinalizador ESQUERDO
boolean pinTurnSignal_RIGHT;	// Representa o estado do pino de saida do microcontrolador ligado ao sinalizador DIREITO
boolean pinBreakSignal;
tuCommand lastCommand;			// Armazena o estado atual dos sinalizadores. Veja tambem: tuCommand
xQueueHandle xCommQueue;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

void task_Key(void *pvParameters);

// Esta � a task que recebe os comandos do teclado e atualiza o valor de lastCommand
void task_Key(void *pParam){	
	int key;
	int n;
	for (;;){
		if (_kbhit()){
			key = _getch();
			switch (key){
			case 'l':
				lastCommand.TurnCommands = command_Left;
				break;

			case 'r':
				lastCommand.TurnCommands = command_Right;
				break;

			case ' ':
				lastCommand.TurnCommands = command_None;
				break;

			case 'a':
				lastCommand.Alert = !lastCommand.Alert;
				break;

			case 'i':
				lastCommand.Ignition = !lastCommand.Ignition;
				break;

			case 'b':
				lastCommand.Break = !lastCommand.Break;
				break;

			default:

				break;
			}

			portBASE_TYPE xStatus = xQueueSendToBack(xCommQueue, &lastCommand, xTicksToWait);

			if (xStatus != pdPASS){
				vPrintString("\n\n Fila de comandos cheia.");
			}
		}
		else {
			vTaskDelay(10);
		}
	}
}

void printSignals(void *pParam);

// Inicializa a Camada de Abstracao de Hardware.
void InitHAL() {
	TurnSignalLeft(0);
	TurnSignalRight(0);
	BreakSignal(0);
	xCommQueue = xQueueCreate(10, sizeof(tuCommand));
	xTaskCreate(task_Key, "task key", 1000, NULL, 1, NULL);
	xTaskCreate(printSignals, "task print", 1000, NULL, 1, NULL);
}
// Metodo que retorna o estado da  alavanca de comando dos sinalizadores ("alavanda das setas junto ao volante"). Ver tamb�m tuCommand
tuCommand getTurnCommand() {
	return lastCommand;
}

void getCommand(tuCommand *command){
	tuCommand tmpCommand;
	portBASE_TYPE xStatus = xQueueReceive(xCommQueue, &tmpCommand, xTicksToWait);

	if (xStatus == pdPASS){
		*command = tmpCommand;
	}
}

/* Liga ou desliga o sinalizador direito.
- s: estado do sinalizador (TRUE = acende / FALSE = apaga)
*/
void TurnSignalRight(boolean s) {
	pinTurnSignal_RIGHT = s;
}

/* Liga ou desliga o sinalizador esquerdo.
- s: estado do sinalizador (TRUE = acende / FALSE = apaga)
*/
void TurnSignalLeft(boolean s) {
	pinTurnSignal_LEFT = s;
}

void BreakSignal(boolean s){
	pinBreakSignal = s;
}


// Inverte o estado do sinalizador direito. Se estava apagado, acende. Se estava aceso, apaga.
void ToggleTurnSignalRight() {
	TurnSignalRight(!pinTurnSignal_RIGHT);
}

// Inverte o estado do sinalizador direito. Se estava apagado, acende. Se estava aceso, apaga.
void ToggleTurnSignalLeft() {
	TurnSignalLeft(!pinTurnSignal_LEFT);
}

void ToggleBreakSignal(){
	BreakSignal(!pinBreakSignal);
}



boolean readPinTurnSignal_Left(){
	return pinTurnSignal_LEFT;
}

boolean readPinTurnSignal_Right(){
	return pinTurnSignal_RIGHT;
}

void printSignals(void *pParam){
	boolean prevLeft = pinTurnSignal_LEFT;
	boolean prevRight = pinTurnSignal_RIGHT;
	boolean prevBreak = pinBreakSignal;
	boolean first = 1;

	for (;;){
		if (prevLeft != pinTurnSignal_LEFT || first){
			moveTo(0, 0);
			if (pinTurnSignal_LEFT){
				vPrintString("[<<<");
			}
			else{
				vPrintString("[   ");
			}
		}		

		if (prevRight != pinTurnSignal_RIGHT || first){
			moveTo(19, 0);
			if (pinTurnSignal_RIGHT){
				vPrintString(">>>]");
			}
			else{
				vPrintString("   ]");
			}
			
		}

		if (prevBreak != pinBreakSignal || prevLeft != pinTurnSignal_LEFT || prevRight != pinTurnSignal_RIGHT || first){
			if (pinBreakSignal){
				moveTo(4, 0);
				vPrintString("|||]");
				moveTo(15, 0);
				vPrintString("[|||");
			}
			else {
				moveTo(4, 0);
				vPrintString("   ]");
				moveTo(15, 0);
				vPrintString("[   ");
			}

			prevBreak = pinBreakSignal;
			prevLeft = pinTurnSignal_LEFT;
			prevRight = pinTurnSignal_RIGHT;
		}
		first = 0;
		vTaskDelay(1);
	}
}

void moveTo(int XPos, int YPos){
	COORD Coord;

	Coord.X = XPos;
	Coord.Y = YPos;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Coord);
}
