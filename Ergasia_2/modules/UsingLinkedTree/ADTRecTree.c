///////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT RecTree μέσω links
//
///////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "ADTRecTree.h"


// Προς υλοποίηση...


//  Ένα δέντρο είναι pointer σε αυτό το struct
struct rec_tree {
	Pointer value;
	RecTree subtree_left;
	RecTree subtree_right;
	int size;
};


// Δημιουργεί και επιστρέφει ένα νέο δέντρο, με τιμή (στη ρίζα) value και υποδέντρα left και right.

RecTree rectree_create(Pointer value, RecTree left, RecTree right) {
	RecTree tree = malloc(sizeof(struct rec_tree));
	tree->value = value;
	tree->subtree_left = left;
	tree->subtree_right = right;
	tree->size = 1;
	//εκμεταλευόμαστε οτι δεν μπορούν να αλλάξουν τα περιεχόμενα των δεντρων δηλαδή
	//δεν θα προσθεσουμε ενα δεντρο ως υποδεντρο αυτου που εχουμε δημιουργησει πιο πριν
	//αλλα θα δημιουργησουμε ενα νεο.Ετσι το size ειναι απλα το size των υποδενδρων +1.
	if(left!=REC_TREE_EMPTY){
		tree->size += left->size;
	}
	if(right!=REC_TREE_EMPTY){
		tree->size += right->size;
	}
	return tree;
}

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο.

int rectree_size(RecTree tree) {
	if(tree != REC_TREE_EMPTY){
	   return tree->size;
	}
	else{
		return 0;
	}
}

// Ελευθερώνει όλη τη μνήμη που δεσμεύει το δέντρο tree.

void rectree_destroy(RecTree tree) {
	free(tree);
}

// Επιστρέφουν την τιμή (στη ρίζα), το αριστερό και το δεξί υποδέντρο του δέντρου tree.

Pointer rectree_value(RecTree tree) {
	if(tree != REC_TREE_EMPTY){
		return tree->value;
	}
	else{
		return NULL;
	}
}

RecTree rectree_left(RecTree tree) {
	if(tree != REC_TREE_EMPTY){
		return tree->subtree_left;
	}
	else{
		return REC_TREE_EMPTY;
	}
}

RecTree rectree_right(RecTree tree) {
	if(tree != REC_TREE_EMPTY){
		return tree->subtree_right;
	}
	else{
		return REC_TREE_EMPTY;
	}
}

