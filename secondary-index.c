//Lorenzo Paciello

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM 16

typedef struct //dados do aluno
{
	char inserido;
	char nusp[9];
	char nome[TAM];
	char sobrenome[TAM];
	char curso[TAM];
	float nota;
}tipoAluno;

typedef struct //index primario
{
	char nusp[9]; //chave do index
	int rrn; //posicao no arquivo
}indexR;

//estrutura de lista
struct no
{
	char nusp[9];
	struct no *prox;
};

typedef struct no No;

typedef struct
{
	No *ini;
}Lista;

//index secundario
typedef struct
{
	char sobrenome[TAM];
	Lista *l; //ponteiro para lista encadeada
	int quant;
}indexS;


//funcao que adiciona a chave e o rrn no index
void indexReg(indexR *ind, int *total, char *nusp, int *aux)
{
	strcpy(ind[*total].nusp, nusp);
	ind[*total].rrn = (*aux);
}


//funcao que ordena o index de acordo com o NUSP
void ordenaIndex(indexR *ind, int *totalIndP)
{
	int i, j;
	indexR aux;

	for(i = 0; i < (*totalIndP); i++)
	{
		for(j = 0; j < (*totalIndP); j++)
		{
			if(strcmp(ind[j].nusp, ind[j+1].nusp) > 0)
			{
				aux = ind[j];
				ind[j] = ind[j+1];
				ind[j+1] = aux;
			}
		}
	}


}

//funcao que remove o index, movendo-o para o final e decrementando o tamanho do vetor
int removeIndex(indexR *ind, int *totalIndP, int *aux)
{
	int i, j;
	indexR indAux;

	for(i = 0; i < (*totalIndP); i++)
	{
		if(ind[i].rrn == (*aux)){ 
		for(j = i; j < (*totalIndP)-1; j++){
			indAux = ind[j];
			ind[j] = ind[j+1];
			ind[j+1] = indAux;
		}
		return 1;
		}
	}

}

//funcao que insere um registro no arquivo
void insereReg(FILE *f, indexR *ind, int *totalReg, int *totalIndP)
{
	fseek(f, 0, SEEK_END); //insere o registro no final do arquivo
	tipoAluno a;
	int aux; //auxiliar para armazenar a posicao no arquivo

		//entrada de dados pelo usuario
		a.inserido = '0'; //valor zero para indica que o registro nao foi inserido no index secundario
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

		aux = ftell(f); //armazena a posicao antes da insercao
		fwrite(&a, sizeof(tipoAluno), 1, f);
		indexReg(ind, totalIndP, a.nusp, &aux); //atualiza o index
		ordenaIndex(ind, totalIndP); //ordena o index
}

//funcao para mostrar os dados no registro
void mostraReg(tipoAluno *a)
{
	printf("INSERIDO: %c\nNUSP: %s\nNome: %s\nSobrenome: %s\nCurso: %s\nNota: %.2f\n\n", a->inserido, a->nusp, a->nome, a->sobrenome, a->curso, a->nota);
}


int procuraReg(FILE *f, indexR *ind, int *totalIndP, char *nusp)
{
	tipoAluno a;
	int i, index_num;

	fseek(f, 0, SEEK_SET); //posiciona o ponteiro para o comeco do arquivo


	for(i = 0; i < (*totalIndP); i++)
	{
		if(strcmp(ind[i].nusp, nusp) == 0)
		{
			printf("REGISTRO ENCONTRADO!!\n");
			fseek(f, ind[i].rrn, SEEK_SET); //move o ponteiro para a 
											//posicao do registro no arquivo
			fread(&a, sizeof(tipoAluno), 1, f); //le o registro no arquivo
			mostraReg(&a); //mostra os dados
			return 0;
		}
	}
	printf("REGISTRO NAO ENCONTRADO!!\n"); //se nao encontrar o NUSP no loop o resgistro nao existe no arquivo
}

//inicialiaza a lista encadeada presente no index secundario
Lista *cria_lista()
{
	Lista *l;

	l = malloc(sizeof(Lista));

	l->ini = NULL;

	return l;
}

//funcao de busca interna para inserir o elemento na posicao correta
void buscaInterna(Lista *l, No **atual, No **ant, char *nusp)
{
	*atual = l->ini; //recebe o comeco da lista do index em questao
	*ant = NULL;

	while((*atual) != NULL && strcmp((*atual)->nusp, nusp) < 0) //enquanto o nusp da posicao indicada for menor q o inserido
	{
		*ant = *atual; //'ant' armazena a posicao atual
		*atual = (*atual)->prox; //'atual' se move para o proximo da lista
	}

}

//funcao que insere um elemento na lista do index secundario
void insereElem(Lista *l, char *nusp)
{
	No *atual, *ant, *novo;

	buscaInterna(l, &atual, &ant, nusp);

	novo = malloc(sizeof(No)); //aloca espaco para o novo elemento

	strcpy(novo->nusp, nusp); //copia o NUSP do elemento
	novo->prox = atual;

	//lista vazia
	if(l->ini == NULL)
	{
		l->ini = novo;
	}
	else if(ant == NULL) //menor q o primeiro da lista
	{
		ant = novo;
		novo->prox = l->ini;
		l->ini = novo;
	}
	else
	{
		ant->prox = novo;
	}
}

//funcao que muda os caracteres do sobrenome para maiusculo afim de facilitar o manuseio
char *strUpperCase(char *str)
{
	char *auxStr = NULL;
	for(int i = 0; i < strlen(str); i++)
	{
		auxStr = realloc(auxStr, i+1 * sizeof(char));
		auxStr[i] = toupper(str[i]);
	}
	
	return auxStr;
}

//funcao que verifica se o sobrenome inserido possui semelhancas com o sobrenome ja inserido na lista utilizando a funcao 'strstr'
int verificaSobrenome(char *indSobrenome, char *fSobrenome)
{
	char *auxInd = malloc(TAM * sizeof(char));
	
	if(strlen(fSobrenome) > strlen(indSobrenome))
	{
	 	auxInd = strstr(strUpperCase(fSobrenome), strUpperCase(indSobrenome));
	}
	else
	{
		auxInd = strstr(strUpperCase(indSobrenome), strUpperCase(fSobrenome));
	}

	if(auxInd != NULL)
	{	
		return 1;
	}

	return 0;

}

//funcao para imprimir a lista do index secundario
void imprimeLista(Lista *l)
{
	No *aux = malloc(sizeof(No));
	aux = l->ini;

	while(aux != NULL)
	{
		printf("%s\n", aux->nusp);
		aux = aux->prox;
	}	
}

//funcao que, para cada sobrenome, imprime a lista de NUSP's
void imprimeSec(indexS *indS, int *totalIndS)
{
	for(int i = 0; i < (*totalIndS); i++)
	{
		printf("%s\n", indS[i].sobrenome);
		imprimeLista(indS[i].l);
	}

}

//fincao que remove o sobrenome do index secundario
int removeIndexSec(indexS *indS, int *totalIndS, char *sobrenome)
{
	for(int i = 0; i < (*totalIndS); i++)
	{
		if(verificaSobrenome(indS[i].sobrenome, sobrenome))
		{
			for(int j = i; j < (*totalIndS)-1; j++)
			{
				indexS auxInd = indS[j];
				indS[j] = indS[j+1];
				indS[j+1] = auxInd;
			}
			*totalIndS -= 1;
			return 1;
		}
	}
}

void destroi_lista(Lista *l)
{
	No *aux;
	while(l->ini != NULL)
	{
		aux = l->ini;
		l->ini = l->ini->prox;
		free(aux);
	}

	l = NULL;
	free(l);

}

//funcao que remove um registro do index secundario a partir do NUSP
int removeRegSec(tipoAluno *a, indexS *indS, int *totalIndS)
{
	No *atual, *ant;

	for(int i = 0; i < (*totalIndS); i++)
	{
		if(verificaSobrenome(indS[i].sobrenome, a->sobrenome))
		{
			printf("SOBRENOME ENCONTRADO: %s\n", indS[i].sobrenome);
			buscaInterna(indS[i].l, &atual, &ant, a->nusp);
			if(ant == NULL) //elemento procurado eh o primeiro da lista
			{
				indS[i].l->ini = atual->prox;
			}
			else
			{
				ant->prox = atual->prox;
			}
			free(atual);
			indS[i].quant -= 1; //decrementa a quantidade de registros

			if(indS[i].quant == 0) //se a lista se encontrar vazia apos a remocao a chave eh retirada
			{
				destroi_lista(indS[i].l);
				removeIndexSec(indS, totalIndS, a->sobrenome);
				indS = realloc(indS, (*totalIndS + 2) * sizeof(indexS));
			}
			return 1;
		}
	}

}

//funcao para remover um registro no arquivo e no indice
int removeReg(FILE *f, indexR *ind, indexS *indS, int *totalReg, int *totalIndP, int *totalIndS)
{
	tipoAluno a;
	int i, aux;
	char *nusp = malloc(9 * sizeof(char));

	fseek(f, 0, SEEK_SET); //posiciona o ponteiro no comeco do arquivo

	printf("NUSP a ser removido: ");
	scanf("%s", nusp); //chave a ser procurada pelo usuario

	for(i = 0; i < (*totalIndP); i++){
		if(strcmp(ind[i].nusp, nusp) == 0) //encontrou a chave no index
		{
			fseek(f, ind[i].rrn, SEEK_SET); //posiciona o ponteiro para a posicao em que se 
											//encontra o registro no arquivo
			aux = ftell(f); //armazena a posicao atual
			fread(&a, sizeof(tipoAluno), 1, f); //le o registro do arquivo
			removeRegSec(&a, indS, totalIndS); //procura e remove o registro do index secundario
			a.nusp[0] = '*'; //adiciona o caracter '*' para indicar uma remocao
			fseek(f, aux, SEEK_SET); //posiciona para o inicio do registro
			fwrite(&a, sizeof(tipoAluno), 1, f);
			removeIndex(ind, totalIndP, &aux); //remove no index primario o registro
			printf("REGISTRO REMOVIDO!!\n");
			(*totalReg) -= 1; //decrementa o total de registros do arquivo
			(*totalIndP) -= 1; //decrementa o total de registros no index primario
			return 0;
		}
	}

	printf("REGISTRO NAO ENCONTRADO\n");
}

//funcao de busca o registro dentro da lista do index secundario em questao
void buscaListaIndSec(Lista *l, indexR *indP, int *totalIndP, FILE *f)
{
	No *aux = malloc(sizeof(No));
	aux = l->ini;

	while(aux != NULL)
	{
		procuraReg(f, indP, totalIndP, aux->nusp);
		aux = aux->prox;
	}

}

//funcao para buscar o sobrenome no index secundario para entao realizar a busca pelo NUSP
void buscarIndexSec(FILE *f, indexR *indP, indexS *indS, int *totalIndS, int *totalIndP, char *str)
{
	int flag = 0; //indica se o registro foi encontrado ou nap
	for(int i = 0; i < (*totalIndS); i++)
	{
		if(verificaSobrenome(indS[i].sobrenome, str))
		{
			flag = 1;
			printf("SOBRENOME ENCONTRADO: %s\n", indS[i].sobrenome);
			buscaListaIndSec(indS[i].l, indP, totalIndP, f);
		}
	}
	if(flag == 0)
	{
		printf("SOBRENOME NAO ENCONTRADO!!\n");
	}
}

//funcao que ordena o index secundario a partir do sobrenome
void ordenaIndexSec(indexS *indS, int *totalIndS)
{
	for(int i = 1; i < (*totalIndS)+1; i++)
	{
		for(int j = 0; j < (*totalIndS)-1; j++)
		{
			char *aux1 = malloc(TAM * sizeof(char)), *aux2 = malloc(TAM * sizeof(char));
				aux1 = strUpperCase(indS[j].sobrenome);
				aux2 = strUpperCase(indS[j+1].sobrenome);
			if(strcmp(aux1, aux2) > 0)
			{
				indexS auxIndex;
				auxIndex = indS[j];
				indS[j] = indS[j+1];
				indS[j+1] = auxIndex;
			}
		}
	}

}

void printIndP(indexR *indP, int *totalIndP)
{
	for(int i = 0; i < (*totalIndP); i++)
	{
		printf("Nusp: %s\tRRN: %d\n", indP[i].nusp, indP[i].rrn);
	}

}

//funcao que insere os dados no index secundario
void insere_sec(indexS *indS, indexR *indP, int *totalIndS, int *totalIndP, FILE *f)
{
	tipoAluno a;
	int flag = 0, auxFseek;

	//fseek(f, 0, SEEK_SET);
	
	for(int i = 0; i < (*totalIndP); i++)
	{
		fseek(f, indP[i].rrn, SEEK_SET); //posiciona o ponteiro na posicao 'i' do index primario no arquivo
		auxFseek = ftell(f); //armazena a posicao do registro 
		fread(&a, sizeof(tipoAluno), 1, f); //le o registro e armazena na variavel 'a'

	//verifica se o registro ja foi inserido no index secundario, no caso, '0' para 'false' e '1' para 'true'
	if(a.inserido == '0')
	{
	
	for(int j = 0; j < (*totalIndS); j++) //loop de acordo com o total de chaves secundarias
	{
		if(verificaSobrenome(indS[j].sobrenome, a.sobrenome))
		{
			flag = 1; //indica que o registo possui semelhanca no sobrenome
			insereElem(indS[j].l, a.nusp); //insele o NUSP do registro na lista de tal index secundario
			a.inserido = '1'; //altera o valor de '0' para '1'
			fseek(f, auxFseek, SEEK_SET); //retorna a posicao do registro no arquivo
			fwrite(&a, sizeof(tipoAluno), 1, f); //reescreve o registro com o valor de inserido igual a '1'
			indS[j].quant += 1; //aumenta o valor total de NUSP naquela chave 

		}
	}
	if(flag == 0) //o registro em questao nao possui semelhanca com nenhum, portanto eh criado uma nova posicao no index secundario
	{
		strcpy(indS[*totalIndS].sobrenome, a.sobrenome); //copia a chave secundaria
		indS[*totalIndS].l = cria_lista(); //cria a lista encadeada da posicao em questao, que nesse caso eh a ultima
		insereElem(indS[*totalIndS].l, a.nusp); //insere o NUSP na lista de tal posicao
		a.inserido = '1'; //muda o valor de inserido para '1'
		fseek(f, auxFseek, SEEK_SET);
		fwrite(&a, sizeof(tipoAluno), 1, f); //reescreve o registro
		indS[*totalIndS].quant = 1; //primeira chave inserida na posicao do index
		*totalIndS += 1;
	}
	}
	}
	ordenaIndexSec(indS, totalIndS);
}

//funcao que carrega o index primario a partir de um arquivo ja existente
indexR *carregaIndexP(FILE *fp, int *totalIndP)
{
	indexR *auxInd = malloc(sizeof(indexR));

	while(fread(&auxInd[*totalIndP], sizeof(indexR), 1, fp))
	{
		auxInd = realloc(auxInd, (*totalIndP + 2) * sizeof(indexR));
		*totalIndP += 1;
	}

	return auxInd;

}

//funcao que carrega o index secundario a partir de um arquivo preexistente
indexS *carregaIndexSec(int *totalIndS, FILE *fs)
{
	//aloca memoria para as variaveis que auxiliarao no carregamento do index
	indexS *auxInd = malloc(sizeof(indexS));
	char *auxNusp = malloc(9 * sizeof(char));
	int *auxTAM = malloc(sizeof(int));

	//le a quantidade de sobrenomes inseridos posteriormente
	fread(auxTAM, sizeof(int), 1, fs);

	for(int i = 0; i < (*auxTAM); i++)
	{
		fread(&auxInd[i].quant, sizeof(int), 1, fs); //quantidade de NUSP naquela chave
		fread(&auxInd[i].sobrenome, TAM * sizeof(char), 1, fs); //le a chave secundaria
		auxInd[i].l = cria_lista(); //cria a lista daquela posicao para armazenar os dados de tal chave
		for(int j = 0; j < auxInd[i].quant; j++)
		{
			fread(auxNusp, 9 * sizeof(char), 1, fs); //le o NUSP da chave na posicao 'j'
			insereElem(auxInd[i].l, auxNusp); //insere o elemento na lista de tal posicao
		}
		*totalIndS += 1; //aumenta o valor total de chaves secundarias
		auxInd = realloc(auxInd, ((*totalIndS) + 2) * sizeof(indexS));
	}

	return auxInd;

}

//funcao que atualiza o arquivo de index primario 
void saveIndexPFile(FILE *fp, indexR *indP, int *totalIndP)
{
	fseek(fp, 0, SEEK_SET);
	fwrite(indP, sizeof(indexR), (*totalIndP), fp);

	fclose(fp); //fechando o arquivo
	free(indP);
}

//funcao que atualiza o arquivo de index secundario
void saveIndexSFILE(FILE *fs, int *totalIndS, indexS *indS)
{
	fseek(fs, 0, SEEK_SET);
	fwrite(totalIndS, sizeof(int), 1, fs); //total de chaves
	for(int i = 0; i < (*totalIndS); i++)
	{
		fwrite(&indS[i].quant, sizeof(int), 1, fs); //quantidade de NUSP de tal chave
		fwrite(&indS[i].sobrenome, TAM * sizeof(char), 1, fs); //a propria chave

		No *aux = indS[i].l->ini;
		while(aux != NULL)
		{
			fwrite(aux->nusp, 9 * sizeof(char), 1, fs); //armazena no arquivo os NUSP's de tal chave
			aux = aux->prox;
		}
	}

	//fecha o arquivo
	fclose(fs);
	free(indS);
}

int main()
{
	FILE *f, *fp, *fs;
	char op;
	int totalReg = 0, totalIndP = 0, totalIndS = 0; //variaveis auxiliares
	indexR *indP; //index primario
	indexS *indS; //index secundario

	indP = malloc(sizeof(indexR));
	indS = malloc(sizeof(indexS));

	//abertura dos arquivos verifica se o arquivo ja existe. Se sim, o arquivo eh criado,
	//senao os index primario e secundario sao carregados a partir dos arquivos preexistentes
	f = fopen("alunos.dat", "r+");
	if(f == NULL)
	{
		f = fopen("alunos.dat", "w+");
	}

	fp = fopen("index_prim.dat", "r+");
	if(fp == NULL)
	{
		fp = fopen("index_prim.dat", "w+");
	}
	else
	{
		indP = carregaIndexP(fp, &totalIndP);
	}

	fs = fopen("index_sec.dat", "r+");
	if(fs == NULL)
	{
		fs = fopen("index_sec.dat", "w+");
		
	}
	else
	{
		indS = carregaIndexSec(&totalIndS, fs);
	}
	

	do{

		//menu de opcoes
		printf("Operacoes:\n1 - Gravar\n2 - Pesquisar por NUSP\n3 - Pesquisar por sobrenome\n4 - Remover por NUSP\n5 - Finalizar\n\n");

		scanf(" %c", &op);

		switch(op)
		{
			case '1':
				insereReg(f, indP, &totalReg, &totalIndP);
				indP = realloc(indP, (totalIndP+2) * sizeof(indexR)); //realoca o index
				totalReg++;
				totalIndP++;
				insere_sec(indS, indP, &totalIndS, &totalIndP, f);
				indS = realloc(indS, (totalIndS+2) * sizeof(indexS));
				break;

			case '2':
				printf("NUSP: ");
				char *nusp = malloc(9 * sizeof(char));
				scanf("%s", nusp);
				procuraReg(f, indP, &totalIndP, nusp);
				break;

			case '3':
				printf("SOBRENOME: ");
				char *sobrenome = malloc(TAM * sizeof(char));
				scanf(" %[^\n]", sobrenome);
				buscarIndexSec(f, indP, indS, &totalIndS, &totalIndP, sobrenome);
				break;

			case '4':
				removeReg(f, indP, indS, &totalReg, &totalIndP, &totalIndS);
				indP = realloc(indP, (totalIndP+2) * sizeof(indexR));
				break;
		}
	}while(op != '5');

	printf("FINALIZANDO!!\n");

	//salvando as informacoes nos arquivos de indices e os fechando
	saveIndexPFile(fp, indP, &totalIndP);
	saveIndexSFILE(fs, &totalIndS, indS);

	//fechando o arquivo de registro
	fclose(f);
	
	return 0;
}
