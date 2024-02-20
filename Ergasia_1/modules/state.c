
#include <stdlib.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"
#include <stdio.h>

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

//δέχεται ενα vector και μία θέση και ανταλλασσει τα περιεχόμενα της θεσης αυτης με της τελευταιας θεσης του βεκτορ 
void vector_swap(Vector vec ,int position){
	Object temp = vector_get_at(vec,position);
	//βαζω στην θεση position το περιεχομενο της τελευταιας θεσης του vector
	vector_set_at(vec,position ,vector_node_value(vec, vector_last(vec) ) );
	//βαζω στο τέλος του vector το περιεχομενο της θέσης που δώθηκε ως ορισμα
	vector_set_at(vec,vector_size(vec) - 1,temp );
	
}

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height, VerticalMovement vert_mov, float speed, bool unstable) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	obj->unstable = unstable;
	return obj;
}

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από x = start_x, και επεκτείνονται προς τα δεξιά.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι η αρχή της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά.
//
// - Στον άξονα y το 0 είναι το πάνω μέρος της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα έχει σταθερό ύψος, οπότε όλες οι
//   συντεταγμένες y είναι ανάμεσα στο 0 και το SCREEN_HEIGHT.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_x) {
	// Προσθέτουμε PLATFORM_NUM πλατφόρμες, με τυχαία χαρακτηριστικά.

	for (int i = 0; i < PLATFORM_NUM; i++) {
		Object platform = create_object(
			PLATFORM,
			start_x + 150 + rand() % 80,						// x με τυχαία απόσταση από το προηγούμενο στο διάστημα [150, 230]
			SCREEN_HEIGHT/4 + rand() % SCREEN_HEIGHT/2,			// y τυχαία στο διάστημα [SCREEN_HEIGHT/4, 3*SCREEN_HEIGHT/4]
			i == 0 ? 250 : 50 + rand()%200,						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0.6 + 3*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 && (rand() % 5) == 0							// το 20% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		vector_insert_last(state->objects, platform);

		// Στο 50% των πλατφορμών (τυχαία), εκτός της πρώτης, προσθέτουμε αστέρι
		if(i != 0 && rand() % 2 == 0) {
			Object star = create_object(
				STAR,
				start_x + 200 + rand() % 60,				// x με τυχαία απόσταση από την προηγούμενη πλατφόρμα στο διάστημα [200,260]
				SCREEN_HEIGHT/8 + rand() % SCREEN_HEIGHT/2,	// y τυχαία στο διάστημα [SCREEN_HEIGHT/8, 5*SCREEN_HEIGHT/8]
				30, 30,										// πλάτος, ύψος
				IDLE,										// χωρίς κίνηση
				0,										 	// ταχύτητα 0
				false										// 'unstable' πάντα false για τα αστέρια
			);
			vector_insert_last(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = vector_create(0, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = vector_get_at(state->objects, 0);
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		45, 45,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	return &(state->info);
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {
	List objects_list=list_create(NULL);

	for(int i = 0; i < vector_size(state->objects); i++){
			Object obj1 = vector_get_at(state->objects,i);
			if (obj1->rect.x <= x_to  &&  obj1->rect.x >= x_from){
				list_insert_next(objects_list, LIST_BOF, obj1);
			}
	}
	return objects_list;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.


//Λόγω της συνθετότητας της συνάρτησης στα σχόλια με ΚΕΦΑΛΑΙΑ γράφεται καθε φορά η λειτουργία του τμήματος που ακολουθε΄ι και
//με μικρά η επεξήγηση
void state_update(State state, KeyState keys) {
	Object ball = state->info.ball;
	//οι λειτουργίες εκτελουνται αν το παιχνιδι παιζεται και δεν ειναι pause Η
	//αν ειναι σε pause και πατηθει το n 
	if( ((state->info.playing == true) && (state->info.paused == false)) || ((state->info.playing==true) &&(keys->n) &&(state->info.paused == true))){

		//ΟΡΙΖΟΝΤΙΑ ΚΙΝΗΣΗ ΜΠΑΛΑΣ	
		if(keys->right){
			ball->rect.x +=6;
		}
		else if(keys->left){
			//να πηγαίνει ελάχιστα πίσω
			ball->rect.x -=0.25;
		}
		else{
			ball->rect.x +=4;
		}


		//ΚΑΤΑΚΟΡΥΦΗ ΚΙΝΗΣΗ ΜΠΑΛΑΣ
		if( (ball->vert_mov) == JUMPING ){  
			//Μετακινείται προς τα πάνω τόσα pixels όσα η κατακόρυφη ταχύτητά της.   
			ball->rect.y -= ball->vert_speed ; 
			ball->vert_speed *= 0.85;
			if(ball->vert_speed <= 0.5){
				ball->vert_mov = FALLING;
			}
		}
		else if( (ball->vert_mov) == FALLING ){
			ball->rect.y += ball->vert_speed;
			ball->vert_speed *= 1.1;
			if( ball->vert_speed > 7 ){
				ball->vert_speed = 7;
			}
		}
		//αν η μπαλα βρίσκεται σε ηρεμία και είναι πατημένο το πάνω βέλος μπαίνει σε κατάσταση άλματος με αρχική ταχύτητα 17.
		else if( (ball->vert_mov) == IDLE ){
			if(keys->up){
				ball->vert_mov = JUMPING;
				ball->vert_speed = 17;
			}
		}


		//LOOP ΓΙΑ ΛΕΙΤΟΥΡΓΙΕΣ ΑΣΚΗΣΗ 3 + ΚΑΤΑΚΟΡΥΦΗ ΚΙΝΗΣΗ ΠΛΑΤΦΟΡΜΑΣ ΑΠΟ ΑΣΚΗΣΗ 2
		Vector current_objects = state->objects;
		Object last_platform = NULL;
		float max_x = 0;
		bool ball_onto_a_platform = false;

		for(int i=0; i < vector_size(current_objects);i++){
			//το αντικειμενο που εχουμε προσβαση σε καθε επαναληψη
			Object obj_temp = vector_get_at(current_objects,i);

			 
			if(obj_temp->type == PLATFORM){
				//(ΑΣΚΗΣΗ 2) ΚΑΤΑΚΟΡΥΦΗ ΚΙΝΗΣΗ ΠΛΑΤΦΟΡΜΑΣ
				if(obj_temp->vert_mov == MOVING_UP){
				//Μετακινείται προς τα πάνω τόσα pixels όσα η κατακόρυφη ταχύτητά της
					obj_temp->rect.y -= obj_temp->vert_speed;
						if(obj_temp->rect.y < SCREEN_HEIGHT/4){ 
						//αφου κινουμαστε προς τα πανω γινεται ολο και πιο αρνητικο για αυτο <
							obj_temp->vert_mov = MOVING_DOWN;
						}
				}
				else if(obj_temp->vert_mov == MOVING_DOWN){
					obj_temp->rect.y += obj_temp->vert_speed;
						if(obj_temp->rect.y > 3*SCREEN_HEIGHT/4){ 
							obj_temp->vert_mov = MOVING_UP;
						}		
				}
				else if (obj_temp->vert_mov == FALLING){
					obj_temp->rect.y += 4;

				}
				//σε περίπτωση σύγκρουσης με έδαφος
				if(obj_temp->rect.y >= SCREEN_HEIGHT){
					//βαζουμε το obj_temp στο τελος του vector
					vector_swap(current_objects,i);
					vector_remove_last(current_objects);
				}
				//βρισκουμε την πλατφορμα με το μεγαλύτερο x η οποία θα είναι η τελευταία πλατφόρμα
				if(max_x < obj_temp->rect.x){
					max_x = obj_temp->rect.x;
					last_platform = obj_temp;
				}
			}
			//ΣΥΓΚΡΟΥΣΕΙΣ ΜΠΑΛΑΣ - (ΑΣΤΕΡΙ Η ΠΛΑΤΦΟΡΜΑ)
			if(CheckCollisionRecs(ball->rect,obj_temp->rect)){
				if(obj_temp->type == PLATFORM){
					//η δευτερη συνθηκη στο if εχει να κανει με το αν η μπαλα ερχεται απο πάνω
					//αν το y της μπαλας + το μισο υψος της , ειναι μικροτερο απο το y της πλατφορμας
					//δηλαδη η μεση της τουλαχιστον ειναι πιο ψηλα απτη πλατφορμα
					//τοτε να μπαινει σε idle
					if ((ball->vert_mov == FALLING) && (ball->rect.y + ball->rect.height/2  <= obj_temp->rect.y)){
						// η αφαιρεση 6 απο το υψος εξηγειται στο readme 4)
						ball->rect.y = obj_temp->rect.y - (ball->rect.height - 6) ;
						ball->vert_mov = IDLE;
					}
					//Συμπεριφορά μπάλας σε κατακόρυφη ηρεμία αν ειναι πανω σε πλατφορμα
					if( (ball->vert_mov) == IDLE){
						ball->rect.y = obj_temp->rect.y - (ball->rect.height - 6); 
						//αν ειναι πανω σε πλατφορμα ενημεωνουμε την μεταβλητη
						//θα χρειαστει εξω απο το loop
						ball_onto_a_platform = true;
					}

					//συγκρουση μπαλας με πλατφορμα unstable
					if (obj_temp->unstable){
						obj_temp->vert_mov = FALLING;
					}
				}

				else if (obj_temp->type == STAR){ 
					vector_swap(current_objects,i);
					vector_remove_last(current_objects);
					state->info.score +=10;
				}	
			}
		}

		//Aν δεν ειναι πανω σε καμια πλατφορμα
		if((ball_onto_a_platform == false) && (ball->vert_mov == IDLE)){
			ball->vert_mov = FALLING ;
			ball->vert_speed = 1.5 ;
		}
		

		//ΣΥΓΚΡΟΥΣΗ ΜΠΑΛΑΣ ΜΕ ΚΑΤΩ ΜΕΡΟΣ ΟΘΟΝΗΣ
		//αφου οι συντεταγμένες της μπαλας είναι η πάνω αριστερά γωνία ,αφαιρούμε το υψος της μπάλας ετσι ωστε ο παίχτης να χάνει όταν το κάτω μέρος της μπάλας ακουμπάει το έδαφος και όχι όταν χάνεται η μπάλα
		if(ball->rect.y >= SCREEN_HEIGHT - ball->rect.height){
			state->info.playing = false ; 
		}

		//Όταν το η μπάλα φτάσει σε απόσταση μίας οθόνης από την τελευταία πλατφόρμα
		//προσθετουμε αντικειμενα και το speed factor γινεται 10% μεγαλυτερο
		if(last_platform->rect.x - ball->rect.x <= SCREEN_WIDTH ){
		 	add_objects(state,last_platform->rect.x + last_platform->rect.width);
		 	state->speed_factor *= 1.1;
			ball->vert_speed *=state->speed_factor;
			//η αλλαγη στο speed factor πρεπει να επηρεαζει ολα τα αντικειμενα 
			for(int i=0; i < vector_size(current_objects);i++){
				Object obj_temp2 = vector_get_at(current_objects,i);
				obj_temp2->vert_speed*=state->speed_factor;
			}
		}

	//τελος διαδικασιων που συμβαινουν μονο αν το παιχνιδι παιζεται η αν πατιεται n	
	}
	
	//ΕΚΚΙΝΗΣΗ ΚΑΙ ΔΙΑΚΟΠΗ
	//Αν το παιχνίδι έχει τελειώσει και πατηθεί enter, τότε ξαναρχίζει από την αρχή.
	if((!state->info.playing) && (keys->enter)){
		state_destroy(state);
		state = state_create();
	}
	//pause
	if(keys->p){
		if(!state->info.paused){
			state->info.paused = true;
		}
		//αν ειναι pause και πατηθει p να αρχισει να παιζει
		else {
			state->info.paused = false;
		}
	}

}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	vector_destroy(state->objects);
	free(state);
}
