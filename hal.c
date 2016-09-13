/*
Camada de Abstracao do Hardware (HAL) para execucao no Visual Studio
*/

#include "hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <conio.h>

// IMPORTANTE: fique a vontade para criar outros metodos e variaveis

// Variaveis da Camada de Abstracao do Hardware (HAL)
boolean pinTurnSignal_LEFT;		// Representa o estado do pino de saida do microcontrolador ligado ao sinalizador ESQUERDO
boolean pinTurnSignal_RIGHT;	// Representa o estado do pino de saida do microcontrolador ligado ao sinalizador DIREITO
tuCommand lastCommand;			// Armazena o estado atual dos sinalizadores. Veja tambem: tuCommand

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

			default:

				break;
			}
		}
		else {
			vTaskDelay(1);
		}
	}
}

// Inicializa a Camada de Abstracao de Hardware.
void InitHAL() {
	TurnSignalLeft(0);
	TurnSignalRight(0);
	xTaskCreate(task_Key, "task key", 1000, NULL, 1, NULL);
	xTaskCreate(printTurnSignals, "task print", 1000, NULL, 1, NULL);
}
// Metodo que retorna o estado da  alavanca de comando dos sinalizadores ("alavanda das setas junto ao volante"). Ver tamb�m tuCommand
tuCommand getTurnCommand() {
	return lastCommand;
}

/* Liga ou desliga o sinalizador direito.
- s: estado do sinalizador (TRUE = acende / FALSE = apaga)
*/
void TurnSignalRight(boolean s) {
	pinTurnSignal_RIGHT = s;

	/*moveTo(15, 0);
	if (pinTurnSignal_RIGHT){
		vPrintString("[>>>]");
	}
	else{
		vPrintString("[   ]");
	}*/
}

/* Liga ou desliga o sinalizador esquerdo.
- s: estado do sinalizador (TRUE = acende / FALSE = apaga)
*/
void TurnSignalLeft(boolean s) {
	pinTurnSignal_LEFT = s;

	/*moveTo(0, 0);
	if (pinTurnSignal_LEFT){
		vPrintString("[<<<]");
	}
	else{
		vPrintString("[   ]");
	}*/
}


// Inverte o estado do sinalizador direito. Se estava apagado, acende. Se estava aceso, apaga.
void ToggleTurnSignalRight() {
	TurnSignalRight(!pinTurnSignal_RIGHT);
}

// Inverte o estado do sinalizador direito. Se estava apagado, acende. Se estava aceso, apaga.
void ToggleTurnSignalLeft() {
	TurnSignalLeft(!pinTurnSignal_LEFT);
}

boolean readPinTurnSignal_Left(){
	return pinTurnSignal_LEFT;
}

boolean readPinTurnSignal_Right(){
	return pinTurnSignal_RIGHT;
}

void printTurnSignals(){
	boolean prevLeft = pinTurnSignal_LEFT;
	boolean prevRight = pinTurnSignal_RIGHT;
	boolean first = 1;

	for (;;){
		if (prevLeft != pinTurnSignal_LEFT || first){
			moveTo(0, 0);
			if (pinTurnSignal_LEFT){
				vPrintString("[<<<]");
			}
			else{
				vPrintString("[   ]");
			}

			prevLeft = pinTurnSignal_LEFT;
		}

		if (prevRight != pinTurnSignal_RIGHT || first){
			moveTo(15, 0);
			if (pinTurnSignal_RIGHT){
				vPrintString("[>>>]");
			}
			else{
				vPrintString("[   ]");
			}

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
