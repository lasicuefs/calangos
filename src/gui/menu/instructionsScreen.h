/*
 *  instructionsScreen.h
 *  Created on: 05/03/2011
 *  Author: jonatas
 */

#ifndef INSTRUCTIONSSCREEN_H
#define INSTRUCTIONSSCREEN_H

#include "screenManager.h"
#include "screen.h"
#include "button.h"

#define ACTION(mn) void mn(); \
static void mn(const Event*, void* d){ ((PT(InstructionsScreen))(InstructionsScreen*)d)->mn(); }

class InstructionsScreen : public Screen {
public:
	InstructionsScreen(PT(ScreenManager) manager);
	~InstructionsScreen();

	void load();
	void unload();
	void show();
	void hide();

	void default_button_config(PT(Button) button, NodePath &np,
			const string &text, float z, EventCallbackFunction *action);

private:

	/* Componentes do Menu */
	PT(Button) btn_controles;	 	NodePath np_btn_controles;		 ACTION(controles_action);
	PT(Button) btn_indicadores; 	NodePath np_btn_indicadores;	 ACTION(indicadores_action);
	PT(Button) btn_voltar; 			NodePath np_btn_voltar;			 ACTION(voltar_action);
};

#undef ACTION

#endif
