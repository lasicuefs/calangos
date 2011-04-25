/*
 * indicatorsScreen.cxx
 *
 *  Created on: Mar 6, 2011
 *      Author: jonatas
 */

#include "indicatorsScreen.h"
#include "simdunas.h"
#include "mouseButton.h"
#include "calangosMenuManager.h"

IndicatorsScreen::IndicatorsScreen(PT(ScreenManager) manager) : Screen(manager) {
	load();
	hide();
}

IndicatorsScreen::~IndicatorsScreen() {}

void IndicatorsScreen::load() {
	img_indicadores = Simdunas::get_window()->load_model(Simdunas::get_window()->get_render_2d(), "models/gui/icones.png");
	img_indicadores.set_scale(0.1, 0.001, 0.15);
	img_indicadores.set_pos(0.0, 0.0, 0.0);

    configure_default_back_button(((CalangosMenuManager* ) manager.p())->get_instructions_screen());
}

void IndicatorsScreen::unload() {
	img_indicadores.remove_node();
	np_btn_back.remove_node();
	btn_back = NULL;
}

void IndicatorsScreen::show() {
	img_indicadores.show();
	np_btn_back.show();
}

void IndicatorsScreen::hide() {
	img_indicadores.hide();
	np_btn_back.hide();
}
