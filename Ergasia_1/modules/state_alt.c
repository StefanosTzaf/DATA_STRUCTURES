
#include <stdlib.h>
#include "state.h"
#include "set_utils.h"

//αν ειναι μεγαλυτερη η συνεταγμενη x  του α αντικειμοενου, γυρναει θετικο αλλιως αρνητικό
int objs_compare(Pointer a,Pointer b){
	Object a1 = a;
    Object b1 = b;
	if(a1->rect.x> b1->rect.x){
        return 1;
    }
    else if(a1->rect.x < b1->rect.x){
        return -1;
    }
	return 0;
}

struct state {
	Set objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
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
		set_insert(state->objects, platform);

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
			set_insert(state->objects, star);
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
	state->objects = set_create(objs_compare, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = set_node_value(state->objects,set_first(state->objects));//η πρωτη πλατφορμα θα ειναι στην αρχη του σετ
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

StateInfo state_info(State state) {
	return &(state->info);
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {
	
	List objects_list=list_create(NULL);

	//φτιαχνουμε δυο αντικειμενα με χ το x_from και το x_to αντιστοιχα
	Object obj_temp1 = create_object(PLATFORM,x_from,0,50,20,FALLING,1.0,false);
	Object obj_temp2 = create_object(PLATFORM,x_to,0,50,20,FALLING,1.0,false);


	//θα παρουμε το πρωτο αντικειμενο του σετ που ειναι αναμεσα στο x_from και το x_to
	Object obj_first = set_find_eq_or_greater(state->objects , obj_temp1);
	//και τον αντιστοιχο κομβο
	SetNode first_node = set_find_node(state->objects,obj_first);
	
	
	//το τελευταιο αντικειμενο του σετ που ειναι αναμεσα στο x_from και το x_to
	Object obj_last = set_find_eq_or_smaller(state->objects,obj_temp2);
	//και τον κομβο του
	SetNode last_node = set_find_node(state->objects,obj_last);

	//αν το obj_first ειναι το μεγαλυτερο στοιχειο του set 'H το obj_last ειναι το μικροτερο τοτε δεν γινεται
	//να υπαρχουν αναμεσα τους στοιχεια του set

	if((obj_first == NULL) || (obj_last == NULL)){
		return objects_list;
	}
	else if(first_node == last_node){
		list_insert_next(objects_list,LIST_BOF,obj_first);
		return objects_list;
	}
	
	

	SetNode current_node = first_node;
	Object current_object = set_node_value(state->objects,current_node);
	//εκμεταλευόμενοι την διαταξη των στοιχείων του set βαζουμε στοιχεια στο σετ 
	//μεχρι το last
	while(current_node != last_node){
		list_insert_next(objects_list,LIST_BOF,current_object);

		current_node = set_next(state->objects,current_node);
		current_object = set_node_value(state->objects,current_node);
	}
	list_insert_next(objects_list,LIST_BOF,obj_last);

	return objects_list;
}







void state_update(State state, KeyState keys) {
	Object ball=state->info.ball;
	if( ((state->info.playing == true) && (state->info.paused == false)) || ((state->info.playing==true) &&(keys->n) &&(state->info.paused == true))){
		//ΟΡΙΖΟΝΤΙΑ ΚΙΝΗΣΗ ΜΠΑΛΑΣ	
		if(keys->right){
			ball->rect.x +=6;
		}
		else if(keys->left){
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

		

		Set current_objects = state->objects;
		//αντικειμενο με x 2 οθονες πισω αφου θελουμε να ελεγχονται τα αντικειμενα με διαφορα δυο οθονων
		Object two_screens_back = create_object(PLATFORM,ball->rect.x - 2*SCREEN_WIDTH,200,250,80,MOVING_UP,1.1,true);
		//παιρνουμε το αμέσως μεγαλύτερο αντικείμενο του set απο το παραπάνω
		Object first_obj = set_find_eq_or_greater(current_objects,two_screens_back);
		SetNode node_first = set_find_node(current_objects,first_obj);

		Object last_platform = NULL;
		float max_x = 0;
		bool ball_onto_a_platform = false;

		//LOOP ΓΙΑ ΛΕΙΤΟΥΡΓΙΕΣ ΑΣΚΗΣΗ 3 + ΚΑΤΑΚΟΡΥΦΗ ΚΙΝΗΣΗ ΠΛΑΤΦΟΡΜΑΣ ΑΠΟ ΑΣΚΗΣΗ 2
		// για τα αντικειμενα 2 οθονων πριν εως δυο οθόνες μετα την μπάλα
		for(SetNode node = node_first;
		 	((Object)set_node_value(current_objects , node))->rect.x <=ball->rect.x + SCREEN_WIDTH*2;
			node = set_next(current_objects , node))
			
			{
			Object obj_temp = set_node_value(current_objects,node);
			//Συμπεριφορά μπάλας σε κατακόρυφη ηρεμία 
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
					//επειδη αφαιρουμε τον κομβο αυτον πρεπει πρωτα να εχουμε προσβαση στον προηγουμενο 
					//για να συνεχριστει η λουπα
					SetNode node_previous = set_previous(current_objects , node);
					set_remove(current_objects , set_node_value(current_objects , node));
					node = node_previous;
				}
				
				if(max_x < obj_temp->rect.x){
					max_x = obj_temp->rect.x;
					last_platform = obj_temp;
				}
			}
			if(CheckCollisionRecs(ball->rect,obj_temp->rect)){
				if(obj_temp->type == PLATFORM){
					// η αφαιρεση 6 απο το υψος εξηγειται στο readme 4)
					if ((ball->vert_mov == FALLING) && (ball->rect.y + ball->rect.height/2 <= obj_temp->rect.y)){		
					
						ball->rect.y = obj_temp->rect.y - (ball->rect.height - 6) ;
						ball->vert_mov = IDLE;
					}
					if( (ball->vert_mov) == IDLE){
						ball->rect.y = obj_temp->rect.y - (ball->rect.height - 6); 
						ball_onto_a_platform = true;
					}

					//συγκρουση μπαλας με unstable πλατφορμα 
					if (obj_temp->unstable){
						obj_temp->vert_mov = FALLING;
					}
				}
				else if (obj_temp->type == STAR){ 
					SetNode node_previous = set_previous(current_objects , node);
					set_remove(current_objects , set_node_value(current_objects , node));
					node = node_previous;
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

		if(last_platform->rect.x - ball->rect.x <= SCREEN_WIDTH ){
		 	add_objects(state,last_platform->rect.x + last_platform->rect.width);
		 	state->speed_factor *= 1.1;
			
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
	set_destroy(state->objects);
	free(state);
}
