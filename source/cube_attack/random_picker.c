int* random_picker(int lower, int upper){
    int range = lower-upper;
    int* list = malloc(sizeof(int)*range);
    for(int i=lower;i<upper; i++){
        list[i] = i;
    }
    for(int g=0;g<2;g++) {
        for (int i = lower; i < upper; i++) {
            int first = rand()%range;
            int second = rand()%range;
            int temp = list[first];
            list[first] = list[second];
            list[second] = temp;
        }
    }
}