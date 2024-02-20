#include "raylib.h"

#include "state.h"
#include "interface.h"

// Assets
Texture bird_img;
Texture ball_img;
Texture star_img;
Sound game_over_snd;


void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game");
	SetTargetFPS(60);
    InitAudioDevice();

	// Φόρτωση εικόνων και ήχων
	bird_img = LoadTextureFromImage(LoadImage("assets/bird.png"));
	ball_img = LoadTextureFromImage(LoadImage("assets/telikhmpala2.png"));
	star_img = LoadTextureFromImage(LoadImage("assets/star2.png"));
	game_over_snd = LoadSound("assets/game_over.mp3");
}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}

// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(RAYWHITE);

	// Σχεδιάζουμε την μπαλα 
	StateInfo info = state_info(state);
	
	float ball_x = info->ball->rect.x;
	
	float x_offset = (SCREEN_WIDTH/2)-200  - ball_x;
	//To offset ειναι μια σταθερα που αν την προσθ΄εσω σε μια μεταβλητή χάρτη γίνεται μεταβλητή οθόνης
	DrawTexture(bird_img, ball_x+x_offset, info->ball->rect.y, WHITE);
	
	//δεν μας νοιαζει ακριβως χοντρικα σχεδιαζουμε οτι υπαρχει απο κει που ειναι η μπαλα και μια οθονη μετα και μια πριν	
	List objs = state_objects(state, ball_x - SCREEN_WIDTH,ball_x + SCREEN_WIDTH);

	for(ListNode node = list_first(objs);
		node != LIST_EOF;
		node = list_next(objs , node)){
			Object obj = list_node_value(objs , node);
			if(obj->type == PLATFORM){
				//το y οθονης ειναι ιδιο με y χαρτη παιχνιδιου
				//το x το βρισκουμε με το offset

				if(!obj->unstable){
					//σχεδιαση σταθερών πλατφορών
					DrawRectangle( obj->rect.x + x_offset  , obj->rect.y ,obj->rect.width,20 , RED);
				}
				else{
					//σχεδίαση ασταθών
					DrawRectangle( obj->rect.x + x_offset  , obj->rect.y ,obj->rect.width,20 , GRAY);
				}
			}
			else if(obj->type == STAR){
				DrawRectangle( obj->rect.x + x_offset  , obj->rect.y ,obj->rect.width,20 , YELLOW);
				//DrawTexture(star_img, obj->rect.x +x_offset, obj->rect.y, WHITE);
			}
		}
	
	 // Σχεδιάζουμε το σκορ και το FPS counter
	 DrawText(TextFormat("%04i", state_info(state)->score), 20, 20, 40, GRAY);
	 DrawFPS(SCREEN_WIDTH - 80, 0);

	 // Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	 if (!(state_info(state)->playing) && (!state_info(state)->paused)) {
	 	DrawText(
	 		"PRESS [ENTER] TO PLAY AGAIN",
	 		 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
	 		 GetScreenHeight() / 2 - 50, 20, GRAY
	 	);
	 }
	 else if ( (state_info(state)->paused)) {
	 	DrawText(
	 		"PRESS [P] TO CONTINUE",
	 		 GetScreenWidth() / 2 - MeasureText("PRESS [P] TO CONTINUE", 20) / 2,
	 		 GetScreenHeight() / 2 - 50, 20, GRAY
	 	);
	 }


 	EndDrawing();
 }