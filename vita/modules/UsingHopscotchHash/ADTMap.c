/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Hopscotch hashing
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>

#include "ADTMap.h"

// Κάθε θέση i θεωρείται γεινοτική με όλες τις θέσεις μέχρι και την i + NEIGHBOURS
#define NEIGHBOURS 3

// Οι κόμβοι του map στην υλοποίηση με hash table, μπορούν να είναι σε 3 διαφορετικές καταστάσεις,
// ώστε αν διαγράψουμε κάποιον κόμβο, αυτός να μην είναι empty, ώστε να μην επηρεάζεται η αναζήτηση
// αλλά ούτε occupied, ώστε η εισαγωγή να μπορεί να το κάνει overwrite.
typedef enum {
	EMPTY, OCCUPIED
} State;

// Το μέγεθος του Hash Table ιδανικά θέλουμε να είναι πρώτος αριθμός σύμφωνα με την θεωρία.
// Η παρακάτω λίστα περιέχει πρώτους οι οποίοι έχουν αποδεδιγμένα καλή συμπεριφορά ως μεγέθη.
// Κάθε re-hash θα γίνεται βάσει αυτής της λίστας. Αν χρειάζονται παραπάνω απο 1610612741 στοχεία, τότε σε καθε rehash διπλασιάζουμε το μέγεθος.
int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

// Χρησιμοποιούμε open addressing, οπότε σύμφωνα με την θεωρία, πρέπει πάντα να διατηρούμε
// τον load factor του  hash table μικρότερο ή ίσο του 0.5, για να έχουμε αποδoτικές πράξεις
#define MAX_LOAD_FACTOR 0.5

// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;		// Το κλειδί που χρησιμοποιείται για να hash-αρουμε
	Pointer value;  	// Η τιμή που αντισοιχίζεται στο παραπάνω κλειδί
	State state;		// Μεταβλητή για να μαρκάρουμε την κατάσταση των κόμβων (βλέπε διαγραφή)
};

// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	MapNode array;				// Ο πίνακας που θα χρησιμοποιήσουμε για το map (remember, φτιάχνουμε ένα hash table)
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

	// Αρχικοποιούμε τους κόμβους που έχουμε σαν διαθέσιμους.
	for (int i = 0; i < map->capacity; i++)
		map->array[i].state = EMPTY;

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

// Συνάρτηση για την επέκταση του Hash Table σε περίπτωση που ο load factor μεγαλώσει πολύ.
static void rehash(Map map) {
	// Αποθήκευση των παλιών δεδομένων
	int old_capacity = map->capacity;
	MapNode old_array = map->array;

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
	for (int i = 0; i < map->capacity; i++)
		map->array[i].state = EMPTY;

	// Τοποθετούμε ΜΟΝΟ τα entries που όντως περιέχουν ένα στοιχείο (το rehash είναι και μία ευκαιρία να ξεφορτωθούμε τα deleted nodes)
	map->size = 0;
	for (int i = 0; i < old_capacity; i++)
		if (old_array[i].state == OCCUPIED)
			map_insert(map, old_array[i].key, old_array[i].value);

	//Αποδεσμεύουμε τον παλιό πίνακα ώστε να μήν έχουμε leaks
	free(old_array);
}

// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.

void swap_nodes(MapNode node_a, MapNode node_b) {
	MapNode temp = malloc(sizeof(*temp));
	temp->key = node_a->key;	
	temp->state = node_a->state;
	temp->value = node_a->value;
	node_a->key = node_b->key;
	node_a->state = node_b->state;
	node_a->value = node_b->value;
	node_b->key = temp->key;
	node_b->state = temp->state;
	node_b->value = temp->value;
	free(temp);
}


void map_insert(Map map, Pointer key, Pointer value) {
	// Υποσημείωση: Αρχικά είχα χρησιμοποιήσει ένα flag για τα στοιχεία που υπάρχουν ήδη στο array αλλά αντιμετώπιζα ένα bug
	// Αρχικα ελέγχω αν υπάρχει το key
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
        node->state = OCCUPIED;
        return;
    }
	uint hash = map->hash_function(key) % map->capacity;		// Η θέση όπου το key hash-άρει
	int distance = 0;											// Απόσταση από τη θέση που το στοιχείο hash-άρει

	// Εύρεση πρώτης ελεύθερης θέσης
	uint pos;
	for (pos = hash;											// ξεκινώντας από τη θέση που κάνει hash το key
		map->array[pos].state != EMPTY;							// αν φτάσουμε σε EMPTY σταματάμε
		pos = (pos + 1) % map->capacity) {						// linear probing, γυρνώντας στην αρχή όταν φτάσουμε στη τέλος του πίνακα

		if (distance != 0 && pos == hash) {						// Αν ξαναβρεθούμε στο pos που ξεκινήσαμε δεν υπάρχει κενή θέση.
			rehash(map);										// Κάνουμε rehash και προσπαθούμε να πάλι να εισάγουμε το στοιχείο
			map_insert(map, key, value);
			return;
		}
		distance++;												// Απόσταση αρχικής θέσης και πρώτης κενής θέσης.
	}
	if (distance <= NEIGHBOURS) {								// Μπορούμε να το τοποθετήσουμε εκεί χωρίς παραπάνω διαδικασίες
		node = &map->array[pos];
	}
	else {
		// Πρέπει να μεταφέρω την κενή θέση κοντά στη "γειτονία" του στοιχείου που θέλουμε να τοποθετήσουμε
		// Επαναλαμβάνω μέχρις ότου η κενή θέση να έρθει αρκετά κοντά στη θέση που το στοιχείο hash-άρει
		while ((map->hash_function(key) + NEIGHBOURS) % map->capacity < pos) {
			bool flag = false;													// flag αν μπόρεσε να γίνει swap
			for (int i = 0; i < NEIGHBOURS; i++) {
				MapNode cand = &map->array[(pos - NEIGHBOURS) + i];				// Υποψήφιο στοιχείο
				uint candhash = map->hash_function(cand->key) % map->capacity;	// Που hash-άρει το key του υποψήφιου στοιχείου
				if (candhash + NEIGHBOURS == pos) {								// Αν μπορεί να γίνει swap
					// Ανταλλαγή στοιχείων
					swap_nodes(cand, &map->array[pos]);
					pos = (pos - NEIGHBOURS) + i;
					flag = true;							// Έγινε ανταλλαγή
					break;
				}
			}

			if (!flag) {									// Αν δεν κατάφερε να γίνει swap χρειάζεται να κάνουμε rehash
				rehash(map);
				map_insert(map, key, value);
				return;
			}
		}
		node = &map->array[pos];							//	Βρέθηκε κενή θέση
	}
	// Προσθήκη τιμών στον κόμβο
	node->state = OCCUPIED;
	node->key = key;
	node->value = value;

	// Νέο στοιχείο, αυξάνουμε τα συνολικά στοιχεία του map
	map->size++;

	// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash.
	// Στο load factor μετράμε και τα DELETED, γιατί και αυτά επηρρεάζουν τις αναζητήσεις.
	float load_factor = (float)map->size / map->capacity;	
	if (load_factor > MAX_LOAD_FACTOR)
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

	// θέτουμε ως "deleted", ώστε να μην διακόπτεται η αναζήτηση, αλλά ταυτόχρονα να γίνεται ομαλά η εισαγωγή
	node->state = EMPTY;	
	map->size--;

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
	for (int i = 0; i < map->capacity; i++) {
		if (map->array[i].state == OCCUPIED) {
			if (map->destroy_key != NULL)
				map->destroy_key(map->array[i].key);
			if (map->destroy_value != NULL)
				map->destroy_value(map->array[i].value);
		}
	}

	free(map->array);
	free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {
	//Ξεκινάμε την επανάληψή μας απο το 1ο στοιχείο, μέχρι να βρούμε κάτι όντως τοποθετημένο
	for (int i = 0; i < map->capacity; i++)
		if (map->array[i].state == OCCUPIED)
			return &map->array[i];

	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	// Το node είναι pointer στο i-οστό στοιχείο του array, οπότε node - array == i  (pointer arithmetic!)
	for (int i = node - map->array + 1; i < map->capacity; i++)
		if (map->array[i].state == OCCUPIED)
			return &map->array[i];

	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	return node->key;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}

MapNode map_find_node(Map map, Pointer key) {
	// Διασχίζουμε τον πίνακα, ξεκινώντας από τη θέση που κάνει hash το key, και για όσο δε βρίσκουμε EMPTY
	int distance = 0;
	for (uint pos = map->hash_function(key) % map->capacity;	// ξεκινώντας από τη θέση που κάνει hash το key
		distance <= NEIGHBOURS;									// αν φτάσουμε σε EMPTY σταματάμε
		pos = (pos + 1) % map->capacity) {						// linear probing, γυρνώντας στην αρχή όταν φτάσουμε στη τέλος του πίνακα

		// Μόνο σε OCCUPIED θέσεις, ελέγχουμε αν το key είναι εδώ
		if (map->array[pos].state == OCCUPIED && map->compare(map->array[pos].key, key) == 0)
			return &map->array[pos];

		distance++;
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