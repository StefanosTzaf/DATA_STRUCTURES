#include "set_utils.h"
#include <stddef.h>


Pointer set_find_eq_or_greater(Set set, Pointer value){
    Pointer p = set_find(set,value) ;
    //εάν υπάρχει το στοιχείο στο set επιστρέφουμε δείκτη σε αυτό
    if(p != NULL ){
        return p;
    }
    //αλλιώς βάζουμε το στοιχείο στο set (το οποιο θα μπει διατεταγμένο λόγω της λειτουργίας του set)
    //παίρνουμε το επόμενο του στοιχειου αυτού, αρα την μικρότερη τιμή του set που είναι μεγαλύτερη
    //από value.Τέλος αφαιρούμε το στοιχείο value αφου πριν δεν υπήρχε.
    else{
        set_insert(set , value);
        //αν ειναι το μεγαλύτερο στοιχείο του set
        if(set_find(set ,value) == set_node_value(set,set_last(set))){
            set_remove(set,value);
            return NULL;
        }
        SetNode node = set_find_node(set,value);
        SetNode node_next = set_next(set, node); 
        set_remove(set,value);
        return set_node_value(set , node_next);
    }
}
    

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    Pointer p;
    if((p = set_find(set,value)) != NULL ){
        return p;
    }
    else{
        set_insert(set , value);
        //αν ειναι το μικρότερο στοιχείο του set
        if(set_find(set ,value) == set_node_value(set,set_first(set))){
            set_remove(set,value);
            return NULL;
        }
        SetNode node = set_find_node(set,value);
        SetNode node_next = set_previous(set, node); 
        set_remove(set,value);
        return set_node_value(set , node_next);
    }    
}