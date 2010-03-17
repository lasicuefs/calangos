#include "maleLizard.h"
#include "simdunas.h"

#include "player.h"
#include "guiManager.h"

#define VEL_WALK 1000.0
#define VEL_RUN 5000.0

#define MAXDEGREE 100
#define PROBTHR 80

MaleLizard::MaleLizard(NodePath node) : Lizard(node){ init(); }
const double MaleLizard::BOBBING_WAITING_TIME = 3.0;

MaleLizard::~MaleLizard(){
	Simdunas::get_evt_handler()->remove_hook(PlayerControl::EV_player_bobbing, player_did_bobbing, (void *) this);
}

void MaleLizard::init() {
    Lizard::init();
    //bind_anims(node());
    last_bobbing_done = 0;
    waiting_player_decide = false;

    PT(AnimControl) ac = get_anim_control()->find_anim("fast_bite");
    if(ac != NULL) ac->set_play_rate(1.5);

    maleSymbol = Simdunas::get_window()->load_model(Simdunas::get_window()->get_aspect_2d(), "models/lizards/symbols/male.png");
    maleSymbol.reparent_to(*this);
    maleSymbol.set_scale(2.0);
    float posZ = maleSymbol.get_z();
    maleSymbol.set_z(posZ + 100);
    maleSymbol.set_billboard_point_eye(0);

    set_gender(LizardGender::male);


}

void MaleLizard::act(){
	PT(Player) player = Player::get_instance();

	float bobbing_dist_thr = 3;
	float eat_thr = 0.05;
	float flee_max_dist = 10;
	float distance = (player->get_pos() - get_pos()).length();


	/* Quando esperando a resposta do player... */
	if(waiting_player_decide == true){
		/* Se o player não respondeu dentro do tempo */
		if(ClockObject::get_global_clock()->get_real_time() - last_bobbing_done > BOBBING_WAITING_TIME){
			waiting_player_decide = false;
			Simdunas::get_evt_handler()->remove_hook(PlayerControl::EV_player_bobbing, player_did_bobbing, (void *) this);

			/* Paciência tem limite... Parte para morder o player */
			set_action("fight", true);
		}
	}

	if(is_action_active("flee") || this->get_energia() < 20){
		if(distance < flee_max_dist) flee();
		else {
			energia = 21;
			set_action("walk", true);
		}

		waiting_player_decide = false;
	}
	else if(is_action_active("fight")){
		/* Round One: FIGHT! */
		if(distance < eat_thr) bite();
		else chase();
	}
	else if(distance < bobbing_dist_thr && !waiting_player_decide){
		bob();
		last_bobbing_done = ClockObject::get_global_clock()->get_real_time();
		waiting_player_decide = true;

		Simdunas::get_evt_handler()->add_hook(PlayerControl::EV_player_bobbing, player_did_bobbing, (void *) this);
	}
	else {
		set_action("walk");
		play_action_anims(true);

		Animal::act();
	}
}


void MaleLizard::player_did_bobbing(const Event *theEvent, void *data){
	MaleLizard* this_lizard = (MaleLizard*) data;
	PT(Player) player = Player::get_instance();

	Simdunas::get_evt_handler()->remove_hook(PlayerControl::EV_player_bobbing, player_did_bobbing, (void *) this_lizard);

	this_lizard->waiting_player_decide = false;

	float fight_prob = (this_lizard->get_tamanho_base() - player->get_tamanho_base()) / 2 + 40.0;

	if (rand() % 100 < fight_prob) this_lizard->set_action("fight", true);
	else this_lizard->set_action("flee", true);
}

void MaleLizard::flee(){
	PT(Player) player = Player::get_instance();

	if(!has_other_anim_active("walk")){
		if(!get_anim_control()->is_playing("walk")) get_anim_control()->play("walk");

		/* Comportamento */
		look_at(*player);  //TODO: Corrigir depois para não permitir muito giro.
		set_h(*this,0); // Corrige modelo errado

		//move(0.02);
		this->move(VEL_RUN);
	}

}

void MaleLizard::bob(){
	//set_action("bobbing");
	//play_action_anims();
	if(!get_anim_control()->is_playing("bobbing")) get_anim_control()->play("bobbing");
}

void MaleLizard::wander(){
	float elapsed = TimeControl::get_instance()->get_elapsed_time();
	if(acting && !stay_quiet()){
		if(rand()%PROBTHR == 34) set_h(*this, rand()%MAXDEGREE - (MAXDEGREE/2));
		this->move(VEL_WALK);
	}

}

void MaleLizard::chase(){
	PT(Player) player = Player::get_instance();

	if(!has_other_anim_active("walk")){

		if(!get_anim_control()->is_playing("walk")){
			//set_action("walk");
			//play_action_anims();
			get_anim_control()->play("walk");
		}

		/* Comportamento */
		look_at(*player);  //TODO: Corrigir depois para não permitir muito giro.
		set_h(*this, 180); // Corrige modelo errado

		move(VEL_RUN);
	}

}

void MaleLizard::bite(){
	/* Manter simples por enquanto */
	if(!get_anim_control()->is_playing("fast_bite")){
		//set_action("fast_bite");
		//play_action_anims();
		//get_anim_control()->stop_all();
		get_anim_control()->play("fast_bite");

		Player::get_instance()->be_bited();
        Player::get_instance()->mordida_recebida(this->get_tamanho_base());
		//Player::get_instance()->add_energia_alimento(-5.0);
		GuiManager::get_instance()->piscar_life();
	}
}
