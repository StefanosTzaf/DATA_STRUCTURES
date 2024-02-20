#include "acutest.h"
#include "ADTRecTree.h"



void test_rec_tree(void){
    //Δημιουργούμε ενα δέντρο μόνο με ρίζα
    int x = 10;
    RecTree tree1;
    tree1 = rectree_create(&x,REC_TREE_EMPTY,REC_TREE_EMPTY);
    
    TEST_ASSERT(tree1 != REC_TREE_EMPTY);  

    //το size πρεπει να ειναι 1
    TEST_ASSERT(rectree_size(tree1) == 1);
    rectree_destroy(tree1);

    //Αν δημιουργήσουμε δυο δεντρα και σε ενα τριτο τα βαλουμε
    //ως υποδέντρα ,το size πρεπει να γινει 3
    RecTree tree2,tree3,tree4;
    int u= -5;
    tree4=rectree_create(&u,REC_TREE_EMPTY,REC_TREE_EMPTY);
    int y = 20;
    tree2 = rectree_create(&y,REC_TREE_EMPTY,REC_TREE_EMPTY);
    int z = 30;
    tree3 = rectree_create(&z,tree4,REC_TREE_EMPTY);
    int v = 11;
    tree1 = rectree_create(&v,tree2,tree3);
    

    TEST_ASSERT(rectree_size(tree2) == 1);
    TEST_ASSERT(rectree_size(tree1) == 4);

    //θελουμε να μας επιστρεψει 20 ,αφου ως αριστερο υποδεντρο του tree1 εχουμε βαλει το tree2
    Pointer value = rectree_value(rectree_left(tree1));
    TEST_ASSERT((*(int*)value) == 20);


    RecTree subtree1 = rectree_left(tree1);
    TEST_ASSERT(rectree_size(subtree1) == 1);

    RecTree subtree2 = rectree_right(tree1);
    //TO ΔΕΝΤΡΟ ΠΟΥ ΔΙΝΟΥΜΕ ΕΙΝΑΙ ΚΕΝΟ
    TEST_ASSERT(rectree_size(rectree_right(subtree2)) == 0);




    //ελεγχος συναρτήσεων για κενά δεντρα
    RecTree emp = REC_TREE_EMPTY;
    TEST_ASSERT(rectree_size(emp) == 0);
    TEST_ASSERT(rectree_left(emp) == REC_TREE_EMPTY);
    TEST_ASSERT(rectree_right(emp) == REC_TREE_EMPTY);
    TEST_ASSERT(rectree_value(emp) == NULL);
    rectree_destroy(tree1);
    rectree_destroy(tree2);
    rectree_destroy(tree3);
    rectree_destroy(tree4);
    
}




// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "TestRecTree", test_rec_tree },
	
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
