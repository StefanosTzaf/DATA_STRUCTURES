//////////////////////////////////////////////////////////////////////////////
//
// Παράδειγμα δημιουργίας ενός παιχνιδιού χρησιμοποιώντας τη βιβλιοθήκη raylib
//
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "raylib.h"

#include "interface.h"
#include "state.h"
#include <stdlib.h>

State state;

void update_and_draw() {
	//δεν υπάρχει εξάρτηση της state_update με την raylib.h (ετσι εχω την ευκολία
	// να χρησημοποιήσω άλλη στο μέλλον χωρίς να αλλάξω την 
	//συνάρτηση state_update) η update and draw που καλεί την state_update περνάει σε αυτή τα keys

	struct key_state keys1;
	keys1.down = IsKeyDown(KEY_DOWN);
	keys1.up = IsKeyDown(KEY_UP);
	keys1.enter = IsKeyDown(KEY_ENTER);
	keys1.left = IsKeyDown(KEY_LEFT);
	keys1.right = IsKeyDown(KEY_RIGHT);
	keys1.n = IsKeyDown(KEY_N);
	keys1.p = IsKeyPressed(KEY_P);

	state_update(state,&keys1);
	interface_draw_frame(state);
}

int main() {
	state = state_create();
	interface_init();

	// Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
	start_main_loop(update_and_draw);

	interface_close();

	return 0;
}



