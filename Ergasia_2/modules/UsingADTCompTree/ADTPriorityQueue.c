#include "ADTCompTree.h"
#include <stdlib.h>
#include "ADTPriorityQueue.h"
#include <assert.h>

struct priority_queue {
	CompTree tree;				// Τα δεδομένα, σε complete δεντρο
	CompareFunc compare;		// Η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
};

// Επιστρέφει την τιμή του κόμβου node_id

static Pointer node_value(PriorityQueue pqueue, int node_id) {
	// τα node_ids είναι 0-based, το node_id αποθηκεύεται στη θέση node_id 
	return comptree_value(comptree_get_subtree(pqueue->tree, node_id ));
}

// Ανταλλάσει τις τιμές των κόμβων node_id1 και node_id2

static void node_swap(PriorityQueue pqueue, int node_id1, int node_id2) {
	// τα node_ids είναι 0-based, το node_id αποθηκεύεται στη θέση node_id 
	if(node_id1 != node_id2){
		CompTree tree0 = comptree_get_subtree(pqueue->tree,node_id1);
		CompTree tree1 = comptree_get_subtree(pqueue->tree,node_id2);

		//φτιάχνουμε δυο νεα δεντρα με ιδια` value με το παλιο τους ,αλλα με τα παιδια του αλλου δεντρου
		CompTree newtree0 = comptree_create(comptree_value(tree0),comptree_left(tree1),comptree_right(tree1));
		CompTree newtree1 = comptree_create(comptree_value(tree1),comptree_left(tree0),comptree_right(tree0));


		pqueue->tree = comptree_replace_subtree(pqueue->tree,node_id1,newtree1);
		pqueue->tree = comptree_replace_subtree(pqueue->tree,node_id2,newtree0);
	}

}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από
//       τον node_id που μπορεί να είναι _μεγαλύτερος_ από τον πατέρα του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_up(PriorityQueue pqueue, int node_id) {
	// Αν φτάσαμε στη ρίζα, σταματάμε
	if (node_id == 0)
		return;

	int parent = (node_id-1) / 2;		// Ο πατέρας του κόμβου. Τα node_ids είναι 0-based

	// Αν ο πατέρας έχει μικρότερη τιμή από τον κόμβο, swap και συνεχίζουμε αναδρομικά προς τα πάνω
	if (pqueue->compare(node_value(pqueue, parent), node_value(pqueue, node_id)) < 0) {
		node_swap(pqueue, parent, node_id);
		bubble_up(pqueue, parent);
	}
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από τον
//       node_id που μπορεί να είναι _μικρότερος_ από κάποιο από τα παιδιά του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.


static void bubble_down(PriorityQueue pqueue, int node_id) {
	// βρίσκουμε τα παιδιά του κόμβου (αν δεν υπάρχουν σταματάμε)
	int left_child = 2 * node_id + 1;
	int right_child = left_child + 1;

	int size = pqueue_size(pqueue);
	if (left_child >= size)
		return;

	// βρίσκουμε το μέγιστο από τα 2 παιδιά
	int max_child = left_child;
	//μικρότερο και όχι ίσο λόγω της 0-based αριθμησης
	if (right_child < size && pqueue->compare(node_value(pqueue, left_child), node_value(pqueue, right_child)) < 0)
			max_child = right_child;

	// Αν ο κόμβος είναι μικρότερος από το μέγιστο παιδί, swap και συνεχίζουμε προς τα κάτω
	if (pqueue->compare(node_value(pqueue, node_id), node_value(pqueue, max_child)) < 0) {
		node_swap(pqueue, node_id, max_child);
		bubble_down(pqueue, max_child);
	}
}


// Αρχικοποιεί το σωρό από τα στοιχεία του vector values.

static void heapify(PriorityQueue pqueue, Vector values) {
	int size = vector_size(values);

	for (int i = 0; i < size; i++){
		//βαζουμε ολα τα στοιχεια του vector στο vector του pqueue
		pqueue->tree = comptree_insert_last(pqueue->tree,vector_get_at(values,i));
	}
	//ο τελευταιος κομβος
	int lastnode_id = size - 1;
	//κανουμε bubble down σε ολους εκτος απο αυτους του τελευταιου επιπεδου
	for(int i = (lastnode_id-1)/2 ;i >=0; i--){
		bubble_down(pqueue,i);
	}
}


PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	assert(compare != NULL);	// LCOV_EXCL_LINE

	PriorityQueue pqueue = malloc(sizeof(*pqueue));
	pqueue->compare = compare;
	pqueue->destroy_value = destroy_value;
	pqueue->tree = COMP_TREE_EMPTY;
    
		if (values != NULL){
			heapify(pqueue, values);
		}

	return pqueue;
}



int pqueue_size(PriorityQueue pqueue){
    return comptree_size(pqueue->tree);
}

// //το μεγαλυτερο θα είναι η ρίζα
Pointer pqueue_max(PriorityQueue pqueue){
	return node_value(pqueue,0);
}

void pqueue_insert(PriorityQueue pqueue, Pointer value){
	//βαζουμε την τιμη στο τελος του δεντου για να παραμείνει complete
	//ΕΦΟΣΟΝ ΔΗΜΙΟΥΡΓΕΙ ΚΑΙΝΟΥΡΙΟ ΔΕΝΤΡΟ ΑΥΤΟ ΠΡΠΕΠΕΙ ΝΑ ΒΑΛΟΥΜΕ ΣΤΟΝ ΣΩΡΟ
	pqueue->tree = comptree_insert_last(pqueue->tree,value);
	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τον τελευταίο, που μπορεί να είναι
	// μεγαλύτερος από τον πατέρα του. Αρα μπορούμε να επαναφέρουμε την ιδιότητα του σωρού καλώντας
	// τη bubble_up γα τον τελευταίο κόμβο (του οποίου το 0-based id ισούται με το νέο μέγεθος του σωρού -1).
	bubble_up(pqueue,comptree_size(pqueue->tree) - 1);


}


void pqueue_remove_max(PriorityQueue pqueue){
	//λογω της 0 based 
	int last_node = pqueue_size(pqueue) - 1;
	assert(last_node != - 1);

	node_swap(pqueue, 0, last_node);

	if (pqueue->destroy_value != NULL)
		pqueue->destroy_value(comptree_value( comptree_get_subtree((pqueue->tree),last_node)));

	
		// Αντικαθιστούμε τον πρώτο κόμβο με τον τελευταίο και αφαιρούμε τον τελευταίο
	
	pqueue->tree = comptree_remove_last(pqueue->tree);
	
	
 	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τη νέα ρίζα
 	// που μπορεί να είναι μικρότερη από κάποιο παιδί της. Αρα μπορούμε να
 	// επαναφέρουμε την ιδιότητα του σωρού καλώντας τη bubble_down για τη ρίζα.
	bubble_down(pqueue,0);
}


DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value){
	DestroyFunc old = pqueue->destroy_value;
	pqueue->destroy_value = destroy_value;
	return old;
}


void pqueue_destroy(PriorityQueue pqueue){
	int pq_size = pqueue_size(pqueue);
	//πρεπει να απελευθερώσουμε την μνημη που χρησημοποιει καθε κομβος βαση της συναρτησης destroy_value

	for(int i = pq_size - 1;i >= 0;i--){
		if (pqueue->destroy_value != NULL){
			pqueue->destroy_value(comptree_value( comptree_get_subtree((pqueue->tree),i)));
		}
		//αφαιρουμε και τον κομβο
		pqueue->tree = comptree_remove_last(pqueue->tree);
	}
	
	free(pqueue);
}
