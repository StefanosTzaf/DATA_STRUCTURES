#include "acutest.h"            // Απλή βιβλιοθήκη για unit testing
#include "ADTRecTree.h"
#include "ADTRecTree_utils.h"


void test_get_subtree (void) {
    //δηλωση στατικών μεταβλητων για να μην εχουμε θεμα με το valgrind λόγω της create int
    
    int x0 = 0;  int x1 = 1;  int x2 = 2;  int x3 = 3;  int x4 = 4;  int x5 = 5;  int x6 = 6;
    int x7 = 7;  int x8 = 8;  int x9 = 9;  int x10 = 10;  int x11 = 11;  int x12 = 12;  int x13 = 13;
    int x14= 14;
    RecTree tree14 = rectree_create(&x14, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree13 = rectree_create(&x13, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree12 = rectree_create(&x12, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree11 = rectree_create(&x11, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree10 = rectree_create(&x10, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree9 = rectree_create(&x9, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree8 = rectree_create(&x8, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree7 = rectree_create(&x7, REC_TREE_EMPTY, REC_TREE_EMPTY);

    RecTree tree6 = rectree_create(&x6, tree13, tree14);
    RecTree tree5 = rectree_create(&x5, tree11, tree12);
    RecTree tree4 = rectree_create(&x4, tree9, tree10);
    RecTree tree3 = rectree_create(&x3, tree7, tree8);
    RecTree tree2 = rectree_create(&x2, tree5, tree6);
    RecTree tree1 = rectree_create(&x1, tree3, tree4);
    RecTree tree0 = rectree_create(&x0, tree1, tree2);
   
    TEST_ASSERT(rectree_size(tree2) == 7);
    TEST_ASSERT(rectree_size(tree3) == 3);
    TEST_ASSERT(rectree_size(tree0) == 15);

    RecTree temp1 = rectree_get_subtree(tree0,2);
    TEST_ASSERT(rectree_size( temp1 ) == 7);
    TEST_ASSERT(*(int*)rectree_value(temp1) == 2);


    TEST_ASSERT(rectree_size( rectree_get_subtree(tree0,3) ) == 3);
    
    TEST_ASSERT(rectree_size( rectree_get_subtree(tree0,7) ) == 1);
    TEST_ASSERT(rectree_size( rectree_get_subtree(tree0,15) ) == 0);
    //το δεντρο που ζητάμε δεv υφίσταται
    TEST_ASSERT(rectree_size( rectree_get_subtree(tree0,20)) == 0);

    
    rectree_destroy(tree14); rectree_destroy(tree13); rectree_destroy(tree12); rectree_destroy(tree11);
    rectree_destroy(tree10); rectree_destroy(tree9); rectree_destroy(tree8); rectree_destroy(tree7); 
    rectree_destroy(tree6); rectree_destroy(tree5); rectree_destroy(tree4); rectree_destroy(tree3);
    rectree_destroy(tree2); rectree_destroy(tree1); rectree_destroy(tree0);
}

void test_replace_subtree(void){
    int x0 = 0;  int x1 = 1;  int x2 = 2;  int x3 = 3;  int x4 = 4;  int x5 = 5;  int x6 = 6;
    int x7 = 7;  int x8 = 8;  int x9 = 9;  int x10 = 10;  int x11 = 11;  int x12 = 12;  int x13 = 13;
    int x14= 14;
    RecTree tree14 = rectree_create(&x14, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree13 = rectree_create(&x13, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree12 = rectree_create(&x12, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree11 = rectree_create(&x11, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree10 = rectree_create(&x10, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree9 = rectree_create(&x9, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree8 = rectree_create(&x8, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree7 = rectree_create(&x7, REC_TREE_EMPTY, REC_TREE_EMPTY);

    RecTree tree6 = rectree_create(&x6, tree13, tree14);
    RecTree tree5 = rectree_create(&x5, tree11, tree12);
    RecTree tree4 = rectree_create(&x4, tree9, tree10);
    RecTree tree3 = rectree_create(&x3, tree7, tree8);
    RecTree tree2 = rectree_create(&x2, tree5, tree6);
    RecTree tree1 = rectree_create(&x1, tree3, tree4);
    RecTree tree0 = rectree_create(&x0, tree1, tree2);
    
    int y1 = 111;
    int y3 = 222;
   
    RecTree tree_temp1 = rectree_create(&y3, REC_TREE_EMPTY, REC_TREE_EMPTY);
    RecTree tree_temp0 = rectree_create(&y1, REC_TREE_EMPTY, tree_temp1);
    
    RecTree replaced = rectree_replace_subtree(tree0,3,tree_temp0);
   
    
    TEST_ASSERT(rectree_size(replaced) == 14);
    //η τιμη πρεπει να χει παραμεινει 2 στο δεξι υποδεντρο (η θεση 3 ειναι στο αριστερο)
    TEST_ASSERT(*(int*)rectree_value( rectree_right(replaced)) == 2);
   
    //ελεγχουμε αν το υποδεντρο μπήκε εκει που πρέπει
    TEST_ASSERT(*(int*)rectree_value( (rectree_left(rectree_left(replaced)))) == 111);
    TEST_ASSERT(*(int*)rectree_value(rectree_right (rectree_left(rectree_left((replaced))) ))== 222);


    
    TEST_ASSERT(rectree_left (rectree_left(rectree_left((replaced)))) ==REC_TREE_EMPTY);
     rectree_destroy(tree14); rectree_destroy(tree13); rectree_destroy(tree12); rectree_destroy(tree11);
     rectree_destroy(tree10); rectree_destroy(tree9); rectree_destroy(tree8); rectree_destroy(tree7); 
     rectree_destroy(tree6); rectree_destroy(tree5); rectree_destroy(tree4);
     rectree_destroy(tree2);  
    //ΤΑ tree0,tree1,tree3 που βρισκονται στο μονοπατι απο την ριζα στο pos εχουν ηδη 
    //καταστραφει μεσα στην συναρτηση αρα δεν πρπεει να ξανακανουμε destroy για να μην υπαρχει θεμα
    //με double free
   
    //!!! με την κληση της replace δημηιουργηθηκαν νεα δεντρα(στην διαδρομη απο την ριζα μεχρι την θεση 
    //που εγινε το replace) είναι ευθυνη του χρηστη να τα καταστρέψει ,αρα μιας και χρησημοποιουμε την συναρτηση
    //στην περιπτωση μας δημιουργηθηκε νεα ριζα την καταστρεφουμε με το rectree_destroy(replaced); 
    // το αριστερο παιδι της νεας ριζας(rectree_destroy((rectree_left(replaced)));)
    //και το ίδιο το subtree (και το παιδι του);
    rectree_destroy(tree_temp1);
    rectree_destroy(tree_temp0);
    rectree_destroy((rectree_left(replaced)));
    rectree_destroy(replaced); 
    
    
}


TEST_LIST = {
    { "test_get_subtree", test_get_subtree },
    { "test_replace_subtree", test_replace_subtree },

    { NULL} // τερματίζουμε τη λίστα με NULL
};