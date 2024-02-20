#include "ADTCompTree.h"
#include "ADTRecTree_utils.h"


CompTree comptree_create(Pointer value, CompTree left, CompTree right){
    return (CompTree)rectree_create(value,(RecTree)left,(RecTree)right);
}

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο (0 για κενά δέντρα).

int comptree_size(CompTree tree){
    return rectree_size((RecTree)tree);
 }

// Ελευθερώνει τη μνήμη που δεσμεύει το δέντρο tree (ΔΕΝ καταστρέφει τα υποδέντρα του).

void comptree_destroy(CompTree tree){
    rectree_destroy((RecTree)tree);
}



Pointer comptree_value(CompTree tree){
    return rectree_value((RecTree)tree);
}

CompTree comptree_left(CompTree tree){
    return (CompTree)rectree_left((RecTree)tree); 
}
CompTree comptree_right(CompTree tree){
    return (CompTree)rectree_right((RecTree)tree);
}

CompTree comptree_insert_last(CompTree tree, Pointer value){
    //το βαζουμε στην θεση size αφου δεν μπορει να χει κενα (complete δεντρο)
    RecTree new_tree = (RecTree)comptree_create(value,COMP_TREE_EMPTY,COMP_TREE_EMPTY);
    return (CompTree)rectree_replace_subtree((RecTree)tree, rectree_size((RecTree)tree) ,new_tree);

}

CompTree comptree_remove_last(CompTree tree){
    //το στοιχείο που θα πρεπει να αφαιρεσουμε βρισκεται στην θέση size - 1
    return (CompTree)rectree_replace_subtree((RecTree)tree,rectree_size((RecTree)tree)-1,REC_TREE_EMPTY);
}


CompTree comptree_get_subtree(CompTree tree, int pos){
    return (CompTree)rectree_get_subtree((RecTree)tree,pos);
}

CompTree comptree_replace_subtree(CompTree tree, int pos, CompTree subtree){
    return (CompTree)rectree_replace_subtree((RecTree)tree,pos,(RecTree)subtree);
}