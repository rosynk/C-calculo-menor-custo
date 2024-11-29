#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main() {
    int num_origens, num_destinos;
    int *demanda, *produtos;
    float **custos;
    float custo_total = 0;

    FILE *arquivo;

    //===================================================
    // INSERÇÃO DE DADOS PELO USUARIO
    //===================================================

    printf("Digite o número de origens/armazém: ");
    scanf("%d", &num_origens);
    printf("Digite o número de destinos: ");
    scanf("%d", &num_destinos);

    // Alocação dinamica
    produtos = (int *)malloc(num_origens * sizeof(int));
    demanda = (int *)malloc(num_destinos * sizeof(int));
    custos = (float **)malloc(num_origens * sizeof(float *));
    for (int i = 0; i < num_origens; i++) {
        custos[i] = (float *)malloc(num_destinos * sizeof(float));
    }

    if (produtos == NULL || demanda == NULL || custos == NULL) {
        printf("Erro ao alocar memória!\n");
        return 1;
    }

    // Entrada dos custos de transporte
    for (int i = 0; i < num_origens; i++) {
        for (int j = 0; j < num_destinos; j++) {
            printf("Digite o custo de transporte do armazém %d para o mercado %d: ", i + 1, j + 1);
            scanf("%f", &custos[i][j]);
        }
    }

    // Entrada de dados para produtos
    for (int i = 0; i < num_origens; i++) {
        printf("Digite a quantidade de produtos em oferta que o armazém %d tem: ", i + 1);
        scanf("%d", &produtos[i]);
    }

    // Entrada de dados para demanda
    for (int i = 0; i < num_destinos; i++) {
        printf("Digite a quantidade de demanda do destino %d: ", i + 1);
        scanf("%d", &demanda[i]);
    }

    //===================================================
    // ABERTURA DO ARQUIVO PARA GRAVAÇÃO
    //===================================================
    arquivo = fopen("resultado_transporte.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para gravação!\n");
        return 1;
    }

    fprintf(arquivo, "Relatório de Transporte:\n\n");
    fprintf(arquivo, "Nota: O custo de transporte de uma dada origem para um dado destino é dado pela ");
    fprintf(arquivo, "multiplicação da quantidade transportada pelo respectivo custo de transporte unitário.\n\n");

    //===================================================
    // CÁLCULO DO MENOR CUSTO TOTAL DE TRANSPORTE
    //===================================================
    bool transporte_possivel = true;

    while (transporte_possivel) {
        transporte_possivel = false;  // Assume que não tem mais transporte possível

        int origem_selecionada = -1;
        int destino_selecionado = -1;
        float menor_custo = __FLT_MAX__; // Inicializa com um valor muito grande

        // Encontrar a combinação com o menor custo unitario
        for (int i = 0; i < num_origens; i++) {
            if (produtos[i] > 0) { // Origem tem produtos disponiveis
                for (int j = 0; j < num_destinos; j++) {
                    if (demanda[j] > 0 && custos[i][j] < menor_custo) { // Destino ainda precisa e o custo é menor
                        origem_selecionada = i;
                        destino_selecionado = j;
                        menor_custo = custos[i][j];
                    }
                }
            }
        }

        // Verificar se encontramos uma combinação valida
        if (origem_selecionada != -1 && destino_selecionado != -1) {
            // Calcular a quantidade de transporte
            int quantidade_transportada = (produtos[origem_selecionada] < demanda[destino_selecionado]) ? 
                                           produtos[origem_selecionada] : demanda[destino_selecionado];
            float custo_transporte = quantidade_transportada * custos[origem_selecionada][destino_selecionado];
            custo_total += custo_transporte;

            // Atualizar os dados
            produtos[origem_selecionada] -= quantidade_transportada;
            demanda[destino_selecionado] -= quantidade_transportada;

            // Registrar a movimentação
            fprintf(arquivo, "Armazém %d → Mercado %d: %d unidades transportadas, custo %.2f\n",
                    origem_selecionada + 1, destino_selecionado + 1, quantidade_transportada, custo_transporte);

            // Houve transporte
            transporte_possivel = true;
        }
    }

    // Verificar destinos não totalmente atendidos
    fprintf(arquivo, "\nDestinos não totalmente atendidos:\n");
    for (int j = 0; j < num_destinos; j++) {
        if (demanda[j] > 0) {
            fprintf(arquivo, "Destino %d: demanda restante de %d unidades\n", j + 1, demanda[j]);
        }
    }

    // Gravar custo total no arquivo
    fprintf(arquivo, "\nCusto total do transporte: %.2f\n", custo_total);

    printf("\nCálculo concluído. Confira o arquivo 'resultado_transporte.txt' para os detalhes.\n");

    //===================================================
    // LIBERAÇÃO DE MEMORIA
    //===================================================
    for (int i = 0; i < num_origens; i++) {
        free(custos[i]);
    }
    free(custos);
    free(produtos);
    free(demanda);

    fclose(arquivo);

    return 0;
}