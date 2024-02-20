#include "acutest.h"
#include "ADTCompTree.h"


void test_comp_tree(void){
    //Δημιουργούμε ενα δέντρο μόνο με ρίζα
    int x = 10;
    CompTree tree1;
    tree1 = comptree_create(&x,COMP_TREE_EMPTY,COMP_TREE_EMPTY);
    
    TEST_ASSERT(tree1 != COMP_TREE_EMPTY);  

    //το size πρεπει να ειναι 1
    TEST_ASSERT(comptree_size(tree1) == 1);
    comptree_destroy(tree1);

    //Αν δημιουργήσουμε δυο δεντρα και σε ενα τριτο τα βαλουμε
    //ως υποδέντρα ,το size πρεπει να γινει 3
    CompTree tree2,tree3,tree4;
    int u= -5;
    tree4=comptree_create(&u,COMP_TREE_EMPTY,COMP_TREE_EMPTY);
    int y = 20;
    tree2 = comptree_create(&y,tree4,COMP_TREE_EMPTY);
    int z = 30;
    tree3 = comptree_create(&z,COMP_TREE_EMPTY,COMP_TREE_EMPTY);
    int v = 11;
    tree1 = comptree_create(&v,tree2,tree3);
    //το δεντρο ειναι complete

    TEST_ASSERT(comptree_size(tree3) == 1);
    TEST_ASSERT(comptree_size(tree1) == 4);

    //θελουμε να μας επιστρεψει 20 ,αφου ως αριστερο υποδεντρο του tree1 εχουμε βαλει το tree2
    Pointer value = comptree_value(comptree_left(tree1));
    TEST_ASSERT((*(int*)value) == 20);


    CompTree subtree1 = comptree_right(tree1);
    TEST_ASSERT(comptree_size(subtree1) == 1);

    CompTree subtree2 = comptree_right(tree1);
    //TO ΔΕΝΤΡΟ ΠΟΥ ΔΙΝΟΥΜΕ ΕΙΝΑΙ ΚΕΝΟ
    TEST_ASSERT(comptree_size(comptree_right(subtree2)) == 0);


    //ελεγχος συναρτήσεων για κενά δεντρα
    CompTree emp = COMP_TREE_EMPTY;
    TEST_ASSERT(comptree_size(emp) == 0);
    TEST_ASSERT(comptree_left(emp) == COMP_TREE_EMPTY);
    TEST_ASSERT(comptree_right(emp) == COMP_TREE_EMPTY);
    TEST_ASSERT(comptree_value(emp) == NULL);



    int insert = 200;
    CompTree new = comptree_insert_last(tree1,&insert);
    //προσθεσαμε εναν κομβο
    TEST_ASSERT(comptree_size(new) == 5);
    //η τιμη μπηκε σε αυτη τη θεση
    TEST_ASSERT(*(int*)comptree_value( comptree_right(comptree_left(new)))==200);

    CompTree new2 = comptree_remove_last(new);
    TEST_ASSERT(comptree_size(new2) == 4);
    

    new2 = comptree_remove_last(new2);
    TEST_ASSERT(comptree_size(new2) == 3);
    TEST_ASSERT(*(int*)comptree_value(comptree_left(new2))==20);

    comptree_destroy(comptree_left(new2));
    comptree_destroy(comptree_right(new2));
    comptree_destroy(new2);


}




// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "TestCompTree", test_comp_tree },
	
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
