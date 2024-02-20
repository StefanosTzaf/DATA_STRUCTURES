//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "state.h"


void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);
	TEST_ASSERT(info->score == 0);
	 
	TEST_ASSERT(info->ball->rect.width == 45);
	
	// στην αρχη η μπαλα πρεπει να βρισκεται αναμεσα σε αυτες τις συντεταγμενες
	TEST_ASSERT(info->ball->rect.x <= 230);
	TEST_ASSERT(info->ball->rect.x >=150);
	
	TEST_ASSERT(info->ball->rect.y >= SCREEN_HEIGHT/4 -40);
	TEST_ASSERT(info->ball->rect.y <= 3*(SCREEN_HEIGHT/4) -40);


}

void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, η μπάλα μετακινείται 4 pixels δεξιά
	Rectangle old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->ball->rect;
	TEST_ASSERT( new_rect.x == old_rect.x + 4 );

	// Με πατημένο το δεξί βέλος, η μπάλα μετακινείται 6 pixels δεξιά
	keys.right = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x + 6 );



	keys.right = false;
	
	//βαζουμε μια αδύνατη τιμή για να μην συγκρουστεί με καμία πλατφόρμα και να ελενξουμε μονο
	//αυτο που θελουμε
	state_info(state)->ball->rect.y =-50;
	state_info(state)->ball->vert_mov = JUMPING;
	state_info(state)->ball->vert_speed = 0.55;
	state_update(state,&keys);
	//αν η μπάλα είναι σε κατάσταη jumping και η ταχυτητα της (οταν γινει το 85% της τιμής της) 
	//ειναι κατω απο 0.5 τοτε να μπει σε κατάσταση falling
	TEST_ASSERT(state_info(state)->ball->vert_mov == FALLING);


	//Αν βρίσκεται σε κατάσταση IDLE και είναι πατημένο το πάνω βέλος μπαίνει σε κατάσταση JUMPING με αρχική ταχύτητα 17.
	state_info(state)->ball->vert_mov = IDLE;
	keys.up = true;
	state_update(state, &keys);
	state_info(state)->ball->rect.x = -10; //ωστε να μην αλλαξει λογω συγκρουσης με πλατφορμα η κατασταση
	TEST_ASSERT(state_info(state)->ball->vert_speed == 17);
	TEST_ASSERT(state_info(state)->ball->vert_mov == JUMPING);


	//Αν η μπάλα φτάσει στο κάτω μέρος της οθόνης τερματίζει το παιχνίδι.
	 state_info(state)->playing = true ;
	 state_info(state)->ball->rect.y = SCREEN_HEIGHT;
	 state_update(state,&keys);
	 TEST_ASSERT(state_info(state)->playing == false);

}

void test_state_objects(){
	State state = state_create();

	//με μια κληση της state create να προηγείται δημιουργουνται σιγουρα 20 πλατφορμες και ισως ενα αστερι σε καθε μια (εκτος της
	//πρωτης) αρα 20+19 στην καλυτερη περιπτωση ,η συναρτηση state objects θα πρεπει να επιστρεφει αν της δωσουμε
	// ολο το width(20*900) το πολυ 39
	List objs =state_objects(state,0,20*900);
	TEST_ASSERT(objs != NULL);
	TEST_ASSERT(list_size(objs)<=39);
	
	for(ListNode node=list_first(objs);
		node !=LIST_EOF;
	 	node=list_next(objs,node) ){
		//για να τσεκαρουμε αν τα αντικειμενα που επέστρεψε στην λίστα είναι οντως αναμεσα σε τιμες του χ[0,20*900]
		Object obj = list_node_value(objs,node);
		TEST_ASSERT(obj->rect.x <= 20*900);
		TEST_ASSERT(obj->rect.x >=0);
	}
	
	//το πολυ 3 πλατφορμες και 2 αστερια στην πρωτη οθονη ΟΠΟΥ SCREEN WIDTH=900
	List objs1 =state_objects(state,0,900);
	TEST_ASSERT(objs != NULL);
	TEST_ASSERT(list_size(objs1)<=5);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },
	{"test_state_objects", test_state_objects},

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};