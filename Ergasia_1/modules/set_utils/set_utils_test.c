#include "acutest.h"            // Απλή βιβλιοθήκη για unit testing

#include "set_utils.h"
#include "state.h"

int compare_ints(Pointer a, Pointer b) {
    int pa = *(int*)a;
    int pb = *(int*)b;
    return pa - pb;
}

int* create_int(int value) {
    int* res = malloc(sizeof(int));
    *res = value;
    return res; 
}

//μια απλη create object αφου μονο το χ αφου αυτο παιζει σημασια στην διαταξη
Object create_objs(float x) {
    Object obj = malloc(sizeof(*obj));
    obj->rect.x = x;
    return obj; 

}
int objs_compare(Pointer a,Pointer b){
	Object a1 = a;
    Object b1 = b;
	if(a1->rect.x> b1->rect.x){
        return 1;
    }
    else if(a1->rect.x < b1->rect.x){
        return -1;
    }
    else{
        return 0;
    }
}

void test_set_find_eq_or() {
     
    Set set = set_create(compare_ints, NULL);
    set_insert(set, create_int(9));
    set_insert(set, create_int(3));
    set_insert(set, create_int(-6));
    set_insert(set, create_int(10));
    set_insert(set, create_int(2));
    set_insert(set, create_int(30));
    
    //πρεπει να επιστρέφoυν το ιδιο το value αφου υπαρχει στο set.  
    int* fun1 = set_find_eq_or_greater(set, create_int(-6));  
    int* fun2 = set_find_eq_or_smaller(set, create_int(-6));


    TEST_ASSERT(*fun1 == -6);
    TEST_ASSERT(*fun2 == -6);

//πρεπει να επιστρέφει το αμέσως μεγαλυτερο στοιχειο του set μετα το value.
//και το αμέσως μικρότερο αντίστοιχα
    fun1 = set_find_eq_or_greater(set, create_int(5));
    fun2 = set_find_eq_or_smaller(set, create_int(5));

    TEST_ASSERT(*fun1 == 9);
    TEST_ASSERT(*fun2 == 3);

//Επιστρέφει NULL αν το value ειναι το μεγαλύτερο στοιχείο του set
//(και αντίστοιχα το μικρότερο)
    fun1 = set_find_eq_or_greater(set, create_int(31));
    TEST_ASSERT(fun1 == NULL);

    fun2 = set_find_eq_or_smaller(set, create_int(-7));
    TEST_ASSERT(fun2 == NULL);

    

//tests με objects

    Set set1 = set_create(objs_compare, NULL);
    set_insert(set1, create_objs(9));
    set_insert(set1, create_objs(3));
    set_insert(set1, create_objs(-6));
    set_insert(set1, create_objs(10));
    set_insert(set1, create_objs(2));

    Object obj1 = set_find_eq_or_greater(set1, create_objs(9));
    TEST_ASSERT(obj1->rect.x == 9);
    obj1 = set_find_eq_or_greater(set1, create_objs(5));
    Object obj2 = set_find_eq_or_smaller(set1, create_objs(5));

    TEST_ASSERT(obj1->rect.x == 9);
    TEST_ASSERT(obj2->rect.x == 3);
}


void test_objs_compare(){
    Object a=malloc(sizeof(*a));
    Object b=malloc(sizeof(*b));
    a->rect.x = 10.0;
    b->rect.x = 11.0;
    int x = objs_compare(a,b);
    //πρεπει να επιστρέφει αρνητικο αφου a<b
    TEST_ASSERT(x < 0);

    a->rect.x = 15.0;
    b->rect.x = 11.0;
    x=objs_compare(a,b);
    TEST_ASSERT(x>0);

    a->rect.x=10.0;
    b->rect.x=10.2;
    x=objs_compare(a,b);
    TEST_ASSERT(x<0);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
    { "test_set_find_eq_or", test_set_find_eq_or},
    {"test_objs_compare",test_objs_compare},

    { NULL} // τερματίζουμε τη λίστα με NULL
};