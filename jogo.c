#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_WIDTH (1000)
#define WINDOW_HEIGHT (600)

//iniciar posição do sprite
float x_pos = 50;
float y_pos = 150;
float x_vel = 0;
float y_vel = 0;

//velocidade em pixels/segundo
#define SPEED (300)

//controlar passagem de tempo
float time;

//controlar renderização de inimigos
int controler;

//menus
int menu1 = 1, menu2 = 0, menu3 = 0, menu4 = 0, menu5 = 0, cmenu = 0, movmenu = 0, histmenu = 0, rankmenu = 0;

//variáveis para o registro de jogadores no arquivo
char best1[5], best2[5], best3[5], best4[5] ,best11, best12, best13, best14, best15, best16;
int pos[4], bestscore[4];

//registrar pontuação após fim de jogo
int updateranking = 0;
int digname = 0;
char playername[5] = ".....";

//exibir ranking
char stringscore1[6], stringscore2[6], stringscore3[6], stringscore4[6];

//pontuação
int score;
char textpoint[20];

//bonus
int qbonus = 0;

//determinar o fim do jogo
int gameover;

//inicializar câmera
SDL_Rect cameraRect = {0, 0, 640, 480};
int levelWidth, levelHeight;

int qenemies = 0;

typedef struct {
   int x;
   int y;
   float dx;
   float dy;
   int health;
   SDL_Surface* surface;
   SDL_Texture* texture;
}ENTITY;

char *intTochar(int value, char *str){

   char temp;
   int i = 0;

   if (value == 0){
   str[i] = '0';
   }

   while (value > 0) {
   int digit = value % 10;

   str[i] = digit + '0';
   value /= 10;
   i++;
   }

   i = 0;
   int j = strlen(str) - 1;

   while (i < j) {
   temp = str[i];
   str[i] = str[j];
   str[j] = temp;
   i++;
   j--;
   }

   return str;
}

void blit(SDL_Texture *texture, int x, int y, SDL_Renderer *rend){

	SDL_Rect dest;

	dest.x = x;
	dest.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	SDL_RenderCopy(rend, texture, NULL, &dest);
}

void spawnenemies(ENTITY enem[], int qenem, char *file, int xpos, int controler, SDL_Renderer *rend){

   for (int i = 0; i < qenem; i++){
   enem[i].surface = IMG_Load(file);
   enem[i].texture = SDL_CreateTextureFromSurface(rend, enem[i].surface);

   SDL_FreeSurface(enem[i].surface);

      if (controler == 1){
      enem[i].x = xpos;
      enem[i].y = rand() % (WINDOW_HEIGHT / 2) + 10;
      enem[i].dx = -1 * (rand() % 10 + 5);
      enem[i].dy = rand() % 2;
      enem[i].health = 1;
      qenemies += 1;
      }
   }
}

void enemiesupdate(ENTITY enem[], int qenem){

   for (int i = 0; i < qenem; i++){
      if (enem[i].health == 1){
      enem[i].x += enem[i].dx;
      enem[i].y += enem[i].dy;
      }
   }
}

void hitenemies(ENTITY enem[], ENTITY bullet, int qenem){

   float distancex, distancey;
   for (int i = 0; i < qenem; i++){
      if (enem[i].health > 0 && bullet.health == 1){
         if (enem[i].x > bullet.x){
         distancex = enem[i].x - bullet.x;
         }
         else{
         distancex = bullet.x - enem[i].x;
         }
         if (enem[i].y > bullet.y){
         distancey = enem[i].y - bullet.y;
         }
         else{
         distancey = bullet.y - enem[i].y;
         }

         if (distancex < 40 && distancey < 40){
         enem[i].health = 0;
         bullet.health = 0;
         score += 100;
         }
      }
   }
}

void hitboss(ENTITY *boss, ENTITY *bullet){

   float distancex, distancey;
   if (boss->health > 0 && bullet->health == 1){
      if (boss->x > bullet->x){
      distancex = boss->x - bullet->x;
      }
      else{
      distancex = bullet->x - boss->x;
      }
      if (boss->y > bullet->y){
      distancey = boss->y - bullet->y;
      }
      else{
      distancey = bullet->y - boss->y;
      }

      if (distancex < 40 && distancey < 60){
      boss->health -= 1;
      //printf("boss health :%d\n", boss->health);
      bullet->health = 0;
      }
   }
}

void playerbosscollision(ENTITY boss){

   float distancepx, distancepy;
   if (x_pos > boss.x){
   distancepx = x_pos - boss.x;
   }
   else{
   distancepx = boss.x - x_pos;
   }
   if (y_pos > boss.y){
   distancepy = y_pos - boss.y;
   }
   else{
   distancepy = boss.y - y_pos;
   }

   if (distancepx < 60 && distancepy < 60 && boss.health > 0 && gameover != 0){
   //printf("VOCÊ FOI ATINGIDO!\n");
   gameover = 1;
   time = 0;
   }
}

void hitbossplayer(ENTITY enbullets[], ENTITY boss, int qbullets){

   float distancebulletx, distancebullety;
   for (int i = 0; i < qbullets; i++){
      if (enbullets[i].health == 1 && boss.health > 0){
         if (enbullets[i].x > x_pos){
         distancebulletx = enbullets[i].x - x_pos;
         }
         else{
         distancebulletx = x_pos - enbullets[i].x;
         }
         if (enbullets[i].y > y_pos + 20){
         distancebullety = enbullets[i].y - (y_pos + 20);
         }
         else{
         distancebullety = (y_pos + 20) - enbullets[i].y;
         }

         if (distancebulletx < 40 && distancebullety < 40){
         gameover = 1;
         time = 0;
         }
      }
   }
}

void playercollision(ENTITY enem[], int qenem){

   float distancepx, distancepy;
   for (int i = 0; i < qenem; i++){
      if (x_pos > enem[i].x){
      distancepx = x_pos - enem[i].x;
      }
      else{
      distancepx = enem[i].x - x_pos;
      }
      if (y_pos > enem[i].y){
      distancepy = y_pos - enem[i].y;
      }
      else{
      distancepy = enem[i].y - y_pos;
      }

      if (distancepx < 40 && distancepy < 40 && enem[i].health == 1 && gameover == 0){
      //printf("VOCÊ FOI ATINGIDO!\n");
      gameover = 1;
      time = 0;
      }
   }
}

void hitplayer(ENTITY enbullets[], ENTITY enem[], int qbullets){

   float distancebulletx, distancebullety;
   for (int i = 0; i < qbullets; i++){
      if (enbullets[i].health == 1 && enem[i].health == 1){
         if (enbullets[i].x > x_pos){
         distancebulletx = enbullets[i].x - x_pos;
         }
         else{
         distancebulletx = x_pos - enbullets[i].x;
         }
         if (enbullets[i].y > y_pos + 20){
         distancebullety = enbullets[i].y - (y_pos + 20);
         }
         else{
         distancebullety = (y_pos + 20) - enbullets[i].y;
         }

         if (distancebulletx < 40 && distancebullety < 40){
         gameover = 1;
         time = 0;
         }
      }
   }
}

void spawnbonus(ENTITY *bonus, int xpos, int ypos){

   bonus->x = xpos;
   bonus->y = ypos;
   bonus->dx = -4;
   bonus->dy = 2;
   bonus->health = 1;

   qbonus += 1;
   //printf("SURGIU UM BONUS\n");

}

void updatebonus(ENTITY *bonus, int *gotbonus){

   if (bonus->health == 1){
   bonus->x += bonus->dx;
   bonus->y += bonus->dy;
   }

   int dbonusx, dbonusy;
   if (bonus->health == 1){
      if (bonus->x > x_pos){
      dbonusx = bonus->x - x_pos;
      }
      else{
      dbonusx = x_pos - bonus->x;
      }
      if (bonus->y > y_pos){
      dbonusy = bonus->y - y_pos;
      }
      else{
      dbonusy = y_pos - bonus->y;
      }

      if (dbonusx < 30 && dbonusy < 30){
      bonus->health = 0;
      *gotbonus = 1;
      //printf("VOCÊ PEGOU UM BONUS!\n");
      }
   }
}

int main(int argc, char *argv[]){

   //inicializar tempo e condição de jogo
   time = 0;
   score = 0;
   gameover = 0;

   //inicializar gráficos
   if (SDL_Init(SDL_INIT_VIDEO) != 0){
   printf("error initializing SDL: %s\n", SDL_GetError());
   return 1;
   }

   TTF_Init();

   SDL_Window* win = SDL_CreateWindow("Carbon Unity", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

   if (!win){
   printf("error creating window: %s\n", SDL_GetError());
   SDL_Quit();
   return 1;
   }

   //criando o renderer
   Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
   SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

   if (!rend){
   printf("error creating renderer: %s\n", SDL_GetError());
   SDL_DestroyWindow(win);
   SDL_Quit();
   return 1;
   }

   //abrir arquivo da pontuação
   FILE *file;
   file = fopen("pontuacao", "r+");

   //inicializar texto
   TTF_Font *font = TTF_OpenFont("arial-mt-narrow-bold-italic.ttf", 20);

   SDL_Color fontcolor = {255, 255, 255};

   //pontuação
   SDL_Surface *textsurface;
   SDL_Texture *text;

   //valor da pontuação
   SDL_Surface *pointsurface;
   SDL_Texture *points;

   SDL_Rect localtext = {cameraRect.x + 15, cameraRect.y + 10, 0, 0};

   //carregar as imagens de jogador e background usando a biblioteca SDL_image
   SDL_Surface* bgsurface = IMG_Load("spacestage.png");
   SDL_Surface* playersurface = IMG_Load("nave.png");

   //carregar imagem para as bullets
   ENTITY bullet;
   memset(&bullet, 0, sizeof(ENTITY));
   bullet.surface = IMG_Load("bullet12.png");

   //carregar imagem para o inimigo 1
   ENTITY enemy;
   memset(&enemy, 0 , sizeof(ENTITY));
   enemy.surface = IMG_Load("enemy.png");

   //segunda horda de inimigos
   int i;
   ENTITY enemies[4];

   for (i = 0; i < 4; i++){
   enemies[i].surface = IMG_Load("enemy.png");
   enemies[i].texture = SDL_CreateTextureFromSurface(rend, enemies[i].surface);

   SDL_FreeSurface(enemies[i].surface);
   }

   //terceira horda de inimigos
   ENTITY shotenemies[2];

   for (i = 0; i < 2; i++){
   shotenemies[i].surface = IMG_Load("shooterenemy.png");
   shotenemies[i].texture = SDL_CreateTextureFromSurface(rend, shotenemies[i].surface);

   SDL_FreeSurface(shotenemies[i].surface);
   }

   //quarta horda de inimigos
   ENTITY enemyg[3];

   for (i = 0; i < 3; i++){
   enemyg[i].surface = IMG_Load("greenenemy.png");
   enemyg[i].texture = SDL_CreateTextureFromSurface(rend, enemyg[i].surface);

   SDL_FreeSurface(enemyg[i].surface);
   }

   //quinta horda de inimigos
   ENTITY penemy[3];

   for (i = 0; i < 3; i++){
   penemy[i].surface = IMG_Load("plasmaenemy.png");
   penemy[i].texture = SDL_CreateTextureFromSurface(rend, penemy[i].surface);

   SDL_FreeSurface(penemy[i].surface);
   }

   //bullets para a quinta horda de inimigos
   ENTITY pbullets[3];

   for (i = 0; i < 3; i++){
   pbullets[i].surface = IMG_Load("plasmabullet.png");
   pbullets[i].texture = SDL_CreateTextureFromSurface(rend, pbullets[i].surface);

   SDL_FreeSurface(pbullets[i].surface);
   }

   //sexta horda de inimigos
   ENTITY penemy2[5];

   for (i = 0; i < 5; i++){
   penemy2[i].surface = IMG_Load("plasmaenemy.png");
   penemy2[i].texture = SDL_CreateTextureFromSurface(rend, penemy2[i].surface);

   SDL_FreeSurface(penemy2[i].surface);
   }

   //bullets para a quinta horda de inimigos
   ENTITY pbullets2[5];

   for (i = 0; i < 5; i++){
   pbullets2[i].surface = IMG_Load("plasmabullet.png");
   pbullets2[i].texture = SDL_CreateTextureFromSurface(rend, pbullets2[i].surface);

   SDL_FreeSurface(pbullets2[i].surface);
   }

   //boss 1
   ENTITY boss;
   int bossdefeated = 0;
   boss.surface = IMG_Load("deathen.png");
   boss.texture = SDL_CreateTextureFromSurface(rend, boss.surface);

   SDL_FreeSurface(boss.surface);

   ENTITY bossbullets[4];
   for (i = 0; i < 4; i++){
   bossbullets[i].surface = IMG_Load("plasmabullet.png");
   bossbullets[i].texture = SDL_CreateTextureFromSurface(rend, bossbullets[i].surface);

   SDL_FreeSurface(bossbullets[i].surface);
   }

   //carregar bonus
   ENTITY bonus;
   bonus.surface = IMG_Load("bonus.png");
   bonus.texture = SDL_CreateTextureFromSurface(rend, bonus.surface);
   int gotbonus1 = 0;

   SDL_FreeSurface(bonus.surface);

   if (!bgsurface || !playersurface || !bullet.surface || !enemy.surface){
   printf("error creating surface\n");
   SDL_DestroyRenderer(rend);
   SDL_DestroyWindow(win);
   SDL_Quit();
   return 1;
   }

   //carregar os dados da imagem na memória do hardware gráfico
   SDL_Texture* bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);
   SDL_Texture* playertex = SDL_CreateTextureFromSurface(rend, playersurface);
   bullet.texture = SDL_CreateTextureFromSurface(rend, bullet.surface);
   enemy.texture = SDL_CreateTextureFromSurface(rend, enemy.surface);

   SDL_FreeSurface(bgsurface);
   SDL_FreeSurface(playersurface);
   SDL_FreeSurface(bullet.surface);
   SDL_FreeSurface(enemy.surface);

   if (!bgtex || !playertex || !bullet.texture || !enemy.surface){
   printf("error creating texture: %s\n", SDL_GetError());
   SDL_DestroyRenderer(rend);
   SDL_DestroyWindow(win);
   SDL_Quit();
   return 1;
   }

   //criar retângulo para o jogador
   SDL_Rect playerdest;

   //pegar a escala e as dimensões da textura
   SDL_QueryTexture(playertex, NULL, NULL, &playerdest.w, &playerdest.h);
   playerdest.w /= 6;
   playerdest.h /= 6;

   //inputs
   int up = 0;
   int down = 0;
   int left = 0;
   int right = 0;
   int fire = 0;

   //controlar o comando para fechar a janela
   int close_requested = 0;

   //loop de animação
   while (!close_requested){
   //processar eventos
   SDL_Event event;

      while (SDL_PollEvent(&event)){
         switch (event.type){
         case SDL_QUIT:
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
            up = 1;
            break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
            left = 1;
            break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
            down = 1;
            break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
            right = 1;
            break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_SPACE:
            fire = 1;
            //printf("ATIRANDO\n");
            break;
            }
         break;
         case SDL_KEYUP:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
            up = 0;
            break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
            left = 0;
            break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
            down = 0;
            break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
            right = 0;
            break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_SPACE:
            fire = 0;
            //printf("PAROU DE ATIRAR\n");
            break;
            }
         break;
         }
      }

      i = 0;
      while (digname == 1){
      bgsurface = IMG_Load("gameover.png");
      bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && digname == 1){
         if (i == 5){
         digname = 0;
         }
         switch (event.type){
         case SDL_QUIT:
         digname = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_A:
            if (i <= 5){
            playername[i] = 'A';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_B:
            if (i <= 5){
            playername[i] = 'B';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_C:
            if (i <= 5){
            playername[i] = 'C';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_D:
            if (i <= 5){
            playername[i] = 'D';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_E:
            if (i <= 5){
            playername[i] = 'E';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_F:
            if (i <= 5){
            playername[i] = 'F';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_G:
            if (i <= 5){
            playername[i] = 'G';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_H:
            if (i <= 5){
            playername[i] = 'H';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_I:
            if (i <= 5){
            playername[i] = 'I';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_J:
            if (i <= 5){
            playername[i] = 'J';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_K:
            if (i <= 5){
            playername[i] = 'K';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_L:
            if (i <= 5){
            playername[i] = 'L';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_M:
            if (i <= 5){
            playername[i] = 'M';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_N:
            if (i <= 5){
            playername[i] = 'N';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_O:
            if (i <= 5){
            playername[i] = 'O';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_P:
            if (i <= 5){
            playername[i] = 'P';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_Q:
            if (i <= 5){
            playername[i] = 'Q';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_R:
            if (i <= 5){
            playername[i] = 'R';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_S:
            if (i <= 5){
            playername[i] = 'S';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_T:
            if (i <= 5){
            playername[i] = 'T';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_U:
            if (i <= 5){
            playername[i] = 'U';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_V:
            if (i <= 5){
            playername[i] = 'V';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_W:
            if (i <= 5){
            playername[i] = 'W';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_X:
            if (i <= 5){
            playername[i] = 'X';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_Y:
            if (i <= 5){
            playername[i] = 'Y';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_Z:
            if (i <= 5){
            playername[i] = 'Z';
               if (i < 5){
               i++;
               }
            }
            break;
            case SDL_SCANCODE_SPACE:
            digname = 0;
            gameover = 0;
            menu1 = 1;
            updateranking = 1;
            cameraRect.x = 0;
            cameraRect.y = 0;
            gotbonus1 = 0;
            break;
            }
         break;
         }
      }
      SDL_Color digcolor = {255, 255, 0};
      SDL_Rect digrect = {cameraRect.x + 400, cameraRect.y + 550, 0, 0};

      SDL_Surface *pointsurface2 = TTF_RenderText_Solid(font, textpoint, digcolor);
      SDL_Texture *texpoint2 = SDL_CreateTextureFromSurface(rend, pointsurface2);

      SDL_Surface *digsurface = TTF_RenderText_Solid(font, "DIGITE SEU NOME: ", digcolor);
      SDL_Texture *digtex = SDL_CreateTextureFromSurface(rend, digsurface);

      SDL_Surface *pnamesurface = TTF_RenderText_Solid(font, playername, digcolor);
      SDL_Texture *pnametex = SDL_CreateTextureFromSurface(rend, pnamesurface);

      SDL_FreeSurface(pointsurface2);
      SDL_FreeSurface(digsurface);
      SDL_FreeSurface(pnamesurface);

      //ler ranking
      fscanf(file, "%d", &pos[0]);
      fscanf(file, "%c", &best11);
      fscanf(file, "%c", &best12);
      fscanf(file, "%c", &best13);
      fscanf(file, "%c", &best14);
      fscanf(file, "%c", &best15);
      fscanf(file, "%c", &best16);
      fscanf(file, "%d", &bestscore[0]);

      fscanf(file, "%d", &pos[1]);
      fscanf(file, "%s", best2);
      fscanf(file, "%d", &bestscore[1]);

      fscanf(file, "%d", &pos[2]);
      fscanf(file, "%s", best3);
      fscanf(file, "%d", &bestscore[2]);

      fscanf(file, "%d", &pos[3]);
      fscanf(file, "%s", best4);
      fscanf(file, "%d", &bestscore[3]);

      SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
      SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);
      blit(texpoint2, digrect.x + 400, digrect.y, rend);
      blit(digtex, digrect.x, digrect.y, rend);
      blit(pnametex, digrect.x + 200, digrect.y, rend);
      SDL_RenderPresent(rend);
      }

   //atualizar ranking
   if (updateranking == 1){
      if (score > bestscore[0]){
         if (score < 1000){
         i = 3;
         }
         else if (score >= 1000 && score < 10000){
         i = 2;
         }
         else if (score >= 10000 && score < 100000){
         i = 1;
         }
         else {
         i = 0;
         }

      //2 para 3
      fseek(file, 32, SEEK_SET);
      fprintf(file, "%s", best2);
      fseek(file, 38 + i, SEEK_SET);
      fprintf(file, "%d", bestscore[1]);

      //3 para 4
      fseek(file, 47, SEEK_SET);
      fprintf(file, "%s", best3);
      fseek(file, 53 + i, SEEK_SET);
      fprintf(file, "%d", bestscore[2]);
      rewind(file);

      //1 para 2
      fseek(file, 17, SEEK_SET);
      fprintf(file, "%c", best12);
      fprintf(file, "%c", best13);
      fprintf(file, "%c", best14);
      fprintf(file, "%c", best15);
      fprintf(file, "%c", best16);
      fseek(file, 23 + i, SEEK_SET);
      fprintf(file, "%d", bestscore[0]);

      //novo para 1
      fseek(file, 2, SEEK_SET);
      fprintf(file, "%s", playername);
      fseek(file, 8 + i, SEEK_SET);
      fprintf(file, "%d", score);
      rewind(file);
      }
      else if (score > bestscore[1]){
         if (score < 1000){
         i = 3;
         }
         else if (score >= 1000 && score < 10000){
         i = 2;
         }
         else if (score >= 10000 && score < 100000){
         i = 1;
         }
         else {
         i = 0;
         }
      //3 para 4
      fseek(file, 47, SEEK_SET);
      fprintf(file, "%s", best3);
      fseek(file, 53 + i, SEEK_SET);
      fprintf(file, "%d", bestscore[2]);

      //2 para 3
      fseek(file, 32, SEEK_SET);
      fprintf(file, "%s", best2);
      fseek(file, 38 + i, SEEK_SET);
      fprintf(file, "%d", bestscore[1]);

      //novo para 2
      fseek(file, 17, SEEK_SET);
      fprintf(file, "%s", playername);
      fseek(file, 23 + i, SEEK_SET);
      fprintf(file, "%d", score);
      rewind(file);
      }

      else if (score > bestscore[2]){
         if (score < 1000){
         i = 3;
         }
         else if (score >= 1000 && score < 10000){
         i = 2;
         }
         else if (score >= 10000 && score < 100000){
         i = 1;
         }
         else {
         i = 0;
         }
      //3 para 4
      fseek(file, 47, SEEK_SET);
      fprintf(file, "%s", best3);
      fseek(file, 53 + i, SEEK_SET);
      fprintf(file, "%d", bestscore[2]);

      //novo para 3
      fseek(file, 32, SEEK_SET);
      fprintf(file, "%s", playername);
      fseek(file, 38 + i, SEEK_SET);
      fprintf(file, "%d", score);
      rewind(file);
      }

      else if (score > bestscore[3]){
         if (score < 1000){
         i = 3;
         }
         else if (score >= 1000 && score < 10000){
         i = 2;
         }
         else if (score >= 10000 && score < 100000){
         i = 1;
         }
         else {
         i = 0;
         }
      //novo para 4
      fseek(file, 47, SEEK_SET);
      fprintf(file, "%s", playername);
      fseek(file, 53 + i, SEEK_SET);
      fprintf(file, "%d", score);
      rewind(file);
      }

      for (i = 0; i < 5; i++){
      playername[i] = '.';
      }

   score = 0;
   updateranking = 0;
   }

   //carregar menu inicial
   int initscreen = 1;

   while (initscreen == 1){
   while (menu1 == 1){
   bgsurface = IMG_Load("menu11.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && menu1 == 1){
         switch (event.type){
         case SDL_QUIT:
         menu1 = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
            menu1 = 0;
            menu2 = 1;
            break;
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
            menu1 = 0;
            menu5 = 1;
            break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_SPACE:
            menu1 = menu2 = menu3 = menu4 = menu5 = cmenu = movmenu = histmenu = rankmenu = 0;
            bgsurface = IMG_Load("spacestage.png");
            bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

            SDL_FreeSurface(bgsurface);
            break;
            }
         }
      }
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   SDL_RenderPresent(rend);
   }

   while (menu2 == 1){
   bgsurface = IMG_Load("menu22.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && menu2 == 1){
         switch (event.type){
         case SDL_QUIT:
         menu2 = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
            menu2 = 0;
            menu1 = 1;
            break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
            menu2 = 0;
            menu3 = 1;
            break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_SPACE:
            menu2 = 0;
            histmenu = 1;
            break;
            }
         }
      }
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   SDL_RenderPresent(rend);
   }

   while (menu3 == 1){
   bgsurface = IMG_Load("menu33.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && menu3 == 1){
         switch (event.type){
         case SDL_QUIT:
         menu3 = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
            menu3 = 0;
            menu2 = 1;
            break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
            menu3 = 0;
            menu4 = 1;
            break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_SPACE:
            menu3 = 0;
            movmenu = 1;
            break;
            }
         }
      }
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   SDL_RenderPresent(rend);
   }

   while (menu4 == 1){
   bgsurface = IMG_Load("menu44.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && menu4 == 1){
         switch (event.type){
         case SDL_QUIT:
         menu4 = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
            menu4 = 0;
            menu3 = 1;
            break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
            menu4 = 0;
            menu5 = 1;
            break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_SPACE:
            menu4 = 0;
            rankmenu = 1;
            break;
            }
         }
      }
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   SDL_RenderPresent(rend);
   }

   while (menu5 == 1){
   bgsurface = IMG_Load("menu55.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && menu5 == 1){
         switch (event.type){
         case SDL_QUIT:
         menu5 = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
            menu5 = 0;
            menu4 = 1;
            break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
            menu5 = 0;
            menu1 = 1;
            break;
            case SDL_SCANCODE_J:
            case SDL_SCANCODE_SPACE:
            menu5 = 0;
            cmenu = 1;
            break;
            }
         }
      }
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   SDL_RenderPresent(rend);
   }

   while (movmenu == 1){
   bgsurface = IMG_Load("menumover.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && movmenu == 1){
         switch (event.type){
         case SDL_QUIT:
         movmenu = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_BACKSPACE:
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
            movmenu = 0;
            menu3 = 1;
            break;
            }
         }
      }
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   SDL_RenderPresent(rend);
   }

   while (cmenu == 1){
   bgsurface = IMG_Load("menucreditos.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && cmenu == 1){
         switch (event.type){
         case SDL_QUIT:
         cmenu = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_BACKSPACE:
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
            cmenu = 0;
            menu5 = 1;
            break;
            }
         }
      }
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   SDL_RenderPresent(rend);
   }

   while (histmenu == 1){
   bgsurface = IMG_Load("menuhistoria1.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && histmenu == 1){
         switch (event.type){
         case SDL_QUIT:
         histmenu = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_BACKSPACE:
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
            histmenu = 0;
            menu2 = 1;
            break;
            }
         }
      }
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   SDL_RenderPresent(rend);
   }

   while (rankmenu == 1){
   bgsurface = IMG_Load("menuranking.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);

      while (SDL_PollEvent(&event) && rankmenu == 1){
         switch (event.type){
         case SDL_QUIT:
         rankmenu = 0;
         close_requested = 1;
         break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_BACKSPACE:
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
            rankmenu = 0;
            menu4 = 1;
            break;
            }
         }
      }
   rewind(file);
   fscanf(file, "%d", &pos[0]);
   fscanf(file, "%s", best1);
   fscanf(file, "%d", &bestscore[0]);

   intTochar(bestscore[0], stringscore1);

   char bestp[5];
   for (i = 0; i < 5; i++){
   bestp[i] = best1[i];
   }

   fscanf(file, "%d", &pos[1]);
   fscanf(file, "%s", best2);
   fscanf(file, "%d", &bestscore[1]);

   intTochar(bestscore[1], stringscore2);

   fscanf(file, "%d", &pos[2]);
   fscanf(file, "%s", best3);
   fscanf(file, "%d", &bestscore[2]);

   intTochar(bestscore[2], stringscore3);

   fscanf(file, "%d", &pos[3]);
   fscanf(file, "%s", best4);
   fscanf(file, "%d", &bestscore[3]);

   intTochar(bestscore[3], stringscore4);

   SDL_Color yellow = {255, 255, 0};
   SDL_Rect rankrect = {cameraRect.x + 350, cameraRect.y, 0, 0};

   SDL_Surface *bestplayersurface = TTF_RenderText_Solid(font, bestp, yellow);
   SDL_Texture *bestplayertext = SDL_CreateTextureFromSurface(rend, bestplayersurface);
   SDL_Surface *rank1surface = TTF_RenderText_Solid(font, stringscore1, yellow);
   SDL_Texture *rank1text = SDL_CreateTextureFromSurface(rend, rank1surface);

   SDL_Surface *best2playersurface = TTF_RenderText_Solid(font, best2, yellow);
   SDL_Texture *best2playertext = SDL_CreateTextureFromSurface(rend, best2playersurface);
   SDL_Surface *rank2surface = TTF_RenderText_Solid(font, stringscore2, yellow);
   SDL_Texture *rank2text = SDL_CreateTextureFromSurface(rend, rank2surface);

   SDL_Surface *best3playersurface = TTF_RenderText_Solid(font, best3, yellow);
   SDL_Texture *best3playertext = SDL_CreateTextureFromSurface(rend, best3playersurface);
   SDL_Surface *rank3surface = TTF_RenderText_Solid(font, stringscore3, yellow);
   SDL_Texture *rank3text = SDL_CreateTextureFromSurface(rend, rank3surface);

   SDL_Surface *best4playersurface = TTF_RenderText_Solid(font, best4, yellow);
   SDL_Texture *best4playertext = SDL_CreateTextureFromSurface(rend, best4playersurface);
   SDL_Surface *rank4surface = TTF_RenderText_Solid(font, stringscore4, yellow);
   SDL_Texture *rank4text = SDL_CreateTextureFromSurface(rend, rank4surface);

   SDL_FreeSurface(bestplayersurface);
   SDL_FreeSurface(best2playersurface);
   SDL_FreeSurface(best3playersurface);
   SDL_FreeSurface(best4playersurface);
   SDL_FreeSurface(rank1surface);
   SDL_FreeSurface(rank2surface);
   SDL_FreeSurface(rank3surface);
   SDL_FreeSurface(rank4surface);

   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   blit(bestplayertext, rankrect.x, rankrect.y + 205, rend);
   blit(best2playertext, rankrect.x, rankrect.y + 242, rend);
   blit(best3playertext, rankrect.x, rankrect.y + 280, rend);
   blit(best4playertext, rankrect.x, rankrect.y + 320, rend);
   blit(rank1text, rankrect.x + 100, rankrect.y + 205, rend);
   blit(rank2text, rankrect.x + 100, rankrect.y + 242, rend);
   blit(rank3text, rankrect.x + 100, rankrect.y + 280, rend);
   blit(rank4text, rankrect.x + 100, rankrect.y + 320, rend);

   SDL_RenderPresent(rend);
   }

      if (menu1 == 0 && menu2 == 0 && menu3 == 0 && menu4 == 0 && menu5 == 0 && cmenu == 0 && movmenu == 0 && histmenu == 0 && rankmenu == 0){
      initscreen = 0;
      }
   }

   //determinar a velocidade
   x_vel = y_vel = 0;
   if (up && !down) y_vel = -SPEED;
   if (down && !up) y_vel = SPEED;
   if (left && !right) x_vel = -SPEED;
   if (right && !left) x_vel = SPEED;

   //atualizar posição
   x_pos += x_vel / 60;
   y_pos += y_vel / 60;

   //atualiar câmera
   cameraRect.x = x_pos - 120;
   cameraRect.y = y_pos - 240;

   if (cameraRect.x < 0){
   cameraRect.x = 0;
   }
   if (cameraRect.y < 0){
   cameraRect.y = 0;
   }

   if (cameraRect.x + cameraRect.w >= levelWidth){
   cameraRect.x = levelWidth - 200;
   }
   if (cameraRect.y + cameraRect.h >= levelHeight){
   cameraRect.y = levelHeight - 150;
   }

   //detectar colisão com a tela
   if (x_pos <= 0) x_pos = 0;
   if (y_pos <= 0) y_pos = 0;
   //if (x_pos >= WINDOW_WIDTH - playerdest.w) x_pos = WINDOW_WIDTH - playerdest.w;
   if (y_pos >= WINDOW_HEIGHT - playerdest.h) y_pos = WINDOW_HEIGHT - playerdest.h;

   playerdest.y = (int) y_pos;
   playerdest.x = (int) x_pos;

   //determinar posição do inimigo 1
   if (qenemies == 0){
   enemy.x = 850;
   enemy.y = 150;
   enemy.dx = -5;
   enemy.dy = 0;
   enemy.health = 1;
   }

   if (enemy.health == 1){
   enemy.x += enemy.dx;
   enemy.y += enemy.dy;
   }

   //detectar colisão com inimigo 1
   float distancex, distancey;

   if (x_pos > enemy.x){
   distancex = x_pos - enemy.x;
   }
   else{
   distancex = enemy.x - x_pos;
   }
   if (y_pos > enemy.y){
   distancey = y_pos - enemy.y;
   }
   else{
   distancey = enemy.y - y_pos;
   }

   if (distancex < 40 && distancey < 40 && enemy.health == 1 && gameover == 0){ //redefinir
   //printf("VOCÊ FOI ATINGIDO!\n");
   gameover = 1;
   time = 0;
   }

   //determinar se o inimigo 1 foi atingido
   float distancebx, distanceby;

   if (enemy.health == 1){
      if (enemy.x > bullet.x){
      distancebx = enemy.x - bullet.x;
      }
      else{
      distancebx = bullet.x - enemy.x;
      }
      if (enemy.y > bullet.y){
      distanceby = enemy.y - bullet.y;
      }
      else{
      distanceby = bullet.y - enemy.y;
      }

      if (distancebx < 40 && distanceby < 40){ //refedinir distância mínima para colisão
      enemy.health = 0;
      bullet.health = 0;
      score += 100;
      }
   }

   //iniciar atributos dos inimigos da segunda horda   [stack smashing]
   if (enemy.health == 0 || enemy.x <= cameraRect.x - 180){
      if (qenemies < 5){
         for (i = 0; i < 4; i++){
         enemies[i].x = cameraRect.x + 950;
         //enemies[i].y = (rand() % (WINDOW_HEIGHT / 2)) + 10;
         enemies[i].dx = -5;
         enemies[i].dy = 0;
         enemies[i].health = 1;

         qenemies += 1;
         }
      }
   }

   enemies[0].y = 380;
   enemies[1].y = 220;
   enemies[2].y = 400;
   enemies[3].y = 300;

   //atualizar inimigos da segunda horda
   enemiesupdate(enemies, 4);

   //determinar se os inimigos da segunda horda foram atingidos
   hitenemies(enemies, bullet, 4);

   //verificar colisão entre player e inimigos da segunda horda
   playercollision(enemies, 4);

   //inicializar inimigos da terceira horda
   if (cameraRect.x >= 600){
      if (qenemies < 6){
         for (i = 0; i < 2; i++){
         shotenemies[i].x = 1900;
         //enemies[i].y = (rand() % (WINDOW_HEIGHT / 2)) + 10;
         shotenemies[i].dx = -10;
         if (i == 0){
         shotenemies[i].y = 300;
         shotenemies[i].dy = 0.5;
         }
         else{
         shotenemies[i].y = 300;
         shotenemies[i].dy = -0.5;
         }
         shotenemies[i].health = 1;

         qenemies += 1;
         }
      }
   }

   //atualizar terceira horda de inimigos
   enemiesupdate(shotenemies, 2);

   //determinar se os inimigos da terceira horda foram atingidos
   hitenemies(shotenemies, bullet, 2);

   //verificar colisão entre player e inimigos da terceira horda
   playercollision(shotenemies, 2);

   //inicializar inimigos da quarta horda
   if (cameraRect.x >= 1500){
      if (qenemies < 8){
         for (i = 0; i < 3; i++){
         enemyg[i].x = 2500;
         enemyg[i].dx = -10;
         if (i == 0){
         enemyg[i].y = 300;
         enemyg[i].dy = 0.5;
         }
         else if (i == 1){
         enemyg[i].y = 250;
         enemyg[i].dy = -0.5;
         }
         else if (i == 2){
         enemyg[i].y = 400;
         enemyg[i].dy = 0;
         }
         else{
         enemyg[i].y = 100;
         enemyg[i].dy = -2;
         }
         enemyg[i].health = 1;

         qenemies += 1;
         }
      }
   }

   //atualizar quarta horda de inimigos
   enemiesupdate(enemyg, 3);

   //determinar se os inimigos da quarta horda foram atingidos
   hitenemies(enemyg, bullet, 3);

   //verificar colisão entre player e inimigos da quarta horda
   playercollision(enemyg, 3);

   //inicializar inimigos da quinta horda
   if (cameraRect.x >= 2000){
      if (qenemies < 11){
         for (i = 0; i < 3; i++){
         penemy[i].x = 4000;
         //enemies[i].y = (rand() % (WINDOW_HEIGHT / 2)) + 10;
         penemy[i].dx = -5;
         if (i == 0){
         penemy[i].y = 300;
         penemy[i].dy = 0.5;
         }
         else if (i == 1){
         penemy[i].y = 300;
         penemy[i].dy = -0.5;
         }
         else{
         penemy[i].y = 500;
         penemy[i].dy = 0;
         }
         penemy[i].health = 1;
         pbullets[i].health = 0;

         qenemies += 1;
         //printf("quantidade de inimigos: %d\n", qenemies);
         }
      }
   }

   //atualizar quinta horda de inimigos
   enemiesupdate(penemy, 3);

   //determinar se os inimigos da quinta horda foram atingidos
   hitenemies(penemy, bullet, 3);

   //verificar colisão entre player e inimigos da quinta horda
   playercollision(penemy, 3);

   //disparos da quinta horda
   for (i = 0; i < 3; i++){
      if (pbullets[i].health == 0){
      //printf("O INIMIGO ESTÁ DISPARANDO!\n");
      pbullets[i].x = penemy[i].x - 10;
      pbullets[i].y = penemy[i].y + 40;
      pbullets[i].dx = -15;
         if (i % 2 == 0){
         pbullets[i].dy = 0.5;
         }
         else{
         pbullets[i].dy = -0.5;
         }
      pbullets[i].health = 1;
      }

   pbullets[i].x += pbullets[i].dx;
   pbullets[i].y += pbullets[i].dy;

      if (pbullets[i].x < cameraRect.x - 300){
      pbullets[i].health = 0;
      }
   }

   //verificar se o player foi atingido pelos disparos
   hitplayer(pbullets, penemy, 3);

   //inicializar inimigos da sexta horda
   if (cameraRect.x >= 3000){
      if (qenemies < 14){
         for (i = 0; i < 5; i++){
         penemy2[i].x = 4000;
         penemy2[i].dx = 0;
         if (i == 0){
         penemy2[i].y = 300;
         penemy2[i].dy = 2;
         }
         else if (i == 1){
         penemy2[i].y = 250;
         penemy2[i].dy = -2;
         }
         else if (i == 2){
         penemy2[i].y = 400;
         penemy2[i].dy = 1;
         }
         else if (i == 3){
         penemy2[i].y = 600;
         penemy2[i].dy = -1;
         }
         else{
         penemy2[i].y = 100;
         penemy2[i].dy = 1;
         }
         penemy2[i].health = 1;
         pbullets2[i].health = 0;

         qenemies += 1;
         //printf("quantidade de inimigos: %d\n", qenemies);
         }
      }
   }

   //atualizar sexta horda de inimigos
   enemiesupdate(penemy2, 5);
   for (i = 0; i < 5; i++){
      if (penemy2[i].y >= 500){
      penemy2[i].dy = -3;
      }
      if (penemy2[i].y < 0){
      penemy2[i].dy = 3;
      }
   }

   //fixar camera durante a sexta horda
   if (penemy2[0].health == 1 || penemy2[1].health == 1 || penemy2[2].health == 1 || penemy2[3].health == 1 || penemy2[4].health == 1){
      if (x_pos >= 3800){
      x_pos = 3795;
      }
   }

   //determinar se os inimigos da sexta horda foram atingidos
   hitenemies(penemy2, bullet, 5);

   //verificar colisão entre player e inimigos da sexta horda
   playercollision(penemy2, 5);

   //disparos da sexta horda
   for (i = 0; i < 5; i++){
      if (pbullets2[i].health == 0){
      //printf("O INIMIGO ESTÁ DISPARANDO!\n");
      pbullets2[i].x = penemy2[i].x - 10;
      pbullets2[i].y = penemy2[i].y + 40;
      pbullets2[i].dx = -15;
         if (i % 2 == 0){
         pbullets2[i].dy = 0.5;
         }
         else{
         pbullets2[i].dy = -0.5;
         }
      pbullets2[i].health = 1;
      }

   pbullets2[i].x += pbullets2[i].dx;
   pbullets2[i].y += pbullets2[i].dy;

      if (pbullets2[i].x < cameraRect.x - 300){
      pbullets2[i].health = 0;
      }
   }

   //verificar se o jogador foi atingido pelos disparos
   hitplayer(pbullets2, penemy2, 5);

   //inicializar inimigos da sétima horda
   ENTITY enemy7[4];
   if (cameraRect.x >= 4000){
      if (qenemies < 19){
      controler = 1;
      }
      else{
      controler = 0;
      }
   spawnenemies(enemy7, 4, "greenenemy.png", 5000, controler, rend);
   }

   //atualizar inimigos da sétima horda
   enemiesupdate(enemy7, 4);

   //verificar se os inimigos da sétima horda foram atingidos
   hitenemies(enemy7, bullet, 4);

   //verificar colisão entre player e inimigos da sétima horda
   playercollision(enemy7, 4);

   //inicializar boss 1
   if (cameraRect.x >= 4800 && qenemies < 23){
   boss.x = 5500;
   boss.y = 400;
   boss.dx = 0;
   boss.dy = 6;
   boss.health = 100;
      for (i = 0; i < 4; i++){
      bossbullets[i].health = 0;
      }
   qenemies += 1;
   }

   //atualizar boss 1
   if (boss.health > 0 && qenemies == 23){
   boss.x += boss.dx;
   boss.y += boss.dy;
      if (boss.y >= 500){
      boss.dy = -6;
      }
      if (boss.y < 0){
      boss.dy = 6;
      }
   }

   //fixar camera durante boss 1
   if (boss.health > 0 && qenemies == 23){
      if (x_pos >= boss.x + 5){
      x_pos = boss.x - 3;
      }
   }

   //determinar se boss 1 foi atingido
   hitboss(&boss, &bullet);
   if (boss.health == 0 && qenemies == 23 && bossdefeated == 0){
   score += 2000;
   bossdefeated = 1;
   }
   if (bossdefeated == 1 && x_pos >= 1000){
   gameover = 0;
   time = 0;
   }

   //verificar colisão entre player e boss 1
   playerbosscollision(boss);

   //disparos do boss 1
   for (i = 0; i < 4; i++){
      if (bossbullets[i].health == 0){
      bossbullets[i].x = boss.x;
      bossbullets[i].dx = -12;
         if (i % 2 == 0){
         bossbullets[i].y = boss.y;
         }
         else{
         bossbullets[i].y = boss.y + 5;
         }
         if (i == 0){
         bossbullets[i].dy = 1;
         }
         else if (i == 1){
         bossbullets[i].dy = -2;
         }
         else if (i == 2){
         bossbullets[i].dy = 4;
         }
         else{
         bossbullets[i].dy = -3;
         }
      bossbullets[i].health = 1;
      }

   bossbullets[i].x += bossbullets[i].dx;
   bossbullets[i].y += bossbullets[i].dy;

      if (bossbullets[i].x < cameraRect.x - 100){
      bossbullets[i].health = 0;
      }
   }

   //verificar se o jogador foi atingido pelos disparos
   hitbossplayer(bossbullets, boss, 4);

   //verificar disparos do jogador
   if (fire == 1 && bullet.health == 0){
   bullet.x = playerdest.x + 70;
   bullet.y = playerdest.y + 38;
   bullet.dx = 27;
   //verificar bonus
      if (bonus.health == 0 && gotbonus1 == 1){
      bullet.dx = 50;
      }
   bullet.dy = 0;
   bullet.health = 1;
   }

   bullet.x += bullet.dx;
   bullet.y += bullet.dy;

   if (bullet.x > cameraRect.x + cameraRect.w + 400){
   bullet.health = 0;
   }

   //lançar bonus
   if (enemies[1].health == 0 && qbonus == 0){
   spawnbonus(&bonus, 980, 100);
   }

   //atualizar bonus
   updatebonus(&bonus, &gotbonus1);

   //desenhar background e player
   if (!gameover){
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);

   //ajustar posição do player com a câmera
   SDL_Rect drawingRect = {playerdest.x - cameraRect.x, playerdest.y - cameraRect.y, playerdest.w, playerdest.h};
   //printf("posição do jogador = x: %d, y: %d\n", drawingRect.x, drawingRect.y);
   //printf("posição da câmera = x: %d, y: %d\n", cameraRect.x, cameraRect.y);
   SDL_RenderCopy(rend, playertex, NULL, &drawingRect);
   }
   else{
   bgsurface = IMG_Load("gameover.png");
   bgtex = SDL_CreateTextureFromSurface(rend, bgsurface);
   SDL_QueryTexture(bgtex, NULL, NULL, &levelWidth, &levelHeight);
   SDL_RenderCopy(rend, bgtex, &cameraRect, NULL);
   }

   //desenhar inimigo 1
   if (enemy.health == 1 && gameover == 0)
   blit(enemy.texture, enemy.x - cameraRect.x, enemy.y - cameraRect.y, rend);
   if (qenemies == 0){
   //printf("SURGIU UM INIMIGO!\n");
   qenemies = 1;
   }

   //desenhar segunda horda de inimigos
   if (qenemies > 1 && gameover == 0){
      for (i = 0; i < 4; i++){
         if (enemies[i].health == 1){
         blit(enemies[i].texture, enemies[i].x - cameraRect.x, enemies[i].y - cameraRect.y, rend);
         //printf("renderizando inimigo %d na posição x: %d, y: %d\n", i, enemies[i].x, enemies[i].y);
         }
      }
   }

   //desenhar terceira horda de inimigos
   if (qenemies > 5 && gameover == 0){
      for (i = 0; i < 2; i++){
         if (shotenemies[i].health == 1){
         blit(shotenemies[i].texture, shotenemies[i].x - cameraRect.x, shotenemies[i].y - cameraRect.y, rend);
         }
      }
   }

   //desenhar quarta horda de inimigos
   if (qenemies > 6 && gameover == 0){
      for (i = 0; i < 3; i++){
         if (enemyg[i].health == 1){
         blit(enemyg[i].texture, enemyg[i].x - cameraRect.x, enemyg[i].y - cameraRect.y, rend);
         }
      }
   }

   //desenhar quinta horda de inimigos
   if (qenemies > 10 && gameover == 0){
      for (i = 0; i < 3; i++){
         if (penemy[i].health == 1){
         blit(penemy[i].texture, penemy[i].x - cameraRect.x, penemy[i].y - cameraRect.y, rend);
         }
      }
   }

   //desenhar bullets da quinta horda
   for (i = 0; i < 3; i++){
      if (pbullets[i].health > 0 && penemy[i].health == 1 && !gameover){
      blit(pbullets[i].texture, pbullets[i].x - cameraRect.x, pbullets[i].y - cameraRect.y, rend);
      //printf("bullets inimigas detectadas na posição = x: %d, y: %d\n", pbullets[i].x - cameraRect.x, pbullets[i].y - cameraRect.y);
      }
   }

   //desenhar sexta horda de inimigos
   if (qenemies > 11 && gameover == 0){
      for (i = 0; i < 5; i++){
         if (penemy2[i].health > 0){
         blit(penemy2[i].texture, penemy2[i].x - cameraRect.x, penemy2[i].y - cameraRect.y, rend);
         }
      }
   }

   //desenhar bullets da sexta horda
   for (i = 0; i < 5; i++){
      if (pbullets2[i].health > 0 && penemy2[i].health == 1 && !gameover){
      blit(pbullets2[i].texture, pbullets2[i].x - cameraRect.x, pbullets2[i].y - cameraRect.y, rend);
      }
   }

   //desenhar sétima horda de inimigos
   for (i = 0; i < 4; i++){
      if (enemy7[i].health == 1){
      blit(enemy7[i].texture, enemy7[i].x - cameraRect.x, enemy7[i].y - cameraRect.y, rend);
      }
   }

   //desenhar boss 1
   if (boss.health > 0 && qenemies == 23 && !gameover){
   blit(boss.texture, boss.x - cameraRect.x, boss.y - cameraRect.y, rend);
   }

   //desenhar bullets do boss 1
   for (i = 0; i < 4; i++){
      if (bossbullets[i].health > 0 && boss.health > 0 && !gameover){
      blit(bossbullets[i].texture, bossbullets[i].x - cameraRect.x, bossbullets[i].y - cameraRect.y, rend);
      }
   }

   //desenhar bullets
   if (bonus.health == 0 && gotbonus1 == 1){
   bullet.surface = IMG_Load("bulletbonus.png");
   bullet.texture = SDL_CreateTextureFromSurface(rend, bullet.surface);
   }
   else{
   bullet.surface = IMG_Load("bullet12.png");
   bullet.texture = SDL_CreateTextureFromSurface(rend, bullet.surface);
   }
   if (bullet.health > 0 && !gameover){
   blit(bullet.texture, bullet.x - cameraRect.x, bullet.y - cameraRect.y, rend);
   //printf("posição da bullet: %d\n", bullet.x - cameraRect.x);
   }

   //desenhar bonus
   if (bonus.health == 1 && !gameover){
   blit(bonus.texture, bonus.x - cameraRect.x, bonus.y - cameraRect.y, rend);
   }

   //desenhar pontuação
   textsurface = TTF_RenderText_Solid(font, "pontos: ", fontcolor);
   text = SDL_CreateTextureFromSurface(rend, textsurface);

   //char textpoint[20];
   intTochar(score, textpoint);

   pointsurface = TTF_RenderText_Solid(font, textpoint, fontcolor);
   points = SDL_CreateTextureFromSurface(rend, pointsurface);

   if (!gameover){
   blit(text, localtext.x, localtext.y, rend);
   blit(points, localtext.x + 70, localtext.y, rend);
   }

   if (!gameover && initscreen == 0 && close_requested == 0){
   SDL_RenderPresent(rend);
   }
   else{
      if (time >= 120 && initscreen == 0 && close_requested == 0){
      SDL_RenderPresent(rend);
         if (time >= 240){
         digname = 1;

         x_pos = 50;
         y_pos = 150;
         qenemies = 0;
         qbonus = 0;
         bossdefeated = 0;
         boss.health = 0;
         enemy.health = 0;
            for (i = 0; i < 4; i++){
            enemies[i].health = 0;
            }
            for (i = 0; i < 2; i++){
            shotenemies[i].health = 0;
            }
            for (i = 0; i < 3; i++){
            enemyg[i].health = 0;
            penemy[i].health = 0;
            }
            for (i = 0; i < 5; i++){
            penemy2[i].health = 0;
            }
            for (i = 0; i < 4; i++){
            enemy7[i].health = 0;
            }
         }
      }
   }
   SDL_Delay(1000 / 60);

   //atualizar tempo
   time += 1;
   if (time >= 1000000000)time = 0;

   //limpar a janela
   SDL_RenderClear(rend);
   }

   fclose(file);
   SDL_FreeSurface(textsurface);
   SDL_FreeSurface(pointsurface);
   TTF_CloseFont(font);
   SDL_DestroyTexture(bgtex);
   SDL_DestroyTexture(playertex);
   SDL_DestroyTexture(bullet.texture);
   SDL_DestroyTexture(enemy.texture);
   for (i = 0; i < 4; i++){
   SDL_DestroyTexture(enemies[i].texture);
   }
   for (i = 0; i < 2; i++){
   SDL_DestroyTexture(shotenemies[i].texture);
   }
   for (i = 0; i < 3; i++){
   SDL_DestroyTexture(enemyg[i].texture);
   SDL_DestroyTexture(penemy[i].texture);
   SDL_DestroyTexture(pbullets[i].texture);
   }
   for (i = 0; i < 5; i++){
   SDL_DestroyTexture(penemy2[i].texture);
   SDL_DestroyTexture(pbullets2[i].texture);
   }
   SDL_DestroyTexture(boss.texture);
   for (i = 0; i < 4; i++){
   SDL_DestroyTexture(bossbullets[i].texture);
   }
   SDL_DestroyTexture(bonus.texture);
   SDL_DestroyRenderer(rend);
   SDL_DestroyWindow(win);
   TTF_Quit();

   SDL_Quit();

   return 0;
}
