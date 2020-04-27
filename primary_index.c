#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 16

typedef struct //dados do aluno
{
	char nusp[9];
	char nome[TAM];
	char sobrenome[TAM];
	char curso[TAM];
	float nota;
}tipoAluno;

typedef struct //index
{
	char nusp[9]; //chave do index
	int rrn; //posicao no arquivo
}indexR;

typedef struct
{
	int *vet; //vetor para guardar o inicio das posicoes vazias
	int header; //posicao do vetor q encontra-se vazia
}pilhaRRN;

void iniciaPilha(pilhaRRN *p)
{
	p->header = -1; //iniciando o header
	p->vet = malloc(sizeof(int)); //alocacao do vetor
}

void empilhaRRN(pilhaRRN *p, int byte) //funcao para realizar o empilhamento, ou seja,
{									   //quando um elemento eh retirado
	p->header++;
	p->vet = realloc(p->vet, (p->header+1) * sizeof(int));
	p->vet[p->header] = byte; //o vetor de espacos recebe a posicao no
}							  //no arquivo que foi removida

void desempilhaRRN(pilhaRRN *p)
{
	p->header--; //remocao do header, ou seja, quando um elemento eh inserido
				 //em uma posicao que anteriormente estava disponivel
	p->vet = realloc(p->vet, (p->header+1) * sizeof(int));
}

int isEmpty(pilhaRRN *p)
{
	if(p->header == -1)
	{
		return 1;
	}
	
	return 0;
}


//funcao que adiciona a chave e o rrn no index
void indexReg(indexR *ind, int *total, char *nusp, int *aux)
{
	
	strcpy(ind[*total].nusp, nusp);
	ind[*total].rrn = (*aux);
}


//funcao que ordena o index de acordo com o rrn
void ordenaIndex(indexR *ind, int *total)
{
	int i, j;
	indexR aux;

	for(i = 1; i < (*total)+1; i++)
	{
		for(j = 0; j < (*total); j++)
		{
			if(ind[j].rrn > ind[j+1].rrn)
			{
				aux = ind[j];
				ind[j] = ind[j+1];
				ind[j+1] = aux;
			}
		}
	}


}

//funcao que remove o index, movendo-o para o final e decrementando o tamanho do vetor
void removeIndex(indexR *ind, int *total, int *aux)
{
	int i, j;
	indexR indAux;

	for(i = 0; i < (*total); i++)
	{
		for(j = 0; j < (*total); j++){
		if(ind[j].rrn == (*aux))
		{
			indAux = ind[j];
			ind[j] = ind[j+1];
			ind[j+1] = indAux;
		}
		}
	}

}

//funcao que insere um registro no arquivo
void insereReg(FILE *f, indexR *ind, int *total, pilhaRRN *p)
{
	tipoAluno a;
	int aux; //auxiliar para armazenar a posicao no arquivo

		//entrada de dados pelo usuario
		printf("NUSP: ");
		scanf(" %s", a.nusp);
		printf("NOME: ");
		scanf(" %[^\n]", a.nome);
		printf("SOBRENOME: ");
		scanf(" %[^\n]", a.sobrenome);
		printf("CURSO: ");
		scanf(" %[^\n]", a.curso);
		printf("NOTA: ");
		scanf(" %f", &a.nota);

		if(isEmpty(p) == 1)//verifica se a pilha de disponiveis esta vazia
		{
			fseek(f, 0, SEEK_END); //posiciona o ponteiro para o final do arquivo
		}
		else
		{
			fseek(f, p->vet[p->header], SEEK_SET); //posiciona o ponteiro para uma 
												   //posicao disponivel na pilha
			desempilhaRRN(p); //retira uma posicao da pilha
		}
		aux = ftell(f); //armazena a posicao no arquivo
		fwrite(&a, sizeof(tipoAluno), 1, f);
		indexReg(ind, total, a.nusp, &aux); //atualiza o index
		ordenaIndex(ind, total); //ordena o index
}

//funcao para mostrar os dados no registro
void mostraReg(tipoAluno *a)
{
	printf("NUSP: %s\nNome: %s\nSobrenome: %s\nCurso: %s\nNota: %.2f\n\n", a->nusp, a->nome, a->sobrenome, a->curso, a->nota);
}


//funcao para remover um registro no arquivo e no indice
int removeReg(FILE *f, indexR *ind, int *total, pilhaRRN *p)
{
	tipoAluno a;
	int i, aux, index_num, nusp;

	fseek(f, 0, SEEK_SET); //posiciona o ponteiro no comeco do arquivo

	printf("NUSP a ser removido: ");
	scanf("%d", &nusp); //chave a ser procurada pelo usuario

	for(i = 0; i < (*total); i++){
		index_num = atoi(ind[i].nusp); //converte a string em int
		if(index_num == nusp) //encontrou a chave no index
		{
			fseek(f, ind[i].rrn, SEEK_SET); //posiciona o ponteiro para a posicao em que se 
											//encontra o registro no arquivo
			empilhaRRN(p, ind[i].rrn); //atualiza a pilha de posicoes disponiveis
			aux = ftell(f); //armazena a posicao atual
			fread(&a, sizeof(tipoAluno), 1, f); //le o registro do arquivo
			a.nusp[0] = '*'; //adiciona o caracter '*' para simbolizar uma remocao
			fseek(f, aux, SEEK_SET); //posiciona para o inicio do registro
			fwrite(&a, sizeof(tipoAluno), 1, f);
			removeIndex(ind, total, &aux); //remove o index do registro
			printf("REGISTRO REMOVIDO!!\n");
			(*total) -= 1; //decrementa o total de registros no index
			return 0;
		}
	}

	printf("REGISTRO NAO ENCONTRADO\n");
}

int procuraReg(FILE *f, indexR *ind, int *total)
{
	tipoAluno a;
	int i, index_num, nusp;
	printf("NUSP: ");
	scanf("%d", &nusp); //entrada de chave

	fseek(f, 0, SEEK_SET); //posiciona o ponteiro para o comeco do arquivo


	for(i = 0; i < (*total); i++)
	{
		index_num = atoi(ind[i].nusp); //conversao para int
		if(index_num == nusp)
		{
			printf("REGISTRO ENCONTRADO!!\n");
			fseek(f, ind[i].rrn, SEEK_SET); //move o ponteiro para a 
											//posicao do registro no arquivo
			fread(&a, sizeof(tipoAluno), 1, f);
			mostraReg(&a);
			return 0;
		}
	}
	printf("REGISTRO NAO ENCONTRADO!!\n");
}


int main()
{
	FILE *f;
	char op;
	int total = 0; //total de registros
	tipoAluno a;
	indexR *ind = NULL;
	pilhaRRN p;

	iniciaPilha(&p); //inicia a pilha de espacos disponiveis

	f = fopen("alunos.dad", "w+");

	do{

		printf("Operacoes:\n1 - Gravar\n2 - Pesquisar\n3 - Remover\n4 - Finalizar\n\n");

		scanf(" %c", &op);

		switch(op)
		{
			case '1':
				ind = realloc(ind, (total+2) * sizeof(indexR)); //realoca o index
				insereReg(f, ind, &total, &p);
				total++;
				break;

			case '2':
				procuraReg(f, ind, &total);
				break;

			case '3':
				removeReg(f, ind, &total, &p);
				ind = realloc(ind, (total+2) * sizeof(indexR));
				break;
		}
	}while(op != '4');

	printf("FINALIZANDO!!\n");

	fclose(f);
	
	
	return 0;
}
