#include <conio2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define VALOR_MAXIMO 100
#define X 77
#define Y 22
#define DIRECAO 4
#define APARECE_PODER 30
#define SOME_PODER 15
#define TEMPO_PODER 10
#define QTD_PODER 6
#define ALCANCE_PULO 5
#define RAPIDO 0
#define LENTO 1

typedef struct
{
    int x;
    int y;
} COORDENADA;

typedef struct
{
    int direcao; // 0..2 para cada uma das direções
    int tamanho; // número de caracteres que ocupa
    int velocidade;
    COORDENADA posicao;
    int aparecendo;
    int espera;
} PLATAFORMA;

typedef struct
{
    char nome[51];
    int ch; // ASCII do caracter que representa o jogador
    int pontos;
    int poder;
    int vidas;
    int status; // 0 = jogando, 1 = morto, 2 = esperando
    int alcance;
    int extrapulo;  // 1 = Possivel 0 = Nao possivel
    int velocidade;
    int espera;
    COORDENADA posicao;
} JOGADOR;

typedef struct
{
    int aparecer_poder;
    int sumir_poder;
    int tempo_poder;
    int aparecendo;
    int secs;
    int tipo_poder;
    COORDENADA posicao_poder;
} CONTROLE_PODER;

typedef enum
{
    CIMA,       // 0
    DIREITA,    // 1
    ESQUERDA,   // 2
    BAIXO       // 3
} DIRECOES;

typedef enum
{
    NENHUM,
    LEVITA,
    SUPERPULO,
    INVENCIVEL,
    PARATEMPO,
    GRAVIDADE,
    VIDA,
    PONTOS
} PODER;

// MENU
void desenha_menu();
void menu(int *opt, JOGADOR *player);
int le_setas_menu();
void menu_fim(int *opt, JOGADOR *player);

// RECORDES
void mostra_recordes();
void mostra_recordes_fim(JOGADOR player);
void gera_recordes(JOGADOR jogadores[]);
void escreve_arquivo(FILE *arq, JOGADOR jogadores[]);
void ordena_recordes(JOGADOR vet[], int n);
void salva_recordes(JOGADOR player);
int acha_jogador_recordes(JOGADOR player);

// PLATAFORMAS
void desenha_plataforma(PLATAFORMA plataforma);
void movimenta_plataforma(PLATAFORMA plataforma[], int quantia);
int busca_num_de_plats();
void busca_plataformas(PLATAFORMA plats[]);
void gera_plat(PLATAFORMA plataforma[],int quantia);
int busca_direcao(int dir);

// JOGADOR
void inf_do_player(JOGADOR *player);
int em_cima(JOGADOR *player, PLATAFORMA plat[],int quantia,int *numplat);
void move_jogador (JOGADOR *player,int direcao);
void pula(JOGADOR *player);
void morreu(JOGADOR *player);
void parado(JOGADOR *player, clock_t *inicio);
void posiciona_jogador(JOGADOR *player);
void controla_movimento_jogador(JOGADOR *player, PLATAFORMA plataformas[], int num_de_plats, int *numplat);
void controla_pulo(JOGADOR *player, PLATAFORMA plataformas[], int num_de_plats, int *numplat);

// PODER
int sorteia_poder(COORDENADA *poder);
void mostra_poder(COORDENADA poder, int cor);
void some_poder(COORDENADA poder);
int busca_cor_poder(int tipo);
void controla_poder(JOGADOR *player, CONTROLE_PODER *controle_poder);
char* nome_poder(JOGADOR player);

// ÁREA DE JOGO
void atualiza_inf(JOGADOR player);
void desenha_bordas();

// VELOCIDADE
int atualiza_espera(JOGADOR *player, PLATAFORMA plataformas[ ], int quantia);

// JOGO
int le_setas();
void desenvolvimento(JOGADOR *player,int num_de_plat);

int main()
{
    int opt, num_plats;
    JOGADOR player;

    srand(time(0));

    menu(&opt,&player);
    while(opt)
    {
        num_plats = busca_num_de_plats();
        desenvolvimento(&player,num_plats);
        salva_recordes(player);
        mostra_recordes_fim(player);
        menu_fim(&opt, &player);
    }
    clrscr();
    return 0;
}

// --------------- MENU ----------------------
void desenha_menu()
{
    // Desenha as bordas do menu
    int i;
    for (i=10; i<=70; i++)
    {
        textbackground(MAGENTA);
        putchxy(i,5,' ');
    }
    for (i=5; i<=20; i++)
    {
        textbackground(MAGENTA);
        putchxy(70,i,' ');
    }
    for (i=70; i>=10; i--)
    {
        textbackground(MAGENTA);
        putchxy(i,20,' ');
    }
    for (i=20; i>=5; i--)
    {
        textbackground(MAGENTA);
        putchxy(10,i,' ');
    }
    textbackground(0);
}
void menu(int *opt, JOGADOR *player)
{
    // Apresenta as opções do menu inicial e trata as ações do usuário
    char cursor = '>';
    int tecla;
    COORDENADA curs;

    curs.x=34;
    curs.y=9;
    *opt=2;
    desenha_menu();
    do
    {
        gotoxy(35,9);
        puts("NOVO JOGO");
        gotoxy(35,12);
        puts("RECORDES");
        gotoxy(35,15);
        puts("SAIR");

        putchxy(curs.x,curs.y,cursor);
        if(kbhit())
        {
            tecla = le_setas_menu();                   // Armazena a tecla pressionada
            putchxy(curs.x,curs.y,' ');
            if (tecla==CIMA && curs.y-3 >=9)    // Move a seta do menu para cima
                curs.y-=3;
            else if(tecla==BAIXO && curs.y+3 <=15) // Move a seta do menu para baixo
                curs.y+=3;
            else if(tecla==5) // ESC - sair
                *opt=0;
            else if(tecla==4)   // Se pressionou ENTER
            {
                if(curs.y==9) // Se está em Novo jogo
                {
                    *opt=1;
                    clrscr();
                    desenha_menu();
                    inf_do_player(player);
                    clrscr();
                }
                else if(curs.y==12)  // Se está em Recordes
                {
                    clrscr();
                    desenha_menu();
                    mostra_recordes();
                    while(!kbhit())         //Fica em Recordes até alguma tecla ser pressionada
                    {
                        *opt=2;
                        Sleep(30);
                    }
                    clrscr();
                    desenha_menu();
                }
                else if(curs.y==15)  // Se está em Sair
                    *opt=0;
            }
        }
        putchxy(curs.x,curs.y,cursor);
        Sleep(20);
    }
    while(*opt==2);                     // Mantém no menu enquanto não fizer nada ou escolher Recordes
}
int le_setas_menu()
{
    int tecla;
    tecla = getch();
    if (tecla==72)
        return CIMA;
    if (tecla==80)
        return BAIXO;
    if(tecla==13)
        return 4; // ENTER
    if(tecla==27)
        return 5; // ESC
}
void menu_fim(int *opt, JOGADOR *player)
{
    // Apresenta as opções do menu final e trata as ações do usuário
    char cursor = '>';
    int tecla;
    COORDENADA curs;

    curs.x=34;
    curs.y=9;
    *opt = 2;
    fflush(stdin);
    do
    {
        desenha_menu();
        gotoxy(35,9);
        puts("JOGAR NOVAMENTE");
        gotoxy(35,12);
        puts("SAIR");
        putchxy(curs.x,curs.y,cursor);
        if(le_setas()!=6)
        {
            tecla = le_setas();                   // Armazena a tecla pressionada
            putchxy(curs.x,curs.y,' ');
            if (tecla==CIMA && curs.y-3 >=9)    // Move a seta do menu para cima
                curs.y-=3;
            else if(tecla==BAIXO && curs.y+3 <=12) // Move a seta do menu para baixo
                curs.y+=3;
            else if(tecla==5) // ESC - sair
                *opt=0;
            else if(tecla==4)   // Se pressionou ENTER
            {
                if(curs.y==9) // Se está em Novo jogo
                {
                    clrscr();
                    *opt=1;
                }
                else if(curs.y==12)  // Se está em Recordes
                {
                    *opt=0;
                }
            }
            putchxy(curs.x,curs.y,cursor);
        }
        Sleep(30);
    }
    while(*opt==2);                     // Mantém no menu enquanto não fizer nada ou escolher Recordes
}
// --------------- MENU ----------------------

// --------------- RECORDES ----------------------

void mostra_recordes()
{
    // Mostra os 5 melhores jogadores que estão salvos no arquivo binario
    FILE *arq;
    JOGADOR aux[5]; // Buffer pra leitura do arquivo
    int i;
    arq = fopen("recordes.bin","rb");
    if(!arq)
    {
        // Caso não exista o arquivo, cria um novo com recordes aleatorios
        gera_recordes(aux);
        ordena_recordes(aux,5);
        escreve_arquivo(arq, aux);
    }
    clrscr();
    desenha_menu();
    gotoxy(36,8);
    puts("OS MELHORES");
    for(i=0; i<5; i++)
    {
        fread(&aux[i],sizeof(JOGADOR),1,arq);
        gotoxy(30,11+i);
        printf("%15s%6d\n",aux[i].nome,aux[i].pontos);
    }
    fclose(arq);
}
void mostra_recordes_fim(JOGADOR player)
{
    int aux;
    // Mostra os 5 melhores jogadores que estão salvos no arquivo binario
    clrscr();
    desenha_menu();
    aux=acha_jogador_recordes(player);
    if(aux)
    {
        if(aux==2)
        {
            gotoxy(23,11);
            printf("Voce nao esta entre os 5 melhores");
            gotoxy(22,19);
            printf("Pressione SPACE/CIMA para continuar");
        }
        else
            mostra_recordes();
    }

    fflush(stdin);
    while(le_setas()!= CIMA)         //Fica em Recordes até alguma tecla ser pressionada
    {
        Sleep(30);
    }
    clrscr();
    fflush(stdin);
}
void gera_recordes(JOGADOR jogadores[])
{
    // Pede ao usuário cinco nomes e salva com uma pontuacao aleatoria e decrescente no arquivo dos recordes
    // {Salva recordes aleatorios no arquivo)
    int i;
    for(i=0; i<5; i++)
    {
        gotoxy(33,9);
        puts("DIGITE UM NOME ALEATORIO");
        gotoxy(33,10);
        gets(jogadores[i].nome);
        clrscr();
        desenha_menu();
    }
    for (i=0; i<5; i++)
    {
        jogadores[i].pontos = (int) (rand() / (double) RAND_MAX * (VALOR_MAXIMO + 1));
    }
}
void escreve_arquivo(FILE *arq, JOGADOR jogadores[])
{
    // Escreve os recordes no arquivo
    int i;

    arq = fopen("recordes.bin","w+b");
    if(!arq)
    {
        puts("Erro na abertura do arquivo de recordes");
    }
    for(i=0; i<5; i++)
    {
        if(!fwrite(&jogadores[i],sizeof(JOGADOR),1,arq))
        {
            puts("Erro na escrita dos recordes");
        }
    }
    fclose(arq);
}
void ordena_recordes(JOGADOR vet[], int n)
{
    //Ordena os recordes em ordem decrescente de pontuacao
    int ind, fim, sinal, aux;
    char aux2[50];
    fim = n-1; //indica até onde o vetor ainda não está ordenado
    do
    {
        sinal = 0; // indica se houve alguma troca nesta passagem
        for (ind=0; ind < fim; ind++)
            if (vet[ind].pontos < vet[ind+1].pontos) //troca 2 elementos
            {
                // COPIA OS NUMEROS
                aux = vet[ind].pontos;
                vet[ind].pontos = vet[ind+1].pontos;
                vet[ind+1].pontos = aux;
                // COPIA OS NOMES
                strcpy(aux2,vet[ind].nome);
                strcpy(vet[ind].nome,vet[ind+1].nome);
                strcpy(vet[ind+1].nome,aux2);
                sinal = 1;
            }
        fim --;
    }
    while (sinal==1 && fim >1);
}
void salva_recordes(JOGADOR player)
{
    int i;
    FILE *arq;
    JOGADOR aux[6];

    arq = fopen("recordes.bin","rb");
    if(!arq)
    {
        puts("Erro na abertura do arquivo de recordes");
    }
    else
    {
        for (i=0; i<5; i++)
        {
            fread(&aux[i],sizeof(JOGADOR),1,arq);
            if(ferror(arq))
                puts("Erro de leitura");
        }
        fclose(arq);

        aux[5] = player;
        ordena_recordes(aux,6);
        escreve_arquivo(arq, aux);
    }
}
int acha_jogador_recordes(JOGADOR player)
{
    int i = 0;
    JOGADOR aux[5];
    FILE *arq;

    arq = fopen ("recordes.bin","r+b");
    if(!arq)
    {
        // Caso não exista o arquivo, cria um novo com recordes aleatorios
        gera_recordes(aux);
        ordena_recordes(aux,5);
        escreve_arquivo(arq, aux);
    }

    arq = fopen ("recordes.bin","r+b");
    do
    {
        if(!fread(&aux[i],sizeof(JOGADOR),1,arq))
        {
            puts("Erro de leitura");
            fclose(arq);
            return 0;
        }
        else
        {
            if(strcmp(aux[i].nome,player.nome)==0)
            {
                fclose(arq);
                return 1;
            }
        }
        i++;
    }
    while(i < 5);
    fclose(arq);
    return 2;
}
// --------------- RECORDES ----------------------

// --------------- PLATAFORMAS ----------------------
void desenha_plataforma(PLATAFORMA plataforma)
{
    // Desenha uma plataforma
    int i,j;
    int x,y;

    x = plataforma.posicao.x;
    y = plataforma.posicao.y;
    for (i=0; i<plataforma.tamanho && x>1 && x<80; i++)
    {
        textbackground(WHITE);
        putchxy(x,y,' ');
        x++;
    }
    textbackground(0);
}
void movimenta_plataforma(PLATAFORMA plataforma[ ],int quantia)
{
    // Movimenta todas as plataformas da lista
    int i,j; // INDICE
    int x,y; // COPIAS DAS COORDENADAS
    for(j=0; j<quantia; j++)
    {
        if (plataforma[j].aparecendo)
        {
            if(plataforma[j].espera==0)
            {
                x = plataforma[j].posicao.x;
                y = plataforma[j].posicao.y;
                switch (plataforma[j].direcao)
                {
                case 0: //Cima
                    for(i=0; i<plataforma[j].tamanho; i++)  // Apaga a plataforma toda se o movimento for pra cima
                    {
                        textbackground(0);
                        if(x>1 && x <80)
                            putchxy(x,y,' ');
                        x++;
                    }
                    plataforma[j].posicao.y--;
                    break;
                case 1: // Direita
                    textbackground(0);
                    if (x<80 && x > 1)
                        putchxy(x,y,' ');
                    plataforma[j].posicao.x++;
                    break;
                case 2:  // Esquerda
                    textbackground(0);
                    if(x+plataforma[j].tamanho-1>1 && x+plataforma[j].tamanho-1<80)
                        putchxy(x+plataforma[j].tamanho-1,y,' ');
                    plataforma[j].posicao.x--;
                    break;
                }
                if(plataforma[j].posicao.y<=1 || plataforma[j].posicao.x>=80 || plataforma[j].posicao.x+plataforma[j].tamanho<=1)
                    // Caso a plataforma esteja fora do espaço de jogo
                    plataforma[j].aparecendo=0;
                else if(plataforma[j].posicao.y>1 && plataforma[j].posicao.y<25 && plataforma[j].posicao.x>1 && plataforma[j].posicao.x<80)
                {
                    // Para não desenhar a plataforma nas bordas da tela de jogo
                    desenha_plataforma(plataforma[j]);
                }
            }
        }
    }
}
int busca_num_de_plats()
{
    // Busca o número de plataformas no arquivo de texto
    int n;
    FILE *arq;
    arq = fopen("plataformas.txt", "r");
    if(!arq)
    {
        printf("Erro na abertura do arquivo das plataformas");
        return 0;
    }
    else
    {
        fscanf(arq, "%d", &n);
        fclose(arq);
        return n;
    }
}
void busca_plataformas(PLATAFORMA plats[])
{
    // Busca todas as plataformas do jogo no arquivo texto
    // Gera os dados aleatórios: Posicao e direcao
    FILE *arq;
    int n, i = 0;
    PLATAFORMA plat_aux;

    arq = fopen("plataformas.txt", "r");
    if(!arq)
        printf("Erro na abertura do arquivo das plataformas");
    else
    {
        fscanf(arq, "%d", &n);
        while(!feof(arq))
        {
            fscanf(arq, "%d %d", &plat_aux.tamanho, &plat_aux.velocidade);
            plat_aux.aparecendo = 1;
            plat_aux.direcao = busca_direcao((int) (rand() / (double) RAND_MAX * (DIRECAO + 1)));
            plat_aux.posicao.x = (int) 2+ (rand() / (double) RAND_MAX * (X));
            plat_aux.posicao.y =(int) 2+ (rand() / (double) RAND_MAX * (Y));
            plat_aux.espera=0;
            plats[i] = plat_aux;
            i++;
        }
        fclose(arq);
    }
}
void gera_plat(PLATAFORMA plataforma[ ],int quantia)
{
    // Gera as novas posicoes das plataformas quando elas saem da tela
    int i;
    for (i=0; i<quantia; i++)
    {
        if(!plataforma[i].aparecendo)
        {
            plataforma[i].direcao= busca_direcao((int) (rand() / (double) RAND_MAX * (DIRECAO + 1)));
            switch(plataforma[i].direcao)
            {
            case 0:
                plataforma[i].posicao.x=(int)2+(rand() / (double) RAND_MAX * (X));
                plataforma[i].posicao.y=24;
                break;
            case 1:
                plataforma[i].posicao.x=1;
                plataforma[i].posicao.y=(int)2+(rand() / (double) RAND_MAX * (Y));
                break;
            case 2:
                plataforma[i].posicao.x=79;
                plataforma[i].posicao.y=(int)2+(rand() / (double) RAND_MAX * (Y));
                break;
            }
            plataforma[i].espera=0;
            plataforma[i].aparecendo=1;
        }
    }
}
int busca_direcao(int dir)
{
    // Dessa forma é possível a proporção 20% - cima, 40% - direita e esqurda
    switch(dir)
    {
    case 0:
        return 0;
    case 1:
    case 2:
        return 1;
    case 3:
    case 4:
        return 2;
    }
}
// --------------- PLATAFORMAS ----------------------

// --------------- JOGADOR ----------------------
void inf_do_player(JOGADOR *player)
{
    // Pede o nome do jogador
    gotoxy(34,8);
    puts("DIGITE SEU NOME");
    gotoxy(34,9);
    textcolor(LIGHTCYAN);
    gets(player->nome);
    textcolor(LIGHTGRAY);
    clrscr();
}
int em_cima(JOGADOR *player, PLATAFORMA plat[ ],int quantia,int *numplat)
{
    // Verifica se o jogador está em cima de uma plataforma
    int i;
    for(i=0; i<quantia; i++)
    {
        // Testa se o personagem está em cima de alguma plataforma

        if (player->posicao.y==plat[i].posicao.y-1 && player->posicao.x >= plat[i].posicao.x && player->posicao.x <= plat[i].posicao.x+plat[i].tamanho)
        {
            *numplat=i; // Salva a plataforma que o personagem está em cima
            player->velocidade=plat[i].velocidade;
            player->espera=plat[i].espera;
            return 1;
        }
    }
    return 0; // SE NAO ESTIVER, ACABA NO RETURN 0 DANDO "FALSE" NO IF DA FUNCAO DESENVOLVIMENTO
}
void move_jogador (JOGADOR *player,int direcao)
{
    //Move o jogador na direção informada, se seu status for em movimento
    if(player->status==0)
    {
        if(player->espera==0)
        {
            if(player->posicao.x>1 && player->posicao.x < 80 && player->posicao.y > 1 && player->posicao.y < 25)
                // Testa pra não apagar os cantos
                putchxy(player->posicao.x,player->posicao.y,' ');
            switch(direcao)
            {
            case 0:             // PULA
                if (player->poder==INVENCIVEL)
                {
                    if(player->posicao.y != 2)
                        player->posicao.y--;
                }
                else player->posicao.y--;
                break;
            case 1:             // DIREITA
                if (player->poder==INVENCIVEL)
                {
                    if(player->posicao.x != 79)
                        player->posicao.x++;
                }
                else player->posicao.x++;
                break;
            case 2:             // ESQUERDA
                if (player->poder==INVENCIVEL)
                {
                    if(player->posicao.x != 2)
                        player->posicao.x--;
                }
                else player->posicao.x--;
                break;
            case 3:             // CAI
                if (player->poder==INVENCIVEL)
                {
                    if(player->posicao.y != 24)
                        player->posicao.y++;
                }
                else player->posicao.y++;
                break;
            default:
                break;
            }
            // Se o personagem chegar nas bordas, ele morre
            if ((player->posicao.y==25 || player->posicao.y==1 || player->posicao.x==1 || player->posicao.x==80)&& player->poder!=INVENCIVEL)
                player->status=1;
            else putchxy(player->posicao.x,player->posicao.y,player->ch);
        }
    }
}
void pula(JOGADOR *player)
{
    player->velocidade = 1;
    if(player->espera==0)
        player->alcance-=1;    // Diminui o alcance, até chegar a zero na função desenvolvimetno
    move_jogador(player,0);  // Move o jogador para cima (pula)
}
void morreu(JOGADOR *player)
{
    // Faz a ações caso o jogador tenha morrido
    if(player->status==1)
    {
        if (player->posicao.y>1 && player->posicao.y<25 && player->posicao.x>1 &&player->posicao.x<80)
            //Apaga o jogador se ele não estiver nas bordas
            putchxy(player->posicao.x,player->posicao.y,' ');
        player->poder=NENHUM;
        player->vidas-=1;
        textcolor(WHITE);
        textbackground(RED);
        gotoxy(35,25);
        printf("Jogador morreu");
        Sleep(500);
        textcolor(LIGHTGRAY);
        textbackground(0);
        if(player->vidas>0)
        {
            // Se o jogador ainda tiver vidas
            textcolor(WHITE);
            textbackground(RED);
            gotoxy(10,25);
            textcolor(LIGHTGRAY);
            textbackground(0);

            player->posicao.x=40;
            player->posicao.y=12;
            putchxy(40,12,player->ch);
            player->status=2;   //Muda o status para parado
        }
    }

}
void parado(JOGADOR *player,clock_t *inicio)
{
    // Mantém o jogador parado até ser pressionada uma tecla
    if(player->status==2)
    {
        gotoxy(20,25);
        textbackground(RED);
        printf("Pressione SPACE/CIMA para continuar a jogar");
        textbackground(0);
    }
    if(le_setas()==CIMA && player->status==2)
    {
        player->status=0;
    }
}
void posiciona_jogador(JOGADOR *player)
{
    player->posicao.x=40;
    player->posicao.y=12;
    player->vidas=3;
    player->ch='X';
    player->status=2;
    player->alcance=0;
    player->extrapulo=0;
    player->espera=0;
}
void controla_movimento_jogador(JOGADOR *player, PLATAFORMA plataformas[], int num_de_plats, int *numplat)
{
    if (em_cima(player,plataformas,num_de_plats,numplat))
    {
        // Caso esteja em cima de alguma plataforma
        if (player->poder!=PARATEMPO)
        {
            // Se o tempo não estiver parado pelo poder
            if(player->poder==LEVITA)
            {
                // Movimenta o jogador apenas para cima se estiver levitando
                if(plataformas[*numplat].direcao==0)
                {
                    move_jogador(player,plataformas[*numplat].direcao);
                }
            }
            else
            {
                // Senão movimenta na direção da plataforma
                move_jogador(player,plataformas[*numplat].direcao);
            }
        }
        player->extrapulo = 1;
    }
    else if((player->posicao.y==24 && player->poder==INVENCIVEL))
        // Se estiver em cima da "plataforma gigante" enquanto invencível, também atribui 1 para o pulo extra
        player->extrapulo=1;
}
void controla_pulo(JOGADOR *player, PLATAFORMA plataformas[], int num_de_plats, int *numplat)
{
    int tecla;
    fflush(stdin);
    if (le_setas() !=6)
    {
        tecla=le_setas();
        if (tecla==CIMA)
        {
            if((em_cima(player,plataformas,num_de_plats,numplat)) || (player->posicao.y==24 && player->poder==INVENCIVEL))
            {
                // Se pressionar espaco em cima de uma plataforma ativara o pulo normal
                player->alcance = ALCANCE_PULO;

                // Se tiver o poder de superpulo, duplica o alcance do pulo, seja o normal ou o extra
                if (player->poder==SUPERPULO)
                    player->alcance*=2;
            }
            else if(player->extrapulo && player->alcance==0)     // Se apertar e nao estiver em cima, e o puloextra estiver ativado, ativara o pulo extra.
            {
                player->alcance =(int)(ALCANCE_PULO/2);
                player->extrapulo = 0;

                // Se tiver o poder de superpulo, duplica o alcance do pulo, seja o normal ou o extra
                if (player->poder==SUPERPULO)
                    player->alcance*=2;
            }
        }
        else if(tecla==DIREITA)
            move_jogador(player,DIREITA);
        else if(tecla==ESQUERDA)
            move_jogador(player,ESQUERDA);
    }

    if (player->alcance>0 && plataformas[*numplat].direcao!=0)  // Continua o pulo
    {
        pula(player);
    }
    else if (player->alcance>0 && !em_cima(player,plataformas,num_de_plats,numplat) && player->extrapulo==0)
    {
        pula(player);
    }
    else if (player->alcance>0 && plataformas[*numplat].direcao==0)
    {
        // Pulo em plataformas que estão subindo, somente ativarao no pulo normal
        move_jogador(player,plataformas[*numplat].direcao);
        pula(player);
    }
    else if (player->alcance==0 && !em_cima(player,plataformas,num_de_plats,numplat) && player->status==0 && player->poder!=LEVITA)
    {
        // Cai se não estiver em cima de nenhuma plataforma e não tiver o poder de levitação
        if(player->poder != GRAVIDADE)
            player->velocidade=2;
        move_jogador(player,3);
    }
}
// --------------- JOGADOR ----------------------

// --------------- PODER ----------------------
int sorteia_poder(COORDENADA *poder)
{
    // Sorteia o tipo de poder e sua coordenada
    poder->x = (int) 2+ (rand() / (double) RAND_MAX * (X));
    poder->y =(int) 2+ (rand() / (double) RAND_MAX * (Y));
    return 1+(rand() / (double) RAND_MAX * QTD_PODER);  // Somei 1 para o "NENHUM" nao aparecer
}
void mostra_poder(COORDENADA poder, int cor)
{
    // Desenha o poder na tela de jogo
    textbackground(cor);
    putchxy(poder.x, poder.y, ' ');
    textbackground(0);
}
void some_poder(COORDENADA poder)
{
    // Apaga o poder na tela de jogo
    textbackground(0);
    putchxy(poder.x,poder.y,' ');
}
int busca_cor_poder(int tipo)
{
    //  Busca a cor do poder de acordo com seu tipo
    switch(tipo)
    {
    case LEVITA:            //FEITO
        return LIGHTGRAY;
    case SUPERPULO:         // FEITO
        return LIGHTBLUE; // ERA PRA SER TURQUESA
    case INVENCIVEL:        // FEITO
        return YELLOW;
    case PARATEMPO:         // FEITO
        return MAGENTA;
    case GRAVIDADE:
        return LIGHTGREEN; // ERA PRA SER ROXO
    case VIDA:              // FEITO
        return GREEN;
    case PONTOS:            // FEITO
        return BROWN;
    }
}
void controla_poder(JOGADOR *player, CONTROLE_PODER *controle_poder)
{
    // Controla todas ações relacionadas aos poderes

    if(player->poder == NENHUM)  // Caso o jogador não tenha nenhum poder
    {
        if(controle_poder->aparecer_poder == 0 && !(controle_poder->aparecendo))
        {
            // Se estiver na hora de aparecer um poder e não tiver nenhum aparecendo
            controle_poder->tipo_poder = sorteia_poder(&controle_poder->posicao_poder);
            mostra_poder(controle_poder->posicao_poder, busca_cor_poder(controle_poder->tipo_poder));
            controle_poder->aparecendo = 1;
            controle_poder->aparecer_poder = APARECE_PODER;
        }
        else if(controle_poder->aparecer_poder > 0)
        {
            // Controla o tempo de aparecimento do próximo poder
            controle_poder->aparecer_poder -= controle_poder->secs;
        }
    }
    else
    {
        //Caso o jogador tenha um poder
        if(controle_poder->tempo_poder == 0)
        {
            // Muda pra nenhum quando o tempo acabar
            player->poder = NENHUM;
        }
        else
        {

            if(player->poder == VIDA)
            {
                player->vidas += 1;
                controle_poder->tempo_poder = 0;
            }
            else if(player->poder == PONTOS)
            {
                player->pontos += 60;
                controle_poder->tempo_poder = 0;
            }
            else
            {
                // Diminui o tempo de poder
                controle_poder->tempo_poder -= controle_poder->secs;
            }
        }
    }

    if (controle_poder->aparecendo)
    {
        if(controle_poder->sumir_poder>0)
        {
            //Se ainda não for o tempo de sumir o poder, mostra e diminui o tempo restante
            mostra_poder(controle_poder->posicao_poder, busca_cor_poder(controle_poder->tipo_poder));
            controle_poder->sumir_poder -= controle_poder->secs;

            if(player->posicao.x == controle_poder->posicao_poder.x && player->posicao.y == controle_poder->posicao_poder.y)
            {
                // Se o jogador estiver na mesma posição do poder
                player->poder = controle_poder->tipo_poder;
                some_poder(controle_poder->posicao_poder);
                controle_poder->tempo_poder = TEMPO_PODER;
                controle_poder->sumir_poder = SOME_PODER;
                controle_poder->aparecendo = 0;
            }
        }
        else
        {
            // Se o tempo tiver acabado, o poder some
            some_poder(controle_poder->posicao_poder);
            controle_poder->sumir_poder = SOME_PODER;
            controle_poder->aparecendo = 0;
        }
    }
}
char* nome_poder(JOGADOR player)
{
    switch(player.poder)
    {
    case LEVITA:
        return "LEVITA";
        break;
    case SUPERPULO:
        return "SUPER PULO";
        break;
    case INVENCIVEL:
        return "IMORRIVEL";
        break;
    case PARATEMPO:
        return "PARATEMPO";
        break;
    case GRAVIDADE:
        return "GRAVIDADE";
        break;
    case VIDA:
        return "LIFE + 1";
        break;
    case PONTOS:
        return "PONTOS + 60";
        break;
    default:
        return "SEM PODER";
        break;
    }
}
// --------------- PODER ----------------------

// --------------- ÁREA DE JOGO ----------------------
void atualiza_inf(JOGADOR player)
{
    // Atualiza as informações nas barras da tela de jogo
    int i;

    textcolor(WHITE);
    textbackground(BLUE);
    gotoxy(6,1);
    puts("MARIO 6.9");
    gotoxy(20,1);
    printf("Pontos:%d",player.pontos);
    gotoxy(35,1);
    printf("Nome:%s",player.nome);
    gotoxy(50,1);
    printf("Poder:%s",nome_poder(player));
    gotoxy(70,1);
    printf("Vidas:%d",player.vidas);
    gotoxy(10,25);
    textbackground(RED);
    if(player.status==0) // Testa se está vivo para limpar a barra vermelha
        for(i=0; i<60; i++)
            printf(" ");
    textcolor(LIGHTGRAY);
    textbackground(0);
}
void desenha_bordas()
{
    // Desenha as bordas da área de jogo
    int i;
    for (i=1; i<=80; i++)
    {
        textbackground(BLUE);
        putchxy(i,1,' ');
    }
    for (i=1; i<25; i++)
    {
        textbackground(0);
        putchxy(80,i,'|');
    }
    for (i=80; i>=1; i--)
    {
        textbackground(RED);
        putchxy(i,25,' ');
    }
    for (i=24; i>=1; i--)
    {
        textbackground(0);
        putchxy(1,i,'|');
    }
}
// --------------- ÁREA DE JOGO ----------------------

// --------------- VELOCIDADE ---------------------
int atualiza_espera(JOGADOR *player, PLATAFORMA plataformas[ ], int quantia)
{
    int i;

    if (player->espera==0)      // Se estiver sem  , procura a procura espera
    {
        if(player->velocidade==1)
            player->espera=LENTO;
        else if (player->velocidade==2)
            player->espera=RAPIDO;
    }
    else if(player->espera>0)   // Se não diminui 1 da espera
        player->espera--;
    for(i=0; i<quantia; i++)
    {
        if(plataformas[i].espera==0)   // Se estiver sem espera, procura a procura espera
        {
            if(plataformas[i].velocidade==2)
                plataformas[i].espera=RAPIDO;
            else if(plataformas[i].velocidade==1)
                plataformas[i].espera=LENTO;
        }
        else if(plataformas[i].espera>0)    // Se não diminui 1 da espera
            plataformas[i].espera--;
    }
}
// --------------- VELOCIDADE ---------------------

// --------------- JOGO ----------------------
int le_setas()
{
    if ((GetKeyState (VK_SPACE) & 0x80) || (GetKeyState (VK_UP) & 0x80))
        return CIMA;
    if (GetKeyState (VK_RIGHT) & 0x80)
        return DIREITA;
    if (GetKeyState (VK_LEFT) & 0x80)
        return ESQUERDA;
    if (GetKeyState (VK_DOWN) & 0x80)
        return BAIXO;
    if (GetKeyState (VK_RETURN) & 0x80)
        return 4; // ENTER
    if (GetKeyState (VK_ESCAPE) & 0x80)
        return 5; // ESC
    else return 6;
}
void desenvolvimento(JOGADOR *player, int num_de_plats)   // SO DEUS SABE
{
    PLATAFORMA plataformas[num_de_plats];
    int numplat;    // Número da plataforma que o jogador está em cima
    int sec = 0; //variavel auxiliar para somar a cada segundo
    CONTROLE_PODER controle_poder = {APARECE_PODER, SOME_PODER, TEMPO_PODER, sec}; // Controle do poder
    clock_t inicio, fim;

    inicio = clock();
    player->poder = NENHUM;
    player->pontos = 0;

    busca_plataformas(plataformas);
    posiciona_jogador(player);
    clrscr(); // Limpa a tela do menu
    desenha_bordas();

    while(player->vidas>0)
    {
        fim = clock(); // salva tempo ao terminar
        sec = (int) ((fim - inicio) / CLOCKS_PER_SEC);
        if(sec == 1)
        {
            inicio = clock(); // Caso já tenha passado 1s, ele zera a var do clock
        }

        atualiza_inf(*player);

        controla_pulo(player, plataformas, num_de_plats, &numplat);
        controla_movimento_jogador(player, plataformas, num_de_plats, &numplat);

        //Movimenta as plataformas
        if (player->poder!=PARATEMPO)
            movimenta_plataforma(plataformas, num_de_plats);
        atualiza_espera(player,plataformas,num_de_plats); // Atualiza a espera das plats e do player

        gera_plat(plataformas, num_de_plats);


        if (!(player->posicao.y==25 || player->posicao.y==1 || player->posicao.x==1 || player->posicao.x==80))
            putchxy(player->posicao.x,player->posicao.y,player->ch); //Mantém o jogador printado na tela

        morreu(player);
        parado(player,&inicio);

        if(player->status == 0)
        {
            // Se o jogador está em movimento, soma um ponto a cada segundo que passa e faz o controle do poder
            player->pontos += sec;
            controle_poder.secs = sec;
            controla_poder(player, &controle_poder);
        }
        Sleep(50);
    }
}
// --------------- JOGO ----------------------
