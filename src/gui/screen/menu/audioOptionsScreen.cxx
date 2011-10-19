/*
 *  audioOptionsScreen.cxx
 *  Created on: Oct 19, 2011
 *  Author: jonatas
 */

#include "audioOptionsScreen.h"
#include "fontPool.h"

#include "mouseButton.h"

#include "simdunas.h"
#include "calangosMenuManager.h"
#include "audioController.h"

/*! O menu é carregado e escondido. O manager é responsável por escolher o menu */
AudioOptionsScreen::AudioOptionsScreen(PT(ScreenManager) manager) : Screen(manager){
	load();
	hide();
}

AudioOptionsScreen::~AudioOptionsScreen() {
	unload();
}

void AudioOptionsScreen::load(){
	cout<<"foi?"<<endl;
	lb_titulo = new TextNode("txt_titulo");
	lb_titulo->set_text("Opções de Áudio");
	lb_titulo->set_font(manager->get_default_font());
	np_lb_titulo = get_root().attach_new_node(lb_titulo);
	np_lb_titulo.set_scale(0.2);
	np_lb_titulo.set_x(-lb_titulo->get_width()*np_lb_titulo.get_sx()*0.5);
	np_lb_titulo.set_z(0.7);
	np_lb_titulo.set_color(0.87, 0.72, 0.52);

	lb_efeitos_sonoros = new TextNode("lb_efeitos_sonoros");
	lb_efeitos_sonoros->set_text("Efeitos Sonoros");
	lb_efeitos_sonoros->set_font(manager->get_default_font());
	np_lb_efeitos_sonoros = get_root().attach_new_node(lb_efeitos_sonoros);
	np_lb_efeitos_sonoros.set_pos(-0.85, 0.0, 0.3);
	np_lb_efeitos_sonoros.set_scale(0.13);
	np_lb_efeitos_sonoros.set_color(1, 1, 1, 1, 0);

	slide = new PGSliderBar("slid");
	slide->set_range(0, 100);
	slide->setup_slider(false, 1.0, 0.06, 0.01);
	slide->set_value(100);
	np_slide = get_root().attach_new_node(slide);
	np_slide.set_scale(0.5, 1.0, 1.0);
	np_slide.set_pos(0, 0.0, 0.27);
	event_handler->add_hook(slide->get_adjust_event(), slide_action, this);

	img_btn_mais = window->load_model(get_root(), "models/buttons/mais");
	img_btn_mais.detach_node();
	btn_mais = new PGButton("mais");
	btn_mais->setup(img_btn_mais);
	np_btn_mais = get_root().attach_new_node(btn_mais);
	np_btn_mais.set_scale(0.12);
	np_btn_mais.set_pos(-0.53, 0.0, 0.28);
	btn_mais->set_frame(-0.4, 0.4, -0.4, 0.4);
	event_handler->add_hook(btn_mais->get_click_event(MouseButton::one()), increase_volume_action, this);

	img_btn_menos = window->load_model(get_root(), "models/buttons/menos");
	img_btn_menos.detach_node();
	btn_menos = new PGButton("menos");
	btn_menos->setup(img_btn_menos);
	np_btn_menos = get_root().attach_new_node(btn_menos);
	np_btn_menos.set_scale(0.12);
	np_btn_menos.set_pos(-1.15, 0.0, 0.28);
	btn_menos->set_frame(-0.4, 0.4, -0.4, 0.4);
	event_handler->add_hook(btn_menos->get_click_event(MouseButton::one()), decrease_volume_action, this);

	configure_default_back_button(((CalangosMenuManager*)(manager.p()))->get_options_screen());
}

void AudioOptionsScreen::slide_action() {
	float value = slide->get_value();
	AudioController::get_instance()->get_audio_repository()->set_sfx_volume_percent(value / 100.0);
}

void AudioOptionsScreen::increase_volume_action() {
	cout<<"aqui?"<<endl;
	float value = slide->get_value();
	cout<<"value = "<<value<<endl;
	if (value == 100) return;
	value += 1;
	AudioController::get_instance()->get_audio_repository()->set_sfx_volume_percent(value / 100.0);
	slide->set_value(value);
}

void AudioOptionsScreen::decrease_volume_action() {
	cout<<"aqui?3"<<endl;
	float value = slide->get_value();
	cout<<"value = "<<value<<endl;
	if (value == 0) return;
	value -= 1;
	AudioController::get_instance()->get_audio_repository()->set_sfx_volume_percent(value / 100.0);
	slide->set_value(value);
}

void AudioOptionsScreen::unload() {
	np_lb_titulo.remove_node();
	np_btn_mais.remove_node();
	np_btn_menos.remove_node();
	np_slide.remove_node();
	np_btn_back.remove_node();
	lb_titulo = NULL;
	lb_efeitos_sonoros = NULL;
	slide = NULL;
	btn_mais = NULL;
	btn_menos = NULL;
	btn_back = NULL;
}
