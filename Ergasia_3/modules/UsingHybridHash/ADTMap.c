/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω υβριδικού Hash Table
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMap.h"
#include "ADTVector.h"

// Οι κόμβοι του map στην υλοποίηση με hash table, μπορούν να είναι σε 2 διαφορετικές καταστάσεις,
// o κομβος είναι είτε EMPTY ειτε OCCUPIED αφού κάθε φορά που κάνουμε αναζήτηση θα κάνουμε στους neighboors
typedef enum {
	EMPTY, OCCUPIED
} State;



// Το μέγεθος του Hash Table ιδανικά θέλουμε να είναι πρώτος αριθμός σύμφωνα με την θεωρία.
// Η παρακάτω λίστα περιέχει πρώτους οι οποίοι έχουν αποδεδιγμένα καλή συμπεριφορά ως μεγέθη.
// Κάθε re-hash θα γίνεται βάσει αυτής της λίστας. Αν χρειάζονται παραπάνω απο 1610612741 στοχεία, τότε σε καθε rehash διπλασιάζουμε το μέγεθος.
int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};
// Κάθε θέση i θεωρείται γεινοτική με όλες τις θέσεις μέχρι και την i + NEIGHBOURS
#define NEIGHBOURS 3
#define MAX_LOAD_FACTOR 0.5

// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;		// Το κλειδί που χρησιμοποιείται για να hash-αρουμε
	Pointer value;  	// Η τιμή που αντισοιχίζεται στο παραπάνω κλειδί
	bool inchain;		// flag αν το στοιχείο βρίσκεται πάνω σε chain
	State state;		// Μεταβλητή για να μαρκάρουμε την κατάσταση των κόμβων (βλέπε διαγραφή)kf
};

// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	MapNode array;				// Ο πίνακας που θα χρησιμοποιήσουμε για το map (remember, φτιάχνουμε ένα hash table)
	Vector chains;				// Vector για separate chaining
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει.
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	CompareFunc compare;		// Συνάρτηση για σύγκριση δεικτών, που πρέπει να δίνεται απο τον χρήστη
	HashFunc hash_function;		// Συνάρτηση για να παίρνουμε το hash code του κάθε αντικειμένου.
	DestroyFunc destroy_key;	// Συναρτήσεις που καλούνται όταν διαγράφουμε έναν κόμβο απο το map.
	DestroyFunc destroy_value;
};

Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	// Δεσμεύουμε κατάλληλα τον χώρο που χρειαζόμαστε για το hash table
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(struct map_node));
	map->chains = vector_create(map->capacity, NULL);		
// Αρχικοποιούμε τους κόμβους που έχουμε σαν διαθέσιμους και δημιουργούμε τα vector

	// Αρχικοποιούμε τους κόμβους που έχουμε σαν διαθέσιμους και τα vectors σε κενα
	for (int i = 0; i < map->capacity; i++){
		map->array[i].state = EMPTY;
		vector_set_at(map->chains,i,NULL);	
	}

	map->size = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	return map;
}



// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}


static void rehash(Map map) {
	// Αποθήκευση των παλιών δεδομένων
	int old_capacity = map->capacity;
	MapNode old_array = map->array;
	Vector old_chains = map->chains;

	// Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
	int prime_no = sizeof(prime_sizes) / sizeof(int);	// το μέγεθος του πίνακα
	for (int i = 0; i < prime_no; i++) {					// LCOV_EXCL_LINE
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}
	// Αν έχουμε εξαντλήσει όλους τους πρώτους, διπλασιάζουμε
	if (map->capacity == old_capacity)					// LCOV_EXCL_LINE
		map->capacity *= 2;								// LCOV_EXCL_LINE

	// Δημιουργούμε ένα μεγαλύτερο hash table
	map->array = malloc(map->capacity * sizeof(struct map_node));
	map->chains = vector_create(map->capacity,NULL);
	for (int i = 0; i < map->capacity; i++){
		map->array[i].state = EMPTY;
		vector_set_at(map->chains,i,NULL);
	}

	
	map->size = 0;
	for (int i = 0; i < old_capacity; i++){
		if (old_array[i].state == OCCUPIED)
			map_insert(map, old_array[i].key, old_array[i].value);

		if (vector_get_at( old_chains,i) != NULL){ // Ελέγχουμε αν έχει αλυσίδα το i'οστό στοιχείο του vector
			Vector vec = vector_get_at(old_chains, i);
			for(VectorNode lnode = vector_first(vec);		// Βρίσκουμε όλα τα "occupied" στοιχεία τα εισάγουμε στο νέο vector και μετά σβήνουμε τα παλιά
				lnode != VECTOR_EOF;
				lnode = vector_next(vec, lnode)) {

				MapNode node = vector_node_value(vec, lnode);
				if (node->state == OCCUPIED)
					map_insert(map, node->key, node->value);
				free(node);
			}
			vector_destroy(vec);
		}
	}

	//Αποδεσμεύουμε τον παλιό πίνακα ώστε να μήν έχουμε leaks
	free(old_array);
	vector_destroy(old_chains);
}




// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.

void map_insert(Map map, Pointer key, Pointer value) {
	// Σκανάρουμε το Hash Table μέχρι να βρούμε διαθέσιμη θέση για να τοποθετήσουμε το ζευγάρι,
	// ή μέχρι να βρούμε το κλειδί ώστε να το αντικαταστήσουμε

	// Αρχικά ελέγχουμε αν το key του στοιχείου που προσπαθούμε να εισάγουμε υπάρχει ήδη.
	MapNode node = map_find_node(map, key);
    if (node != MAP_EOF) {
		// Διαγραφή των στοιχείων που είναι διαφορετικά από αυτά που θα εισάγουμε
        if (node->key != key) {
			if (map->destroy_key != NULL)
				map->destroy_key(node->key);
			node->key = key;
		}
		if (node->value != value) {
			if (map->destroy_value != NULL)
				map->destroy_value(node->value);
			node->value = value;
		}
        return;
    }

	int distance = 0;	
	uint pos;
	uint key_hash = map->hash_function(key) % map->capacity;


// Εύρεση πρώτης ελεύθερης θέσης
	for (pos = key_hash;									// ξεκινώντας από τη θέση που κάνει hash το key
		map->array[pos].state != EMPTY && (distance <= NEIGHBOURS + 1);						// αν φτάσουμε σε EMPTY σταματάμε
		pos = (pos + 1) % map->capacity) {					// linear probing, γυρνώντας στην αρχή όταν φτάσουμε στη τέλος του πίνακα
		
		distance ++;
		
	}


	//μπαινει στον πινακα και οχι στο chain
	if (distance <= NEIGHBOURS) {		//δεν κανουμε malloc γιατι εχει οριστει σαν μεταβλητη πιο πανω ΚΑΙ ΤΟ MALLOC ΕΧΕΙ ΓΙΝΕΙ ΣΤΟ CREATE									// Μπορούμε να το τοποθετήσουμε εκεί χωρίς παραπάνω διαδικασίες
		node = &map->array[pos];
		node->state = OCCUPIED;
		node->key = key;
		node->value = value;
		node->inchain = false;
	}
	else{
		MapNode inserted = malloc(sizeof(*inserted) );
		inserted->inchain = true ;
		inserted->key = key;
		inserted->state = OCCUPIED;
		inserted->value = value;

		//αν δεν υπαρχει vector το δημιουργουμε
		if (vector_get_at(map->chains,  map->hash_function(key) % map->capacity) == NULL)
			vector_set_at(map->chains, map->hash_function(key) % map->capacity, vector_create(0, NULL));

		vector_insert_last(vector_get_at(map->chains,  map->hash_function(key) % map->capacity), inserted);
	}

	map->size ++;

	float load_factor = (float)(map->size) / map->capacity;
	if (load_factor> MAX_LOAD_FACTOR )
		rehash(map);
}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node == MAP_EOF)
		return false;


	// destroy
	if (map->destroy_key != NULL)
		map->destroy_key(node->key);
	if (map->destroy_value != NULL)
		map->destroy_value(node->value);
	map->size--;
	node->state = EMPTY;
	return true;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node != MAP_EOF)
		return node->value;
	else
		return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	 DestroyFunc old = map->destroy_value;
	 map->destroy_value = destroy_value;
	 return old;
	
}

// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
	for(uint i = 0;i < map->capacity;i++){
		if(map->array[i].state == OCCUPIED){
			if (map->destroy_key != NULL)
				map->destroy_key(map->array[i].key);
			if (map->destroy_value != NULL)
				map->destroy_value(map->array[i].value);
		}
				// Αποδέσμευση vector για separate-chaining
		if (vector_get_at(map->chains, i) != NULL) {
			Vector vec = vector_get_at(map->chains, i);
			for(VectorNode lnode = vector_first(vec);
				lnode != VECTOR_EOF;
				lnode = vector_next(vec, lnode)) {

				MapNode node = vector_node_value(vec, lnode);
				free(node);
			}
			vector_destroy(vec);
		}
	}
	vector_destroy(map->chains);
	free(map->array);
	free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {
	// Ξεκινάμε την επανάληψή μας απο το 1ο στοιχείο, μέχρι να βρούμε κάτι όντως τοποθετημένο
	for (int i = 0; i < map->capacity; i++){
		if (map->array[i].state == OCCUPIED){
			return &map->array[i];
		}
	}
	//αν δεν υπαρχει κανενα στον πινακα
	for(uint i = 0;i<map->capacity;i++){
		Vector vec = vector_get_at(map->chains,i);
		for(VectorNode node = vector_first(vec);
			node != VECTOR_EOF;
			node = vector_next(vec,node))
			{
				//το περιεχομενο του κομβου vectornode σε καθε chain ειναι ενα mapnode!!
				MapNode m_node = vector_node_value(vec,node);
				if (m_node->state == OCCUPIED){
					return m_node;
				}
			}
	}
	return MAP_EOF;

}

MapNode map_next(Map map, MapNode node) {
	bool found = false;
	int i = map->hash_function(node->key) % map->capacity;	// Ξεκινάμε την αναζήτηση από το chain που το key του node hash-άρει
	if (!node->inchain) {
		// Το node είναι pointer στο i-οστό στοιχείο του array, οπότε node - array == i  (pointer arithmetic!)
		for (int j = node - map->array + 1; j < map->capacity; j++)
			if (map->array[j].state == OCCUPIED)
				return &map->array[j];

		// Αν βρισκόμαστε εδώ σημαίνει πως έχουμε διασχίσει όλα τα στοιχεία του βασικού array και πρέπει να συνεχίσουμε στα chains
		found = true;	// Προκειμένου να επιστρέψουμε το πρώτο "occupied" στοιχείο υποθέτουμε ότι έχουμε βρει ήδη το node
		i = 0;			// Ξεκινάμε από το πρώτο chain
	}

	while (i < map->capacity) {
		if (vector_get_at(map->chains, i) != NULL) {
			Vector vec = vector_get_at(map->chains, i);
			for(VectorNode lnode = vector_first(vec);	// loop node
				lnode != VECTOR_EOF;
				lnode = vector_next(vec, lnode)) {

				MapNode mnode = vector_node_value(vec, lnode);

				if (mnode->state == OCCUPIED && map->compare(mnode->key, node->key) == 0)
					found = true;	// Προκειμένου να βρούμε το σωστό επόμενο "occupied" στοιχείο πρέπει πρώτα να βρούμε το node που έχουμε.
				else if (found && mnode->state == OCCUPIED)
					return mnode;	// Εφόσον έχουμε βρει το node είμαστε στο σωστό στοιχείο.
			}
		}
		i++;	// Επόμενο chain
	}


	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	return node->key;
	
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}

MapNode map_find_node(Map map, Pointer key) {
	// Διασχίζουμε τον πίνακα, ξεκινώντας από τη θέση που κάνει hash το key,ψαχνοντας στις γειτονικες θεσεις και στο Vector που μπορει να βρισκεται

	int distance = 0;//η αποσταση απτον παροντα κομβο οσο μεγαλωνει πρεπει να ελεγχουμε αν ξεφυγαμε απο τους γειτονικους

	uint key_hash = map->hash_function(key) % map->capacity;
	for (uint pos = key_hash;										// ξεκινώντας από τη θέση που κάνει hash το key
		distance <= NEIGHBOURS;										//ψαχνουμε μονο γειτονικες θεσεις
		pos = (pos + 1) % map->capacity) {						    // linear probing, γυρνώντας στην αρχή όταν φτάσουμε στη τέλος του πίνακα

		//ελέγχουμε αν το key είναι εδώ
		if (map->array[pos].state == OCCUPIED && map->compare(map->array[pos].key, key) == 0){
			return &map->array[pos];
		}
		distance++;
	}

	//αν συνεχισει η συναρτηση να εκτελείται σημαινει οτι δεν βρέθηκε το key στον πίνακα και πρέπει να ψαξουμε
	//στο αντιστοιχο vector
	Vector current_vec =vector_get_at(map->chains,key_hash);
	
	if (current_vec != NULL){
		for (uint i = 0;i < vector_size(current_vec);i++){
			MapNode node = (MapNode)vector_get_at(current_vec,i);
			if (node->state == OCCUPIED && map->compare(node->key, key) == 0){
				return node;
			}
		}

	}
	return MAP_EOF;
}

// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}

uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}

uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}