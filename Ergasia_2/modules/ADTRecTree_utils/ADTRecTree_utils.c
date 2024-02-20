#include <stdlib.h>
#include "ADTRecTree_utils.h"

RecTree rectree_get_subtree(RecTree tree, int pos){
    if (pos == 0){
        return tree;
    }
    int parent_pos = (pos-1)/2;
    RecTree desired_tree = REC_TREE_EMPTY;
    //αναδρομικά βρίσκουμε το υποδέντρο που θέλουμε

    RecTree parent = rectree_get_subtree(tree,parent_pos);
    //αν η θεση του παιδιου ειναι οσο του πατερα*2 +1 ειναι αριστερο παιδι αλλιως δεξι

    if(pos == parent_pos*2 +1){
       desired_tree = rectree_left(parent);
    }
    else if(pos == parent_pos*2 +2){
        desired_tree = rectree_right(parent);
    }

    return desired_tree;

}



RecTree rectree_replace_subtree(RecTree tree, int pos, RecTree subtree){
    
    if(pos == 0){
        rectree_destroy(tree);
        return subtree;
    }
    
    //θεση του πατερα
    int parent_pos = (pos-1)/2;
    //διαγραφει το υποδενδρο στη θεση του οποιου πρεπει να μπει το subtree
    
    rectree_destroy(rectree_get_subtree(tree,pos));
    
    //ο πατερας πρεπει και αυτος να διαγραφει και όποιος αλλος κόμβος βρίσκεται 
    //στην διαδρομη προς τη ριζα.(να αντικατασταθει απο νεο)
    while(1){
        RecTree parent_to_destroy = rectree_get_subtree(tree,parent_pos);
    
        //το subtree μεγαλωνει καθε φορα

        //αν το υποδεντρο πρεπει να μπει ως αριστερο παιδι του γονιου
        if(pos == parent_pos*2 +1){
            //αφηνουμε value και δεξι παιδι αυτα που προυπύρχαν
            subtree = rectree_create(rectree_value(parent_to_destroy) ,subtree, rectree_right(parent_to_destroy));
        }
        //αν το υποδεντρο πρεπει να μπει ως δεξι παιδι του γονιου
        else if(pos == parent_pos*2 +2){
            //αφηνουμε value και αριστερο παιδι αυτα που προυπύρχαν
            subtree = rectree_create(rectree_value(parent_to_destroy) ,rectree_left(parent_to_destroy),subtree );
        }

        //ο γονιος πια δεν μας χρειάζεται
        rectree_destroy(parent_to_destroy);

        //θελουμε να ελεγχεται η συνθηκη στο τελος πριν ομως αλλαξουμε το ποιος ειναι γονιος
        if(parent_pos == 0){
            break;
        }
        //ανεβαινυομε προς τα πάνω μεχρι να φτάσουμε στην ρίζα
        pos = parent_pos;
        parent_pos = (pos-1)/2;
    
    }
   
    return subtree;
}
