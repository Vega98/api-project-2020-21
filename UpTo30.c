#include <stdio.h>
#include <stdbool.h>

//Numero di nodi, lunghezza della classifica, somma dei cammini minimi, numero di nodi rimossi dalla coda, flags vari, lunghezza classifica
int n, k, graph_counter, deleted_nodes, max_heapified, not_first, ranking_lenght;

typedef struct {
    int id, path_sum;
}graph;

typedef struct {
    int id, distance, flag;
}node;

typedef struct {
    graph * graphs;
    int capacity, curr_size;
}maxHeap;

//Funzioni di utilità che dato un certo indice restituiscono l'indice dei figli o del genitore
int leftChild(int i) { return (2 * i) + 1; }
int rightChild(int i) { return (2 * i) + 2; }
int parent(int i) { return (i - 1) / 2; }

//Funzione di utilità che scambia il contenuto di due nodi
void swapNodes(graph ranking[], int node1, int node2){
    int id_tmp, dist_tmp;
    id_tmp = ranking[node1].id;
    dist_tmp = ranking[node1].path_sum;
    ranking[node1].id = ranking[node2].id;
    ranking[node1].path_sum = ranking[node2].path_sum;
    ranking[node2].id = id_tmp;
    ranking[node2].path_sum = dist_tmp;
}

//Riordina il subtree per renderlo un maxHeap
void pushDown(graph ranking[], int node) {
    int bigger = node;
    //il nodo ha entrambi i figli? vuol dire che ha anche il figlio destro. se sì confronta il loro path_sum e prendi l'indice del figlio con quello maggiore
    if (rightChild(node) < k) {
        if (ranking[rightChild(node)].path_sum > ranking[leftChild(node)].path_sum)
            bigger = rightChild(node);
        else bigger = leftChild(node);
    }
        //il nodo ha solo il figlio sinistro? prendi l'indice del figlio sinistro
    else if (leftChild(node) < k) {
        bigger = leftChild(node);
    }
    //il nodo ha figli? se non ha figli, non devo fare nulla.
    if (bigger != node) {
        //il nodo ha una chiave minore del figlio selezionato? se sì, sostituisci i due nodi
        if (ranking[node].path_sum < ranking[bigger].path_sum) {
            swapNodes(ranking, node, bigger);
            pushDown(ranking, bigger);
        }
    }
}

//Funzione per riodinare la classifica la prima volta quando viene riempita
void maxHeapify(graph ranking[]){
    for (int i = k/2; i >=0; i--){
        pushDown(ranking, i);
    }
}

//Funzione per cercare il minimo nella coda di priorità di dijkstra
int findMin(node * priority_list){
    //trova il nodo con distanza minima e flag positivo, pone il flag di tale nodo a 0 così non lo leggo più nei prossime run
    //incrementa il contatore di nodi cancellati
    int min = -1, min_index = 0, gotbase = 0;
    for (int i = 0; i < n; i++){
        if (priority_list[i].flag == 1 && priority_list[i].distance != -1){ //se la distanza è -1 ignoro il nodo perché ha distanza infinita
            if (gotbase == 0 ) {
                min = priority_list[i].distance;
                min_index = i;
                gotbase = 1;
            } else if (priority_list[i].distance < min || min == -1){
                min = priority_list[i].distance;
                min_index = i;
            }
        }
    }

    priority_list[min_index].flag = 0;
    deleted_nodes++;
    return min_index;
}

//Funzione che implementa il calcolo dei cammini minimi
int dijkstra(node priority_list[n], int incidence_matrix[n][n]) {
    int i, weight, path_sum = 0;
    while (deleted_nodes != n) {
        i = findMin(priority_list); //prendo il nodo con la minor distanza dalla priority list
        //printf("\nEstraggo nodo minimo (%d), cammino minimo: %d", priority_list[i].id, priority_list[i].distance);
        path_sum += priority_list[i].distance;
        for (int j = 0; j < n; j++) {
            weight = incidence_matrix[i][j];
            //se la distanza dal j-esimo nodo è non nulla e ho trovato un cammino migliore, allora aggiorno la distanza
            if (weight != 0 && ( weight + priority_list[i].distance < priority_list[j].distance || priority_list[j].distance == -1)) {
                //aggiorna la distanza del j-esimo nodo, facendola diventare min_node.distance + weight
                priority_list[j].distance = weight + priority_list[i].distance;
            }
        }
    }
    //printf("\npathsum calcolato: %d\n", path_sum);
    return path_sum;
}

//Restituisco l'input, ignorando le virgole che separano i nodi e \n
int parseInput(){
    char temp = '0';
    int output = 0, input = 0;
    while (temp != '\n' && temp != ',' && temp != ' '){
        temp = getchar_unlocked();
        if (temp != ',' && temp != '\n' && temp != ' ') input = (10 * input) + (temp - '0');
        else output = input;
    }
    return output;
}

//Sostituisce un elemento nell'heap preservando le proprietà
void heapInsert(graph ranking[], int path_sum){
    //se il grafo da inserire ha un peso minore del massimo, lo sostituisco alla radice e poi chiamo maxHeapify
    if (ranking[0].path_sum > path_sum){
        //printf("\nSostituisco il grafo %d con il grafo %d\n", ranking[0].id, graph_counter);
        ranking[0].id = graph_counter;
        ranking[0].path_sum = path_sum;
        pushDown(ranking, 0);
        //maxHeapify(ranking);
    }
}

//Leggo la matrice di incidenza del grafo, valuto la somma dei cammini minimi e aggiungo il grafo in classifica
void addToRanking(graph ranking[]){
    deleted_nodes = 0;
    int path_sum;
    node priority_list[n];
    int incidence_matrix[n][n]; //<- anche lei forse come variabile globale? come tutte le altre variabili che non cambiano dimensione
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            incidence_matrix[i][j] = parseInput();
        }
        priority_list[i].id = i;
        priority_list[i].distance = -1;
        priority_list[i].flag = 1;
    }
    priority_list[0].distance = 0; //Imposto la distanza del nodo 0 da se stesso pari a zero
    path_sum = dijkstra(priority_list, incidence_matrix); //calcolo la somma dei cammini minimi del grafo
    if (graph_counter < k) {
        ranking[graph_counter].id = graph_counter;//Finchè il maxHeap non è pieno, posso semplicemente aggiungere i grafi come fosse un vettore normale
        ranking[graph_counter].path_sum = path_sum;
        ranking_lenght++;
        //printf("\naggiunto grafo %d in classifica con path_sum = %d\n", ranking[graph_counter].id, ranking[graph_counter].path_sum);
    }
    else { //Quando invece il maxHeap è pieno, devo cominciare a sostituire quindi ristabilisco le propietà di heap
        if (!max_heapified){
            maxHeapify(ranking);
            //printf("\nClassifica ordinata");
            max_heapified = 1;
        }
        heapInsert(ranking, path_sum);
    }
    graph_counter++;
}

//Funzione che stampa la classifica
void topk(graph ranking[]){
    if (not_first == 1) printf("\n");
    not_first = 1;
    for (int i = 0; i < ranking_lenght/2; i++){
        //printf("\nStampo il grafo in posizione %d: ",i);
        if (i < (ranking_lenght/2)-1) {
            printf("%d %d ", ranking[i].id, ranking[ranking_lenght - i - 1].id);
        } else {
            printf("%d %d", ranking[i].id, ranking[ranking_lenght - i - 1].id);
        }
    } if(ranking_lenght % 2 == 1){
        printf(" %d", ranking[ranking_lenght/2].id);
    }
}

//Funzione di parsing dei comandi
bool parseCommands(graph ranking[]){
    char cmd, temp;
    bool tookFirstChar = false;
    do {
        temp=getchar_unlocked();
        if (temp==EOF) {
            return false;
        } else if (!tookFirstChar){
            cmd=temp;
            tookFirstChar = true;
        }
    } while (temp!='\n');
    if (cmd == 'A')
        addToRanking(ranking);
    if (cmd == 'T')
        topk(ranking);
    return true;
}

int main() {
    bool not_done;
    graph_counter = 0; //-1
    max_heapified = 0;
    ranking_lenght = 0;
    //Prendi n e k dall'input
    n = parseInput();
    k = parseInput();
    graph ranking[k];
    //printf("\nn=%d k=%d\n", n, k);
    do {
        not_done = parseCommands(ranking);
    } while (not_done);
    return 0;
}
