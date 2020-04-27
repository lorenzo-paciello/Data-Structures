//Lorenzo Paciello

#include <stdio.h>
#include <stdlib.h>

#define TAM 16

typedef struct //registro com os dados do aluno
{
	int tipoChave;
	char nome[TAM];
	char sobrenome[TAM];
	char curso[TAM];
	float nota;
}tipoAluno;

void insereRegistro(tipoAluno *a, FILE *f)
{
	fseek(f, 0, SEEK_END); //posiciona ao final do arquivo para inserir novo registro
	printf("NUSP: ");
	scanf(" %d", &a->tipoChave);
	printf("Nome: ");
	scanf(" %s", a->nome);
	printf("Sobrenome: ");
	scanf(" %s", a->sobrenome);
	printf("Curso: ");
	scanf(" %s", a->curso);
	printf("Nota: ");
	scanf(" %f", &a->nota);
	fprintf(f, "%d %s %s %s %.2f\n", a->tipoChave, a->nome, a->sobrenome, a->curso, a->nota);
}

void imprimeRegistro(tipoAluno *a)
{
	printf("nUSP: %d\nNome: %s\nSobrenome: %s\nCurso: %s\nNota: %.2f\n\n", 
	a->tipoChave, a->nome, a->sobrenome, a->curso, a->nota); //funcao usada dentro da funcao 'pesquisaRegistro'
}

int pesquisaRegistro(tipoAluno *a, FILE *f)
{
	int nusp;

	printf("Digite o nUSP do aluno: ");
	scanf("%d", &nusp); //le o nUSP digitado pelo usuario

	fseek(f, 0, SEEK_SET); //posiciona no comeco do arquivo

	while(!feof(f))
	{
		fscanf(f, "%d %s %s %s %f\n", &a->tipoChave, a->nome, a->sobrenome, a->curso, &a->nota);
		if(a->tipoChave == nusp){ //encontrou o nUSP desejado
		printf("Registro encontrado!\n");
		imprimeRegistro(a);
		return 0;
		}
	}
	printf("Registro nao encontrado!\n\n");
	return 1;
}

int main(int argc, char const *argv[])
{
	FILE *f;
	int comando = 0;
	tipoAluno a;

	f = fopen("alunos.dad", "w+");

	do
	{
		printf("Operacoes:\n1 - Gravar\n2 - Pesquisar\n3 - Finalizar\n\n");
		scanf("%d", &comando);

		switch(comando)
		{
			case 1:
				insereRegistro(&a, f);
				break;

			case 2:
				pesquisaRegistro(&a, f);
				break;
		}

	}while(comando != 3);

	printf("Saindo!\n");

	fclose(f);
	
	return 0;
}
