#include "microClima.h"


#define ALFA_SOLO 0.0425
#define BETA_SOLO 3.9

#define ALFA_TOCA 0.15
#define BETA_TOCA 26

#define ALFA_UMIDADE -3.9
#define BETA_UMIDADE 158

#define RED_SOMBRA 0.05

#define PI 3.141592653589793

#define DEBUG false

TypeHandle MicroClima::_type_handle;

bool MicroClima::instanceFlag = false;
PT(MicroClima) MicroClima::single = NULL;

MicroClima::~MicroClima(){
	instanceFlag = false;
}

MicroClima::MicroClima(){

	/*Inicializa as variáveis com os mesmos valores das variáveis ambientais de climaTempo.
	*É como inicializar o lagarto no meio de uma duna, sem sombras. Isso é corrigido rápidamente
	*se o lagarto estiver em outro lugar.*/
	temp_ar_sector = ClimaTempo::get_instance()->get_temp_ar();
	temp_solo_sector = ClimaTempo::get_instance()->get_temp_solo();
	temp_toca_sector = ClimaTempo::get_instance()->get_temp_toca();
	umidade_relativa_sector = ClimaTempo::get_instance()->get_umi_rel();

	//recebe evento de mudana de setor do lagarto
	Simdunas::get_evt_handler()->add_hook(PlayerControl::get_instance()->EV_player_move, event_player_hour_move, this);
	//mudanca de hora, tem que atualizar as variveis tambem
	Simdunas::get_evt_handler()->add_hook(TimeControl::get_instance()->EV_pass_hour, event_player_hour_move, this);
}

void MicroClima::event_player_hour_move(const Event*, void *data){

	/*Sempre iguala as variáveis dessa classe com as variáveis ambientais da classe climaTempo.
	* Elas só serão alteradas se o lagarto estiver em uma sombra ou em uma toca.*/
	MicroClima::get_instance()->temp_ar_sector = ClimaTempo::get_instance()->get_temp_ar();
	MicroClima::get_instance()->temp_solo_sector = ClimaTempo::get_instance()->get_temp_solo();
	MicroClima::get_instance()->temp_toca_sector = ClimaTempo::get_instance()->get_temp_toca();
	MicroClima::get_instance()->umidade_relativa_sector = ClimaTempo::get_instance()->get_umi_rel();

	/*se o player est� na sombra e é entre 5h e 19h, atualiza as vari�veis*/
	if((World::get_default_world()->get_terrain()->get_shadows()->is_in_shadow(*Player::get_instance(), 0.1)) && (TimeControl::get_instance()->get_hora() > 6) && (TimeControl::get_instance()->get_hora() < 20)){
		
		if(DEBUG){
			cout << "\n Atualizando Microclima" << endl;
		}

		/*Fatores calculados para uma redução razoável e válida dos valores das variáveis ambientais, em relação
		*aos valores calculados na classe climaTempo.*/
		float r = RED_SOMBRA; 
		double a = r/(cos(5*PI/14)-1);
		double b = 1-r-a;

		//calculando temperatura do ar do microclima. Até 15h, curva crescente
		if(TimeControl::get_instance()->get_hora() < 15){
			MicroClima::get_instance()->temp_ar_sector = MicroClima::get_instance()->temp_ar_sector*(1-r*sin(((TimeControl::get_instance()->get_hora())-6)*PI/16));			
		}else{
			MicroClima::get_instance()->temp_ar_sector = MicroClima::get_instance()->temp_ar_sector*(b+a*cos(((TimeControl::get_instance()->get_hora())-15)*PI/14));
		}

		//Calcula temperatura do solo do microclima
		MicroClima::get_instance()->temp_solo_sector = (ALFA_SOLO*(MicroClima::get_instance()->temp_ar_sector)*(MicroClima::get_instance()->temp_ar_sector)) + BETA_SOLO;
		//Calcula temperatura das tocas do microclima
		MicroClima::get_instance()->temp_toca_sector = (ALFA_TOCA*(MicroClima::get_instance()->temp_ar_sector)) + BETA_TOCA;

		//Na sombra, a temperatura do solo não será maior que 40º
		if(MicroClima::get_instance()->temp_solo_sector>40)
			MicroClima::get_instance()->temp_solo_sector=40;

		if(ClimaTempo::get_instance()->get_umi_rel() != 100){
			MicroClima::get_instance()->umidade_relativa_sector = ALFA_UMIDADE*MicroClima::get_instance()->temp_ar_sector + BETA_UMIDADE;
			if(MicroClima::get_instance()->umidade_relativa_sector > 100){
				MicroClima::get_instance()->umidade_relativa_sector = 100;
			}
		}
	}

	if(DEBUG){
		if((MicroClima::get_instance()->arquivo = fopen("TempArMicro.txt", "a+")) != NULL){
			fprintf(MicroClima::get_instance()->arquivo, "%f\n", MicroClima::get_instance()->get_temp_ar_sector());
			fclose(MicroClima::get_instance()->arquivo);
		}
		if((MicroClima::get_instance()->arquivo = fopen("TempSoloMicro.txt", "a+")) != NULL){
			fprintf(MicroClima::get_instance()->arquivo, "%f\n", MicroClima::get_instance()->get_temp_solo_sector());
			fclose(MicroClima::get_instance()->arquivo);
		}
		/*if((arquivo = fopen("TempToca.txt", "a+")) != NULL){
			fprintf(arquivo, "%f\n", temTocaControl);
			fclose(arquivo);
		}*/
		cout << "\n Temperatura do ar da microregiao: " << MicroClima::get_instance()->temp_ar_sector << endl;
		cout << " Temperatura do solo da microregiao: " << MicroClima::get_instance()->temp_solo_sector << endl;
		cout << " Temperatura da toca da microregiao: " << MicroClima::get_instance()->temp_toca_sector << endl;
		cout << " Umidade relativa do ar: " << MicroClima::get_instance()->umidade_relativa_sector << endl;
	}
}

double MicroClima::get_temp_ar_sector(){
	return temp_ar_sector;
}

double MicroClima::get_temp_solo_sector(){
	return temp_solo_sector;
}

double MicroClima::get_temp_toca_sector(){
	return temp_toca_sector;
}

double MicroClima::get_umidade_relativa_sector(){
	return umidade_relativa_sector;
}

//Descarregamento da memória, para permitir reinicialização
void MicroClima::unload_microClima(){
	MicroClima::instanceFlag = false;
	MicroClima::single = NULL;
}

//Implementação do singleton
PT(MicroClima) MicroClima::get_instance(){
	if(!instanceFlag){
		if(DEBUG){
			cout << "\n single mclima\n " << endl;
		}
		single = new MicroClima();
        instanceFlag = true;
    }
    return single;
}
