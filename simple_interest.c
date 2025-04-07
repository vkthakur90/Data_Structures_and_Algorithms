#include <stdio.h>

typedef struct _ProgramData {
    struct {
        float principal;
        float rate;
        float time;
    } inputs;
    struct {
        float interest;
        float ammount;
    } outputs;
} ProgramData;

void ProgramData_readInputs(struct _ProgramData * restrict data_ptr){
    scanf("%f", &data_ptr->inputs.principal);
    scanf("%f", &data_ptr->inputs.rate);
    scanf("%f", &data_ptr->inputs.time);
}

void ProgramData_computeInterest(struct _ProgramData * restrict data_ptr){
    data_ptr->outputs.interest = 1;
    data_ptr->outputs.interest *= data_ptr->inputs.principal;
    data_ptr->outputs.interest *= data_ptr->inputs.rate / 100.0;
    data_ptr->outputs.interest *= data_ptr->inputs.time;
}

void ProgramData_computeAmmount(struct _ProgramData * restrict data_ptr){
    data_ptr->outputs.ammount = 0;
    data_ptr->outputs.ammount += data_ptr->inputs.principal;
    data_ptr->outputs.ammount += data_ptr->outputs.interest;
}

void ProgramData_compute(struct _ProgramData * restrict data_ptr){
    ProgramData_computeInterest(data_ptr);
    ProgramData_computeAmmount(data_ptr);
}

void ProgramData_displayOutputs(struct _ProgramData * restrict data_ptr){
    printf("%s\n" , "--------------------------------------------------------");
    printf("%s%f\n", "Principal: ", data_ptr->inputs.principal);
    printf("%s%f\n", "Rate     : ", data_ptr->inputs.rate);
    printf("%s%f\n", "Time     : ", data_ptr->inputs.time);
    printf("%s\n" , "--------------------------------------------------------");
    printf("%s%f\n", "Interest : ", data_ptr->outputs.interest);
    printf("%s%f\n", "Ammount  : ", data_ptr->outputs.ammount);
    printf("%s\n" , "--------------------------------------------------------");
}

int main(){
    ProgramData data;
    
    ProgramData_readInputs(&data);
    ProgramData_compute(&data);
    ProgramData_displayOutputs(&data);
    
    return 0;
}