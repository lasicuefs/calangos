#include "player.h"

#include "textureStageCollection.h"
#include "audioController.h"

/* Bloco de constantes
 * ------------------------------------------------------------------------- */
/* Idade, em meses, na qual o lagarto poderá se reproduzir */
#define IDADE_REPRODUTIVA 12
/* Idade, em meses, em que o lagarto deve morrer */
#define IDADE_MORTE 36
/* Energia e hidratação iniciais */
#define ENERGIA_INIT 50.0
#define HIDRATACAO_INIT 50.0

/* Tempo (em meses virtuais) em que o lagarto poderá chegar ao tamanho máximo */
#define MESES_TAMANHO_MAXIMO 36


/* Debugar */
#define DEBUG_PHEALTH 0
/* ------------------------------------------------------------------------- */

/* Refatorando a parte de informações internas do lagarto */
class LizardHealthInfo {
public:
	float ideal_temperature;
	float max_temperature;
	float min_temperature;
	float temperature_range;

	float max_hours_without_feed;
	float max_hours_with_low_hidration;

	float humidity_affects_hidration;
	float humidity_param; // moisture?

	float min_hydration;
	float max_hydration;

	float min_energy; // stamina?
	float energy_cost_low_temperature;
	float energy_cost_high_temperature;

	float thermal_equilibrium_speed;
};

float init_tropidurus_health(){
	LizardHealthInfo health;

	health.ideal_temperature = 38.0;
	health.max_temperature = 50.0;
	health.min_temperature = 15.0;
	health.temperature_range = 2.0;

	health.max_hours_without_feed = 168; /* 7 dias */
	health.max_hours_with_low_hidration = 48; /* 2 dias */
}

/*! Carrega os atributos iniciais relacionados à saúde do lagarto*/
/* Parâmetros da espécie Tropidurus */
const float Player::temperatura_interna_ideal_trop = 38;
const float Player::qnt_h_sem_alimento_trop = 168;
const float Player::qnt_h_baixa_hidrat_trop = 48;
const float Player::umidade_afeta_hidrat_trop = 40;
const float Player::umidade_param_trop = 10;
const float Player::temp_interna_max_trop = 50;
const float Player::temp_interna_min_trop = 15;
const float Player::hidrat_min_trop = 45;
const float Player::energia_min_trop = 5;
const float Player::faixa_tolerancia_tem_interna_trop = 2;
const float Player::gasto_baixa_temp_trop = 0.05;
const float Player::gasto_alta_temp_trop = 0.05;
const float Player::vel_equi_termico_trop = 0.1;

/* Parâmetros da espécie Eurolophosaurus */
const float Player::temperatura_interna_ideal_euro = 36;
const float Player::qnt_h_sem_alimento_euro = 190;
const float Player::qnt_h_baixa_hidrat_euro = 48;
const float Player::umidade_afeta_hidrat_euro = 40;
const float Player::umidade_param_euro = 10;
const float Player::temp_interna_max_euro = 45;
const float Player::temp_interna_min_euro = 13;
const float Player::hidrat_min_euro = 40;
const float Player::energia_min_euro = 3;
const float Player::faixa_tolerancia_tem_interna_euro = 1.5;
const float Player::gasto_baixa_temp_euro = 0.06;
const float Player::gasto_alta_temp_euro = 0.08;
const float Player::vel_equi_termico_euro = 0.1;

/* Parâmetros da espécie Cnemidophorus */
const float Player::temperatura_interna_ideal_cnem = 40;
const float Player::qnt_h_sem_alimento_cnem = 168;
const float Player::qnt_h_baixa_hidrat_cnem = 48;
const float Player::umidade_afeta_hidrat_cnem = 40;
const float Player::umidade_param_cnem = 10;
const float Player::temp_interna_max_cnem = 50;
const float Player::temp_interna_min_cnem = 15;
const float Player::hidrat_min_cnem = 45;
const float Player::energia_min_cnem = 5;
const float Player::faixa_tolerancia_tem_interna_cnem = 2;
const float Player::gasto_baixa_temp_cnem = 0.06;
const float Player::gasto_alta_temp_cnem = 0.06;
const float Player::vel_equi_termico_cnem = 0.1;

/* Carrega os valores inicias para a espécie Tropidurus */
float Player::arrayTropidurus[13] = {temperatura_interna_ideal_trop, qnt_h_sem_alimento_trop, qnt_h_baixa_hidrat_trop, 
		umidade_afeta_hidrat_trop, umidade_param_trop, temp_interna_max_trop, temp_interna_min_trop, hidrat_min_trop,
		energia_min_trop, faixa_tolerancia_tem_interna_trop, gasto_baixa_temp_trop, gasto_alta_temp_trop, vel_equi_termico_trop};

/* Carrega os valores inicias para a espécie Europhosaurus */
float Player::arrayEurolophosaurus[13] = {temperatura_interna_ideal_euro, qnt_h_sem_alimento_euro, qnt_h_baixa_hidrat_euro,
		umidade_afeta_hidrat_euro, umidade_param_euro, temp_interna_max_euro, temp_interna_min_euro, hidrat_min_euro, energia_min_euro,
		faixa_tolerancia_tem_interna_euro, gasto_baixa_temp_euro, gasto_alta_temp_euro, vel_equi_termico_euro};

/* Carrega os valores inicias para a espécie Cnemidosphorus */
float Player::arrayCnemidophorus[13] = {temperatura_interna_ideal_cnem, qnt_h_sem_alimento_cnem, qnt_h_baixa_hidrat_cnem,
		umidade_afeta_hidrat_cnem, umidade_param_cnem, temp_interna_max_cnem, temp_interna_min_cnem, hidrat_min_cnem, energia_min_cnem,
		faixa_tolerancia_tem_interna_cnem, gasto_baixa_temp_cnem, gasto_alta_temp_cnem, vel_equi_termico_cnem};

/*! Carrega informações customizadas da saúde do lagarto (para fase 2) */
void Player::load_custom_health(){
	if(Session::get_instance()->get_level() > 1){
//		nout << "-------------------------------------------" << endl;
//		nout << "Ant Diet" << " : " << properties.ant_diet << endl;
//		nout << "Plant Diet" << " : " << properties.plant_diet << endl;
//		nout << "Others Diet" << " : " << properties.general_diet << endl;
//		nout << "-------------------------------------------" << endl;

		/* Configurações para a fase 2 */
		/* Dá um valor de 0 a 1 para as possíveis escolhas de velocidade */
		velocity_factor = (properties.speed - properties.min_speed) /
								  (properties.max_speed - properties.min_speed);

		/* Configura a temperatura ideal */
		set_temp_interna_ideal(properties.ideal_tempature);

		/* Configura a capacidade de se enterrar */
		/* Seria melhor deixar em properties ao invés de criar uma nova variável? */
		set_bury_ability(properties.bury_ability);
	}
}

void Player::load_health(int especie){
	calc_visual_size_factor();
	calc_atualizacoes_phora();
	lizardEpecie lizard = lizardEpecie(especie);

	if(lizard == tropidurus){
		init_tropidurus_lizard();
	}else if(lizard == eurolophosaurus){
		init_eurolophosaurus_lizard();		
	}else{
		init_cnemidophorus_lizard();
	}

	load_custom_health();

	equi_term_atual = equi_term;
	//valores que independem da espécie do lagarto	
	energia = ENERGIA_INIT;
	hidratacao = HIDRATACAO_INIT;
	energia_alimento = 0;
	energia_alimento_acumulada=0;
	y_fator_umidade = 0;
	fator_umidade = 0;
	letargia = 0;
	//considera o lagarto parado no início do jogo
	gasto_movimento = 1;

	num_atualizacoes_dia = 0;
	soma_energia_dia = 0;
	soma_media_energia_diaria = 0;
	num_dias = 0;
	media_energia_mes = 0;

	/* Passado pelo editor ou hardcoded para outras fases */
	max_size = get_max_lizards_size();
	tamanho_lagarto_real = get_min_size(); // Pelo editor
	tamanho_lagarto_base = calc_tamanho_base(tamanho_lagarto_real);

	idade = 0;
	num_ovos = 0;
}

void Player::load_health(){
	load_health(eurolophosaurus);
}

/* Inicializando lagartos */
void Player::init_eurolophosaurus_lizard(){

	num_horas_alimento = Player::arrayEurolophosaurus[QNT_H_SEM_ALIMENTO];
	temp_interna_ideal = Player::arrayEurolophosaurus[TEMP_INTER_IDEAL];
	gasto_basal = ENERGIA_INIT / (num_horas_alimento * atualizacoes_phora);
	a_fator_umidade = -(HIDRATACAO_INIT / Player::arrayEurolophosaurus[QNT_H_BAIXA_HIDRAT]) / (Player::arrayEurolophosaurus[UMID_AFETA_HIDRAT] - Player::arrayEurolophosaurus[UMID_PARAM]);
	b_fator_umidade = -(a_fator_umidade * (Player::arrayEurolophosaurus[UMID_AFETA_HIDRAT]));
	temp_interna =  Player::arrayEurolophosaurus[TEMP_INTER_IDEAL];
	temp_interna_maxlimite =  Player::arrayEurolophosaurus[TEMP_INT_MAX];
	temp_interna_minlimite = Player::arrayEurolophosaurus[TEMP_INT_MIN];
	hidratacao_minlimite = Player::arrayEurolophosaurus[HIDT_MIN];
	energia_minlimite = Player::arrayEurolophosaurus[ENER_MIN];
	variacao_temp_interna = Player::arrayEurolophosaurus[TOLER_TEMP_INTER];
	gasto_baixa_temp = Player::arrayEurolophosaurus[GASTO_BAIXA_TEMP];
	gasto_alta_temp = Player::arrayEurolophosaurus[GASTO_ALTA_TEMP];
	equi_term = Player::arrayEurolophosaurus[VEL_EQUI_TERM];
}

void Player::init_tropidurus_lizard(){

	num_horas_alimento = Player::arrayTropidurus[QNT_H_SEM_ALIMENTO];
	temp_interna_ideal = Player::arrayTropidurus[TEMP_INTER_IDEAL];
	gasto_basal = ENERGIA_INIT / (num_horas_alimento * atualizacoes_phora);
	a_fator_umidade = -(HIDRATACAO_INIT / Player::arrayTropidurus[QNT_H_BAIXA_HIDRAT]) / (Player::arrayTropidurus[UMID_AFETA_HIDRAT] - Player::arrayTropidurus[UMID_PARAM]);
	b_fator_umidade = -(a_fator_umidade * (Player::arrayTropidurus[UMID_AFETA_HIDRAT]));
	temp_interna =  Player::arrayTropidurus[TEMP_INTER_IDEAL];
	temp_interna_maxlimite =  Player::arrayTropidurus[TEMP_INT_MAX];
	temp_interna_minlimite = Player::arrayTropidurus[TEMP_INT_MIN];
	hidratacao_minlimite = Player::arrayTropidurus[HIDT_MIN];
	energia_minlimite = Player::arrayTropidurus[ENER_MIN];
	variacao_temp_interna = Player::arrayTropidurus[TOLER_TEMP_INTER];
	gasto_baixa_temp = Player::arrayTropidurus[GASTO_BAIXA_TEMP];
	gasto_alta_temp = Player::arrayTropidurus[GASTO_ALTA_TEMP];
	equi_term = Player::arrayTropidurus[VEL_EQUI_TERM];
}

void Player::init_cnemidophorus_lizard(){

	num_horas_alimento = Player::arrayCnemidophorus[QNT_H_SEM_ALIMENTO];
	temp_interna_ideal = Player::arrayCnemidophorus[TEMP_INTER_IDEAL];
	gasto_basal = ENERGIA_INIT / (num_horas_alimento * atualizacoes_phora);
	a_fator_umidade = -(HIDRATACAO_INIT / Player::arrayCnemidophorus[QNT_H_BAIXA_HIDRAT]) / (Player::arrayCnemidophorus[UMID_AFETA_HIDRAT] - Player::arrayCnemidophorus[UMID_PARAM]);
	b_fator_umidade = -(a_fator_umidade * (Player::arrayCnemidophorus[UMID_AFETA_HIDRAT]));
	temp_interna =  Player::arrayCnemidophorus[TEMP_INTER_IDEAL];
	temp_interna_maxlimite =  Player::arrayCnemidophorus[TEMP_INT_MAX];
	temp_interna_minlimite = Player::arrayCnemidophorus[TEMP_INT_MIN];
	hidratacao_minlimite = Player::arrayCnemidophorus[HIDT_MIN];
	energia_minlimite = Player::arrayCnemidophorus[ENER_MIN];
	variacao_temp_interna = Player::arrayCnemidophorus[TOLER_TEMP_INTER];
	gasto_baixa_temp = Player::arrayCnemidophorus[GASTO_BAIXA_TEMP];
	gasto_alta_temp = Player::arrayCnemidophorus[GASTO_ALTA_TEMP];
	equi_term = Player::arrayCnemidophorus[VEL_EQUI_TERM];
}

/* EVENTOS
 * ------------------------------------------------------------------------- */

/* Evento chamado a cada minuto virtual para atualização dos valores */
void Player::event_gasto_energia(const Event*, void *data){
	/* Chama as funções para atualizar a saúde do lagarto */
	//WARNING: Ordem é importante
	player->calc_temp_interna();
	player->calc_letargia();
	player->calc_gasto_temp();
	player->calc_gasto_basal();
	player->calc_gasto_total();
	player->calc_y_fator_umidade();
	player->calc_fator_umidade();
	player->calc_hidratacao();
	player->calc_energia();
	player->display();
	player->atualiza_vector();

	/* Verificação de morte do personagem por energia*/
	if(player->energia < player->energia_minlimite){
		//Informa morte do personagem por desnutrição.
		Session::get_instance()->player_death(1);
	}else{
		/* Verificação de morte do personagem por temperatura interna */
		if(player->temp_interna >= player->temp_interna_maxlimite){
			Session::get_instance()->player_death(3);
		}else {
			//Verifica se a morte foi por baixa temperatura
			if(player->temp_interna < player->temp_interna_minlimite){
				Session::get_instance()->player_death(4);
			}else{
				/* Verificação de morte do lagarto por desidratação */
				if(player->hidratacao < player->hidratacao_minlimite){
					Session::get_instance()->player_death(2);
				}
			}
		}
	}
	player->num_atualizacoes_dia++;
	player->soma_energia_dia = player->soma_energia_dia + player->energia;
}

/*! Obtém o tamanho máximo que o player poderá atingir. Este é o valor que deve
 *  ser especificado no editor de personagens */
float Player::get_max_size(){
	return max_size;
}

float Player::get_min_size(){
	return max_size * 0.2;
}

/*! Obtém o tamanho (comprimento) máximo de lagarto permitido. Em centímetros.
 * Este valor é especificado com o lagarto em sua idade máxima */
float Player::get_max_lizards_size(){
	return 0.6;
}

/*! Obtém o tamanho (comprimento) mínimo de lagarto permitido. Em centímetros.
 *  Este valor é especificado com o lagarto em sua idade máxima */
float Player::get_min_lizards_size(){
	return 0.2;
}

/*! Obtém um fator de 0 a 1, representando o tamanho do lagarto em comparação
 *  com o menor e maior lagartos possíveis */
float Player::get_absolute_size_factor(){
	return (get_tamanho_real() - get_min_lizards_size() * 0.2) /
			(get_max_lizards_size() - get_min_lizards_size());
}

/*! Obtém o fator tamanho baseado no tamanho máximo e mínimo que o lagarto pode
 * atingir. Em outras palavras é relativo ao tamanho máximo do jogador, e não
 * ao tamanho máximo de lagartos existentes */
float Player::get_relative_size_factor(){
	return (get_tamanho_real() - get_min_lizards_size()) /
			(get_max_lizards_size() - get_min_lizards_size());
}

/*! Calcula, com base no menor tamanho possível e maior tamanho possível para
 *  os lagartos, um valor de 0 a 100 (e não de 0 a 1).
 *  Ou seja, retorna uma espécie de valor normalizado com base em um tamanho */
float Player::calc_tamanho_base(float tamanho_real){
	return 100 * (tamanho_real - get_min_size()) / (get_max_size() - get_min_size());
}

/*! Obtem a taxa de crescimento */
float Player::get_taxa_crescimento(){
	return taxa_crescimento;
}

/*! Deprecated. Obtem o "tamanho base" do lagarto. Este é um valor de 0 a 100,
 *  que representa o tamanho do lagarto em relação ao tamanho máximo que este
 *  lagarto pode atingir */
float Player::get_tamanho_base(){
	return this->tamanho_lagarto_base;
}

/*! Obtém o tamanho real do lagarto */
float Player::get_tamanho_real(){
	return this->tamanho_lagarto_real;
}

/*! Calcula o tamanho real do lagarto, além de atualizar várias variáveis
 *  importantes da dinâmica interna do lagarto */
void Player::calc_tamanho_lagarto_real(float media_energia_mensal){
	if(media_energia_mensal < 90){
		this->taxa_crescimento = media_energia_mensal/100;
	} else {
		this->taxa_crescimento = 1;
	}

	float add_tamanho = ((get_max_size() - get_min_size())/MESES_TAMANHO_MAXIMO)*this->taxa_crescimento;

	tamanho_lagarto_real = tamanho_lagarto_real + add_tamanho;
	if(tamanho_lagarto_real > get_max_lizards_size()) tamanho_lagarto_real = get_max_lizards_size();
	tamanho_lagarto_base = calc_tamanho_base(tamanho_lagarto_real);

	this->equi_term_atual = this->equi_term-(this->equi_term*(this->tamanho_lagarto_base/100)*0.8);
}

/*! Calcula o fator pelo qual o tamanho especificado para lagarto deverá ser
 *  multiplicado, uma vez que os modelos vem com escalas distorcidas e por
 *  alguma razão, o flatten_light() torna inativa as animações, mesmo com o
 *  binding posterior. Também fica mais fácil criar distorções entre o tamanho
 *  real e o tamanho aparente do lagarto */
void Player::calc_visual_size_factor(){
	/* Queremos obter o tamanho do lagarto sem nenhum transform */
	CPT(TransformState) transform = get_transform();
	clear_transform();

	/* Calcula o tamanho real do lagarto */
	LPoint3f min, max;
	calc_tight_bounds(min, max);
	float length = (max - min).get_y();

	/* Define o fator */
	visual_size_factor = 1.0 / length;

	nout << "Visual size factor: " << visual_size_factor << endl;

	/* Coloca o transform de volta */
	set_transform(transform);
}

/*! Obtém o tamanho visual do lagarto */
float Player::get_visual_size(){
	nout << visual_size_factor * tamanho_lagarto_real << endl;
	nout << "Tamanho real: " << tamanho_lagarto_real << endl;
	nout << "Visual size Factor: " << visual_size_factor << endl;
	return visual_size_factor * tamanho_lagarto_real;
}


/*! Na passagem do dia, faz a média diária de energia, e armazena a soma.
 *   Essa média servirá para determinar o quanto o lagarto irá crescer na passagem de um mês.*/
void Player::event_pday(const Event*, void *data){
	player->num_dias++;

	/* Calcula a média de energia do dia corrente */
	float media_energia_hoje = player->soma_energia_dia/player->num_atualizacoes_dia;

	/* Soma as médias de energia de cada dia, para na passagem do mês seja feita uma média diária do mês */
	player->soma_media_energia_diaria = player->soma_media_energia_diaria + media_energia_hoje;

	player->num_atualizacoes_dia = 0;
	player->soma_energia_dia = 0;
}

/*! Na passagem do mês, faz a média de energia dos dias do mês que passou.
 *  Essa média servirá para determinar o quanto o lagarto irá crescer na passagem do mês.*/
void Player::event_pmonth(const Event*, void *data){
	/* Calcula a média de energia diária do mês */
	player->media_energia_mes = player->soma_media_energia_diaria/player->num_dias;

	player->num_dias = 0;
	player->soma_media_energia_diaria = 0;

	/* Calcula o quanto o lagarto vai crescer, tomando como parâmetro a média
	 * diária de energia do lagarto durante o mês que passou.*/
	player->calc_tamanho_lagarto_real(player->media_energia_mes);

	/* Ajusta o novo tamanho do personagem */
	player->set_scale(render, player->get_visual_size());
	//	player->set_scale(render, player->tamanho_lagarto_real);
	//player->set_length(100 * player->tamanho_lagarto_real, true);

	/* Aumenta a idade do lagarto */
	player->idade++;

	/* Verifica se lagarto chegou na idades reprodutiva */
	if (player->idade == IDADE_REPRODUTIVA) {
		/* Deveria mandar evento */
		GuiManager::get_instance()->liga_led_estado_reprodutivo();

		if (player->get_specie_name(player->lizard_specie) != "custom") { //caso seja o lagarto personalizado não trocar a textura
			PT(TextureStage) ts = player->find_all_texture_stages().get_texture_stage(0);
			ts->set_mode(TextureStage::M_modulate);
			PT(Texture) t = TexturePool::load_texture("models/lizards/"
					+ player->get_specie_name(player->lizard_specie)
					+ "/male/texture.jpg");
			player->set_texture(ts, t, 2);
		}
		player->lizard_gender = Player::male;
	}
	/* Verifica se o personagem chegou na idade máxima */
	if (player->idade >= IDADE_MORTE) {
		Session::get_instance()->player_death(5);
	}
}

/* ------------------------------------------------------------------------- */
/* CÁLCULOS
 * ------------------------------------------------------------------------- */

/*! Calcula o valor de energia do lagarto
 *  A energia do lagarto é dada por:
 *  Energia Atual - Gasto Total + Energia Adquirida por ingestão de alimentos */
void Player::calc_energia(){

	double energia_temp = 0;
	/* Calcula a energia e faz o corte se necessário */
	energia_temp = this->energia - this->gasto_total + this->energia_alimento;

	if(energia_temp < 100) this->energia = energia_temp;
	else this->energia = 100;

	/* Energia do alimento já consumida */
	this->energia_alimento = 0;

}

/*! Calcula o gasto de energia por temperatura do lagarto. */
void Player::calc_gasto_temp(){

	/* Se a temperatura interna estiver abaixo da faixa de tolerância */
	if(temp_interna < (this->temp_interna_ideal  - this->variacao_temp_interna)) {
		this->gasto_temp = 1/(1+((this->temp_interna_ideal  - this->variacao_temp_interna) - (this->temp_interna))*this->gasto_baixa_temp);
	}
	else {
		/* Se a temperatura interna estiver acima da faixa de tolerância */
		if(temp_interna > (this->temp_interna_ideal  + this->variacao_temp_interna)){
			this->gasto_temp = 1+(this->temp_interna - (this->temp_interna_ideal  + this->variacao_temp_interna))*this->gasto_alta_temp;
		}
		else {
			/* Temperatura na faixa de tolerância */
			this->gasto_temp = 1;
		}
	}
}

/*! Calcula o gasto de energia do lagarto.
 * O gasto de energia do lagarto é dado por:
 * Gasto Total = Gasto Basal * Gasto por Temperatura */
void Player::calc_gasto_total(){
	this->gasto_total = this->gasto_basal*this->gasto_temp*this->gasto_movimento;
}

/*! Calcula o gasto de energia inerente ao lagarto */
void Player::calc_gasto_basal(){
	this->gasto_basal = (ENERGIA_INIT/(num_horas_alimento*atualizacoes_phora))*(1+(5*this->tamanho_lagarto_base/100));
}

/*! Calcula a temperatura interna do lagarto */
void Player::calc_temp_interna(){
	float temp_toca = MicroClima::get_instance()->get_temp_toca_sector();
	float temp_solo = MicroClima::get_instance()->get_temp_solo_sector();

	if(in_toca){
		temp_interna = this->temp_interna + this->equi_term_atual*(temp_toca - this->temp_interna);
		AudioController::get_instance()->warning_temp(temp_interna, temp_toca, temp_interna_minlimite, temp_interna_maxlimite);
	}
	else {
		/* Se tiver enterrado diminui a inércia térmica */
		float fator_diminuir_inercia = is_buried() ? 1.8 : 1.0;
		/* Variação de temperatura com inércia térmica */
		float dtemp = (equi_term_atual * fator_diminuir_inercia) * (temp_solo - temp_interna);
		temp_interna = temp_interna + dtemp;

		AudioController::get_instance()->warning_temp(temp_interna, temp_solo, temp_interna_minlimite, temp_interna_maxlimite);
	}
}

/*! Calcula a letargia do lagarto
 * 0 - Nenhuma letargia (temperatura interna maior que a ideal)
 * 1 - Total letargia - culmina na morte do lagarto */
void Player::calc_letargia(){
	double menor_temp_ideal = this->temp_interna_ideal - this->variacao_temp_interna;

	//TODO: Chamar morte do lagarto quando a letargia for = 1?
	if(this->temp_interna > menor_temp_ideal) letargia = 0;
	else if(this->temp_interna < temp_interna_minlimite) letargia = 1;
	else letargia = 1 - ( (this->temp_interna - temp_interna_minlimite) / (menor_temp_ideal - temp_interna_minlimite) );
}



/*! Calcula a hidratação do lagarto
 * A hidratação do lagarto é dada por:
 * Hidratação = Hidratação - Fator Umidade + Hidratação adquirida */
void Player::calc_hidratacao(){

	double hidratacao_temp = this->hidratacao - this->fator_umidade + this->hidratacao_alimento;
	if(hidratacao_temp < 100) this->hidratacao = hidratacao_temp;
	else this->hidratacao = 100;

	/* Hidratação já consumida */
	this->hidratacao_alimento = 0;

	AudioController::get_instance()->warning_hydrat(hidratacao, hidratacao_minlimite);

}

/*! Calcula o valor de decaímento caso as atualizações fossem de hora em hora */
void Player::calc_y_fator_umidade(){

	double umidade_temp = MicroClima::get_instance()->get_umidade_relativa_sector();
	if(umidade_temp < 40) this->y_fator_umidade = a_fator_umidade*umidade_temp + b_fator_umidade;
	else this->y_fator_umidade = 0;
}

/*! Calcula o fator umidade
 * Armazena em quanto o valor da hidratação vai cair a cada atualização de variáveis
 * com base no valor da umidade relativa da microregião em que o lagarto se encontra */
void Player::calc_fator_umidade(){
	this->fator_umidade = this->y_fator_umidade/this->atualizacoes_phora;
}

void Player::calc_atualizacoes_phora(){
	this->atualizacoes_phora = 60*((3*60.0)/(24*60)); //3 min por dia
	//this->atualizacoes_phora = 60*(TimeControl::get_instance()->get_seconds_min());
}


/*!Atualiza os valores dos vetores utiliazados para geração dos gráfico*/
void Player::atualiza_vector(){

	Vetores::get_instance()->addElementVectorTemperaturaLagarto(this->temp_interna);
	Vetores::get_instance()->addElementVectorHidratacaoLagarto(this->hidratacao);
	Vetores::get_instance()->addElementVectorEnergia(this->energia);
	Vetores::get_instance()->addElementVectorAlimentacao(this->energia_alimento_acumulada);
	this->energia_alimento_acumulada=0;
	Vetores::get_instance()->addElementVectorGastoEnergeticoTotal(this->gasto_total);

	Vetores::get_instance()->addElementVectorTemperaturaAr(MicroClima::get_instance()->get_temp_ar_sector());
	if(in_toca){
		Vetores::get_instance()->addElementVectorTemperaturaSolo(MicroClima::get_instance()->get_temp_solo_sector());
	} else {
		Vetores::get_instance()->addElementVectorTemperaturaSolo(MicroClima::get_instance()->get_temp_toca_sector());
	}
	Vetores::get_instance()->addElementVectorUmidadeAmbiente(MicroClima::get_instance()->get_umidade_relativa_sector());

	Vetores::get_instance()->addElementVectorTempo(TimeControl::get_instance()->get_hora_generica());

#if(DEBUG_PHEALTH)
	cout << endl << "A temperatura interna do lagarto eh: " << this->temp_interna << endl;
	cout << endl << "A temperatura interna do lagarto adicionada ao vetor foi: " << vetores->getLastElementTemperaturaLagarto() << endl;
	cout << endl << "O tamanho do vetor de temperatura interna do lagarto eh: " << vetores->getSizeVectorTemperaturaLagarto() << endl;

	cout << endl << "A temperatura do ambiente eh: " << MicroClima::get_instance()->get_temp_ar_sector() << endl;
	cout << endl << "A temperatura do ambiente adicionada ao vetor foi: " << vetores->getLastElementTemperaturaAr() << endl;
	cout << endl << "O tamanho do vetor de temperatura do ambiente eh: " << vetores->getSizeVectorTemperaturaAr() << endl;
#endif
}

/* ------------------------------------------------------------------------- */
/* GETTERS
 * ------------------------------------------------------------------------- */

//TODO: Realmente precisa disso aqui? */

/*! Obtém o valor de energia do alimento */
double Player::get_energia_alimento(){
	return this->energia_alimento;
}

/*! Obtém o valor de hidratação do alimento */
double Player::get_hidratacao_alimento(){
	return this->hidratacao_alimento;
}

/*! Obtém o valor de hidratação do lagarto */
double Player::get_hidratacao(){
	return this->hidratacao;
}

/*! Obtém o valor de gasto de energia inerente ao lagarto */
double Player::get_gasto_basal(){
	return this->gasto_basal;
}

/*! Obtém o valor de gasto de energia relativo à temperatura */
double Player::get_gasto_temp(){
	return this->gasto_temp;
}

/*! Obtém o valor total em gastos */
double Player::get_gasto_total(){
	return this->gasto_total;
}

/*! Obtém o valor de energia do lagarto */
double Player::get_energia(){
	return this->energia;
}

/*! Obtém o valor de temperatura interna do lagarto */
double Player::get_temp_interna(){
	return this->temp_interna;
}

/*! Obtém o valor ideal de temperatura interna do lagarto */
double Player::get_temp_interna_ideal(){
	return this->temp_interna_ideal;
}

/*! Obtém o valor máximo aceitável de temperatura interna do lagarto */
double Player::get_temp_interna_maxlimite(){
	return this->temp_interna_maxlimite;
}

/*! Obtém o valor mínimo aceitável de temperatura interna do lagarto */
double Player::get_temp_interna_minlimite(){
	return this->temp_interna_minlimite;
}

/*! Obtém o valor mínimo aceitável de temperatura interna do lagarto */
double Player::get_letargia(){
	return this->letargia;
}

/*! Obtém o valor mínimo aceitável de hidratação do lagarto */
double Player::get_hidratacao_minlimite(){
	return this->hidratacao_minlimite;
}

/*! Obtém o valor mínimo aceitável de energia do lagarto */
double Player::get_energia_minlimite(){
	return this->energia_minlimite;
}


double Player::get_environment_temp(){
	if(in_toca){
		return MicroClima::get_instance()->get_temp_toca_sector();
	}else{
		return MicroClima::get_instance()->get_temp_solo_sector();
	}
}

int Player::get_num_atualizacoes_dia(){
	return this->num_atualizacoes_dia;
}

int Player::get_num_dias(){
	return this->num_dias;
}

float Player::get_soma_energia_dia(){
	return this->soma_energia_dia;
}


float Player::get_soma_media_energia_diaria(){
	return this->soma_media_energia_diaria;
}

float Player::get_media_energia_mes(){
	return this->media_energia_mes;
}

int Player::get_idade(){
	return this->idade;
}

bool Player::get_estado_reprodutivo(){
	if(this->idade >= IDADE_REPRODUTIVA){
		return true;
	}else{
		return false;
	}
}

int Player::get_num_ovos(){
	return this->num_ovos;
}

/* ------------------------------------------------------------------------- */
/* SETTERS
 * ------------------------------------------------------------------------- */

/*! Define o valor de energia do alimento */
void Player::set_energia_alimento(double nova_energia_alimento){
	this->energia_alimento = nova_energia_alimento;
}

/*! Define o valor de hidratação do alimento */
void Player::set_hidratacao_alimento(double nova_hidratacao_alimento){
	this->hidratacao_alimento = nova_hidratacao_alimento;
}

/*! Define o valor de hidratação do lagarto */
void Player::set_hidratacao(double nova_hidratacao){
	if(nova_hidratacao > 100) hidratacao = 100;
	if(nova_hidratacao < 0) hidratacao = 0;
	this->hidratacao = nova_hidratacao;
}

/*! Define o valor de energia do lagarto */
void Player::set_energia(double nova_energia){
	this->energia = nova_energia;
}

/*! Define o valor de temperatura interna do lagarto */
void Player::set_temp_interna(double nova_temp_interna){
	this->temp_interna = nova_temp_interna;
}

/*! Define o valor ideal de temperatura interna do lagarto */
void Player::set_temp_interna_ideal(double nova_temp_interna_ideal){
	this->temp_interna_ideal = nova_temp_interna_ideal;
}

/*! Define o valor máximo aceitável de temperatura interna do lagarto */
void Player::set_temp_interna_maxlimite(double nova_temp_interna_maxlimite){
	this->temp_interna_maxlimite = nova_temp_interna_maxlimite;
}

/*! Define o valor mínimo aceitável de temperatura interna do lagarto */
void Player::set_temp_interna_minlimite(double nova_temp_interna_minlimite){
	this->temp_interna_minlimite = nova_temp_interna_minlimite;
}

/*! Define o valor mínimo aceitável de hidratação do lagarto */
void Player::set_hidratacao_minlimite(double nova_hidratacao_minlimite){
	this->hidratacao_minlimite = nova_hidratacao_minlimite;
}

/*! Define o valor mínimo aceitável de energia do lagarto */
void Player::set_energia_minlimite(double nova_energia_minlimite){
	this->energia_minlimite = nova_energia_minlimite;
}

/* Vai fazer o gasto do lagarto igual ao basal */
void Player::set_lagarto_parado(){
	this->gasto_movimento = 1;
}

const float min_mult = 1.2;	 /* Mínima velocidade andando  */
const float max_mult = 2.0;  /* Máxima velocidade correndo */

float get_cost_multiplier(float speed){
	float min_speed_walking = PlayerProperties::min_speed;
	float max_speed_running = PlayerProperties::max_speed * 10;

	float tgteta = (max_mult - min_mult) / (max_speed_running - min_speed_walking);
	return (tgteta * (speed - min_speed_walking) + min_mult);
}

/* Vai fazer o gasto do lagarto 20% maior que o basal */
void Player::set_lagarto_andando(){
	this->gasto_movimento = 1.2;

	if(Session::get_instance()->get_level() > 1){
		this->gasto_movimento = get_cost_multiplier(get_speed_walking());
		//nout << gasto_movimento << endl;
	}
}

/*! Vai fazer o gasto do lagarto 50% maior que o basal */
void Player::set_lagarto_correndo(){
	this->gasto_movimento = 1.5;

	if(Session::get_instance()->get_level() > 1){
		this->gasto_movimento = get_cost_multiplier(get_speed_running());
		//nout << gasto_movimento << endl;
		//this->gasto_movimento = (this->gasto_movimento * (1.0 + velocity_factor)) - (range * 0.5);
	}
}

/* ------------------------------------------------------------------------- */
/* "ADDERS"
 * ------------------------------------------------------------------------- */

/*! Adiciona um valor de energia relativo ao alimento em questão */

void Player::add_energia_alimento(double ganho_energia_alimento){
	if (ganho_energia_alimento < -0.1) {
		AudioController::get_instance()->heart_beat(energia, energia_minlimite);
		AudioController::get_instance()->only_play(AudioRepository::PREDATOR_HIT);
	}

	this->energia_alimento = this->energia_alimento + ganho_energia_alimento;
	this->energia_alimento_acumulada+=ganho_energia_alimento;
}

/*! Adiciona um valor de hidratação relativo ao alimento em questão */
void Player::add_hidratacao_alimento(double ganho_hidratacao_alimento){
	this->hidratacao_alimento = this->hidratacao_alimento + ganho_hidratacao_alimento;
}

void Player::add_ovos(){
	this->num_ovos++;
}
/* ------------------------------------------------------------------------- */
/* "OUTROS"
 * ------------------------------------------------------------------------- */

void Player::mordida_recebida(int tamanho_lagarto_base){
	//energia e hidratação que o lagarto retira do outro em uma mordida é igual a 5% do seu temanho(0-100)
	float ener_hidr_perdida = tamanho_lagarto_base/20;

	if (ener_hidr_perdida > 0) {
		AudioController::get_instance()->heart_beat(energia, energia_minlimite);
	}

	this->energia = this->energia - ener_hidr_perdida;
	//this->hidratacao = this->hidratacao - ener_hidr_perdida;
	GuiManager::get_instance()->piscar_life();
}
