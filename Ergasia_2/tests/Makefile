# Κάνοντας compile το <foo>_test.c με μια υλοποίηση <foo>.c του
# συγκεκριμένου τύπου, παράγουμε ένα tets για την υλοποίηση αυτή.

# Υλοποιήσεις μέσω Heap: ADTPriorityQueue
# Το Heap χρησιμοποιεί Vector, οπότε χρειαζόμαστε και μια υλοποίηση του ADTVector.

UsingLinkedTree_ADTRecTree_test_EX_1_OBJS  = ADTRecTree_test.o $(MODULES)/UsingLinkedTree/ADTRecTree.o 




UsingADTMap_ADTRecTree_test_ΕΧ_2_OBJS  = ADTRecTree_test.o $(MODULES)/UsingADTMap/ADTRecTree.o $(MODULES)/UsingADTSet/ADTMap.o $(MODULES)/UsingAVL/ADTSet.o




UsingADTRecTree_utils_test_ΕΧ_3_map_OBJS = ADTRecTree_utils_test.o $(MODULES)/UsingADTMap/ADTRecTree.o $(MODULES)/UsingADTSet/ADTMap.o $(MODULES)/UsingAVL/ADTSet.o $(MODULES)/ADTRecTree_utils/ADTRecTree_utils.o 

UsingADTRecTree_utils_test_ΕΧ_3_linkedtree_OBJS = ADTRecTree_utils_test.o $(MODULES)/UsingLinkedTree/ADTRecTree.o $(MODULES)/ADTRecTree_utils/ADTRecTree_utils.o 




UsingADTRecTree_ADTCompTree_test_EX_4_linkedtree_OBJS = ADTCompTree_test.o $(MODULES)/UsingADTRecTree/ADTCompTree.o $(MODULES)/UsingLinkedTree/ADTRecTree.o  $(MODULES)/ADTRecTree_utils/ADTRecTree_utils.o

UsingADTRecTree_ADTCompTree_test_EX_4_map_OBJS = ADTCompTree_test.o $(MODULES)/UsingADTRecTree/ADTCompTree.o $(MODULES)/UsingADTMap/ADTRecTree.o $(MODULES)/UsingADTSet/ADTMap.o $(MODULES)/UsingAVL/ADTSet.o $(MODULES)/ADTRecTree_utils/ADTRecTree_utils.o



#μας χρειαζεται και η υλοποιηση του vector για τα test παρολο που δε τα χρησημοποιουμαι στην υλοποιηση του pqueue
UsingADTCompTree_ADTPriorityQueue_EX_5_linkedtree_test_OBJS  = ADTPriorityQueue_test.o $(MODULES)/UsingDynamicArray/ADTVector.o $(MODULES)/UsingDynamicArray/ADTVector.o $(MODULES)/UsingADTCompTree/ADTPriorityQueue.o $(MODULES)/UsingADTRecTree/ADTCompTree.o $(MODULES)/UsingLinkedTree/ADTRecTree.o $(MODULES)/ADTRecTree_utils/ADTRecTree_utils.o

UsingADTCompTree_ADTPriorityQueue_EX_5_map_test_OBJS  = ADTPriorityQueue_test.o $(MODULES)/UsingDynamicArray/ADTVector.o $(MODULES)/UsingADTCompTree/ADTPriorityQueue.o $(MODULES)/UsingADTRecTree/ADTCompTree.o $(MODULES)/UsingADTMap/ADTRecTree.o $(MODULES)/UsingADTSet/ADTMap.o $(MODULES)/UsingAVL/ADTSet.o $(MODULES)/ADTRecTree_utils/ADTRecTree_utils.o




UsingHeap_ADTPriorityQueue_test_OBJS  = ADTPriorityQueue_test.o $(MODULES)/UsingHeap/ADTPriorityQueue.o $(MODULES)/UsingDynamicArray/ADTVector.o
# Ο βασικός κορμός του Makefile
include ../common.mk