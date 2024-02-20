#include <stdlib.h>
#include "ADTRecTree.h"
#include "ADTMap.h"


//map στο οποίο αντιστοιχίζονται τα δέντρα με τις τιμες που περιεχουν
Map values;
//map στο οποίο τα δέντρα αντιστοιχίζονται με το αριστερό τους παιδί κάθε φορα
Map left_children;
//map στο οποίο τα δέντρα αντιστοιχίζονται με το δεξί τους παιδί κάθε φορα
Map right_children;
//map για τα sizes
Map sizes; 

//μεταβλητή global για να μετραμε πόσα δέντρα έχουν δημιουργηθεί και καθε
//φορά να αυξάνουμε τον μετρητη.
int current_tree = 0;

//το struct δεν έχει κάποια χρησημότητα
struct rec_tree {

};

//συγκρινουμε pointers που ομως τους εχουμε περασει ακεραιες τιμες για αυτο δεν κανουμε 
//derefference
int compare_pointers(Pointer a,Pointer b){
	return  a -b;
}


// Δημιουργεί και επιστρέφει ένα νέο δέντρο, με τιμή (στη ρίζα) value και υποδέντρα left και right.

RecTree rectree_create(Pointer value, RecTree left, RecTree right) {
	//δημιουργουμε τα maps με την πρώτη κληση της rectree_create

	if(current_tree == 0){
		//η compare καθε φορα συγκρίνει τα κλειδιά που είναι δηλωμένοι ως pointers 
		//(αριθμός δεντρου)
		left_children = map_create(compare_pointers,NULL,NULL);
		right_children = map_create(compare_pointers,NULL,NULL);
		values = map_create(compare_pointers,NULL,NULL);
		sizes = map_create(compare_pointers,NULL,NULL);
	}
	current_tree++;
	//αν τα υποδεντρα είναι κενά χρειάζεται να το μηδενίσουμε για τα insert
	//με 0 χαρακτηριζουμε το κενο δεντρο
	if(left == REC_TREE_EMPTY){
		left = 0;
	}
	if(right == REC_TREE_EMPTY){
		right = 0;
	}

	map_insert(values,(RecTree)(size_t)current_tree,value);
	map_insert(left_children,(RecTree)(size_t)current_tree,left);
	map_insert(right_children,(RecTree)(size_t)current_tree,right);

	//το μέγεθος του δέντρου είναι τουλάχιστον 1
	int size = 1 ;
	Pointer left_ch,right_ch;
	//αν τα υποδέντρα δεν είναι κενά προσθέτουμε το μέγεθος τους στο δεντρο
	//το long για να ταιριάζει το μεγεθος
	if((left_ch = map_find(left_children,(RecTree)(size_t)current_tree)) != 0){
		size+= (long)(map_find(sizes,left_ch));
	}
	if((right_ch = map_find(right_children,(RecTree)(size_t)current_tree)) != 0){
		size+= (long)(map_find(sizes,right_ch));
	}
	
	map_insert(sizes,(RecTree)(size_t)current_tree,(RecTree)(size_t)size);
	//στον pointer στο δεντρο αυτο θα επιστρέψουμε τον αριθμό του δεντρου
	//αφου ξερουμε οτι δεν θα γινει ποτέ dereference.
	return (RecTree)(size_t)current_tree;

}

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο.

int rectree_size(RecTree tree) {
	//αν το δεντρο είναι το κενό
	if(tree == 0){
		return 0;
	}
	return (long)map_find(sizes,tree);
}

// Ελευθερώνει όλη τη μνήμη που δεσμεύει το δέντρο tree.

void rectree_destroy(RecTree tree) {
	map_remove(left_children,tree);
	map_remove(right_children,tree);
	map_remove(sizes,tree);
	map_remove(values,tree);
	//αν ήταν το τελευταίο δεντρο καταστρέφουμε τα maps
	if(map_size(values) == 0){
		map_destroy(right_children);
		map_destroy(left_children);
		map_destroy(values);
		map_destroy(sizes);
		//μηδενίζουμε τον μετρητή για επόμενη κλήση της create
		current_tree = 0;
	}
}

// Επιστρέφουν την τιμή (στη ρίζα), το αριστερό και το δεξί υποδέντρο του δέντρου tree.

Pointer rectree_value(RecTree tree) {
	//αν δεν είναι το κενό δεντρο
	if(tree != 0){
		return map_find(values,tree);
	}
	else {
		return NULL;
	}
}

RecTree rectree_left(RecTree tree) {
	if(tree != 0){
		return map_find(left_children,tree);
	}
	else {
		return 0;
	}
}

RecTree rectree_right(RecTree tree) {
	if(tree != 0){
		return map_find(right_children,tree);
	}
	else {
		return 0;
	}
}

