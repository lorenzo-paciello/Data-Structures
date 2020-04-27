//Lorenzo Paciello

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 16

typedef struct //registro com os dados do aluno
{
	int tipoChave;
	char nome[TAM];
	char sobrenome[TAM];
	char curso[TAM];
	float nota;
}tipoAluno;


void insereRegistro(tipoAluno *a, FILE *f, char *buffer)
{
	char size;

	fseek(f, 0, SEEK_END); //posiciona ao final do arquivo para inserir novo registro
	//Lendo os dados digitados pelo usuario
	printf("NUSP: ");
	scanf(" %d", &a->tipoChave);
	printf("Nome: ");
	scanf(" %[^\n]", a->nome);
	printf("Sobrenome: ");
	scanf(" %[^\n]", a->sobrenome);
	printf("Curso: ");
	scanf(" %[^\n]", a->curso);
	printf("Nota: ");
	scanf(" %f", &a->nota);
	printf("\n");
	//atribui o tamanho do registro a variavel 'size' com os delimitadores de campo
	size = sprintf(buffer, "%d|%s|%s|%s|%f|", a->tipoChave, a->nome, a->sobrenome, a->curso, a->nota);
	fwrite(&size, sizeof(size), 1, f);
	fwrite(buffer, size, 1, f);

}

char *parser(char *buffer, int *pos) //funcao que le o campo ate o delimitador '|' e retorna a posicao inicial do mesmo
{
	int pos_aux = *pos;

	while(buffer[*pos] != '|')
	{
		(*pos)++;
	}

	buffer[*pos] = '\0';
	(*pos)++;

	return &buffer[pos_aux];
}

void imprimeRegistro(tipoAluno *a)
{
	printf("nUSP: %d\nNome: %s\nSobrenome: %s\nCurso: %s\nNota: %.2f\n\n", 
	a->tipoChave, a->nome, a->sobrenome, a->curso, a->nota); //funcao usada dentro da funcao 'pesquisaRegistro'
}

int pesquisaRegistro(tipoAluno *a, FILE *f, char *buffer)
{
	int nusp, pos;
	char size;

	printf("Digite o nUSP do aluno: ");
	scanf("%d", &nusp); //le o nUSP digitado pelo usuario

	fseek(f, 0, SEEK_SET); //posiciona o ponteiro no comeco do arquivo

	while(fread(&size, sizeof(size), 1, f))
	{
		pos = 0;
		fread(buffer, size, 1, f);
		sscanf(parser(buffer, &pos), "%d", &a->tipoChave);
		if(a->tipoChave == nusp) //encontrou o nUSP desejado
		{
			printf("Registro encontrado!!\n\n");
			strcpy(a->nome, parser(buffer, &pos));
			strcpy(a->sobrenome, parser(buffer, &pos));
			strcpy(a->curso, parser(buffer, &pos));
			sscanf(parser(buffer, &pos), "%f", &a->nota);
			imprimeRegistro(a);
			return 1;

		}
	}
	printf("Registro nao encontrado!\n\n");
	return 0;
}

int main(int argc, char const *argv[])
{
	FILE *f;
	int comando = 0, n;
	tipoAluno a;
	char buffer[1000];

	f = fopen("alunos.dad", "w+");

	do
	{
		printf("Operacoes:\n1 - Gravar\n2 - Pesquisar\n3 - Finalizar\n\n");
		scanf("%d", &comando);

		switch(comando)
		{
			case 1:
				insereRegistro(&a, f, buffer);
				break;

			case 2:
				pesquisaRegistro(&a, f, buffer);
				break;

			case 3:
				printf("Finalizando!\n\n");
				break;

			default:
			printf("Opcao invalida!\n\n");
			
			
		}

	}while(comando != 3);

	fclose(f);
	
	return 0;
}
