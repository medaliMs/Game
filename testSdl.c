#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define WINDOW_WIDTH 1280
#define WINDOW_LENGTH 960
#define SPEED_MOUVEMENT 5
#define GRAVITY 0.4f

typedef struct
{
	float x;
	float y;
	float dy;
	int dx;
	int onLedge;
} Player;

typedef struct
{
	int x, y, w, h, dx, dy;
} Box;

typedef struct
{
	int x, y, w, h, dx, dy;
} Trap;

typedef struct
{
	int x, y, w, h, dx, dy, eaten;
} Item;

typedef struct
{
	Player player;
	Box boxes[23];
	Trap trap[10];
	Item apple[15];
	SDL_Renderer *renderer;
	SDL_Texture *background;
	SDL_Texture *item;
	SDL_Texture *playerAnimation[3];
	SDL_Texture *brick;
	SDL_Texture *traps[3];
	SDL_Texture *label[3];
	SDL_Texture *score;
	int points;
	int time;
	TTF_Font *font;

} Game;

int nbApples = 15;
int applesXPositions[] = {10, 120, 170, 320, 330, 390, 450, 460, 490, 510, 550, 600, 680, 690, 750};
int applesYPositions[] = {25, 75, 130, 175, 190, 780, 452, 125, 50, 190, 550, 170, 10, 100, 140};

int processEvents(Game *game, int close);
void collisionDetect(Game *game);

int cnt = 0;

int main(int argc, char *argv[])
{
	Game game;
	srand((int)time(NULL));
	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window *win = SDL_CreateWindow("GAME", // creates a window
									   SDL_WINDOWPOS_CENTERED,
									   SDL_WINDOWPOS_CENTERED,
									   WINDOW_WIDTH, WINDOW_LENGTH, 0);

	// triggers the program that controls
	// your graphics hardware and sets flags
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

	// creates a renderer to render our images
	SDL_Renderer *render = SDL_CreateRenderer(win, -1, render_flags);

	game.renderer = render;
	TTF_Init();
	game.font = TTF_OpenFont("./fonts/font.ttf", 48);
	// creates a surface to load an image into the main memory
	SDL_Surface *surface;

	// please provide a path for your image
	surface = IMG_Load("./assets/blue.PNG");
	game.background = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = IMG_Load("./assets/Apple.PNG");
	game.item = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = IMG_Load("./assets/playerJump.PNG");
	game.playerAnimation[2] = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = IMG_Load("./assets/playerIdle.PNG");
	game.playerAnimation[0] = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = IMG_Load("./assets/playerRun.PNG");
	game.playerAnimation[1] = SDL_CreateTextureFromSurface(game.renderer, surface);

	// traps

	surface = IMG_Load("./assets/spikeTrap.PNG");
	game.traps[0] = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = IMG_Load("./assets/spikeBallTrap.PNG");
	game.traps[1] = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = IMG_Load("./assets/spikeHeadTrap.PNG");
	game.traps[2] = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = TTF_RenderText_Solid(game.font, "Hello!", (SDL_Color){255, 255, 255, 255});
	game.label[0] = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = TTF_RenderText_Solid(game.font, "Collect all Apples", (SDL_Color){255, 0, 0, 255});
	game.label[1] = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = TTF_RenderText_Solid(game.font, "Without touching traps!", (SDL_Color){255, 0, 0, 255});
	game.label[2] = SDL_CreateTextureFromSurface(game.renderer, surface);

	surface = IMG_Load("./assets/Box.PNG");
	game.brick = SDL_CreateTextureFromSurface(game.renderer, surface);

	// clears main-memory

	game.points = 0;
	Player player1 = {200, 200, 0, 0, false};
	game.player = player1;
	game.time = 0;
	for (int i = 0; i < 20; i++)
	{
		game.boxes[i].w = 64;
		game.boxes[i].h = 64;
		game.boxes[i].x = i * 64;
		game.boxes[i].y = WINDOW_LENGTH - 64;
		game.boxes[i].dx = 0;
		game.boxes[i].dy = 0;
	}

	for (int i = 0; i < 20; i++)
	{
		if (i == 12 || i == 13)
			continue;
		game.boxes[i].w = 64;
		game.boxes[i].h = 64;
		game.boxes[i].x = i * 64;
		game.boxes[i].y = WINDOW_LENGTH - 64;
		game.boxes[i].dx = 0;
		game.boxes[i].dy = 0;
	}

	for (int i = 0; i < 5; i++)
	{
		game.trap[i].w = 64;
		game.trap[i].h = 64;
		game.trap[i].x = WINDOW_WIDTH - ((i + 1) * 64);
		game.trap[i].y = WINDOW_LENGTH - 128;
		game.trap[i].dx = 0;
		game.trap[i].dy = 0;
	}
	for (int i = 5; i < 7; i++)
	{
		game.trap[i].w = 64;
		game.trap[i].h = 64;
		game.trap[i].x = (i * 128);
		game.trap[i].y = WINDOW_LENGTH - (i - 3) * 128;
		game.trap[i].dx = i;
		game.trap[i].dy = 0;
	}
	/*for(int i = 7; i < 10; i++)
	{
		game.trap[i].w = 64;
		game.trap[i].h = 64;
		game.trap[i].x = (i*128);
		game.trap[i].y = WINDOW_LENGTH-128;
		game.trap[i].dx = 0;
		game.trap[i].dy = 0;
	}*/

	for (int i = 0; i < nbApples; i++)
	{
		game.apple[i].w = 16;
		game.apple[i].h = 16;
		game.apple[i].x = applesXPositions[i];
		game.apple[i].y = applesYPositions[i];
		game.apple[i].dx = 0;
		game.apple[i].dy = 0;
		game.apple[i].eaten = 0;
	}

	game.boxes[12].w = 64;
	game.boxes[12].h = 64;
	game.boxes[12].x = 250;
	game.boxes[12].y = WINDOW_LENGTH - 564;
	game.boxes[12].dx = -2;
	game.boxes[12].dy = 0;

	game.boxes[13].w = 64;
	game.boxes[13].h = 64;
	game.boxes[13].x = 250;
	game.boxes[13].y = WINDOW_LENGTH - 728;
	game.boxes[13].dx = 4;
	game.boxes[13].dy = 0;

	game.boxes[20].w = 64;
	game.boxes[20].h = 64;
	game.boxes[20].x = 250;
	game.boxes[20].y = WINDOW_LENGTH - 400;
	game.boxes[20].dx = 3;
	game.boxes[20].dy = 0;

	game.boxes[21].w = 64;
	game.boxes[21].h = 64;
	game.boxes[21].x = 250;
	game.boxes[21].y = WINDOW_LENGTH - 850;
	game.boxes[21].dx = 3;
	game.boxes[21].dy = 0;

	game.boxes[22].w = 64;
	game.boxes[22].h = 64;
	game.boxes[22].x = 250;
	game.boxes[22].y = WINDOW_LENGTH - 220;
	game.boxes[22].dx = -3;
	game.boxes[22].dy = 0;

	// controls animation loop
	int close = 0;

	// animation loop

	int textWidth, textHeight;

	// SDL_RenderClear(game.renderer);
	for (int i = 0; i < 3; i++)
	{
		SDL_QueryTexture(game.label[i], NULL, NULL, &textWidth, &textHeight);

		// Clear the renderer

		// Render the texture to the renderer
		SDL_Rect destRects = {100, 100 * (i + 1), textWidth, textHeight};
		SDL_RenderCopy(game.renderer, game.label[i], NULL, &destRects);

		// Present the renderer
		SDL_RenderPresent(game.renderer);
	}
	SDL_RenderClear(game.renderer);

	// Delay for a few seconds
	SDL_Delay(3000);

	while (!close)
	{
		game.time++;
		char text[20] = "Score: 0";
		int cnt = 0;
		game.player.dy += GRAVITY;
		game.player.y += game.player.dy;
		close = processEvents(&game, close);
		if (!close)
		{
			collisionDetect(&game);
			// clears the screen
			SDL_RenderClear(game.renderer);

			// Prepare the player box
			SDL_Rect player = {game.player.x, game.player.y, 64, 64};

			// Render the background
			for (int i = 0; i <= WINDOW_WIDTH; i += 64)
			{
				for (int j = 0; j <= WINDOW_LENGTH; j += 64)
				{
					SDL_Rect back = {i, j, 64, 64};
					SDL_RenderCopy(game.renderer, game.background, NULL, &back);
				}
			}

			// Render the boxes
			for (int i = 0; i < 23; i++)
			{
				game.boxes[i].x = (game.boxes[i].x + game.boxes[i].dx + WINDOW_WIDTH) % WINDOW_WIDTH;
				game.boxes[i].y = (game.boxes[i].y + game.boxes[i].dy + WINDOW_WIDTH) % WINDOW_WIDTH;
				SDL_Rect boxRect = {game.boxes[i].x,
									game.boxes[i].y,
									game.boxes[i].w,
									game.boxes[i].h};
				SDL_RenderCopy(game.renderer, game.brick, NULL, &boxRect);
			}

			// Render the non eaten apples
			for (int i = 0; i < 15; i++)
			{
				if (!game.apple[i].eaten)
				{
					SDL_Rect items = {game.apple[i].x, game.apple[i].y, game.apple[i].w, game.apple[i].h};
					SDL_RenderCopy(game.renderer, game.item, NULL, &items);
					// Check for any eaten apples to update the score
					bool collusion = SDL_HasIntersection(&items, &player);
					if (collusion)
					{
						game.apple[i].eaten = 1;
						game.points += 10;
					}
				}
			}

			// Render the traps
			for (int i = 0; i < 7; i++)
			{
				int cnt = 0;
				if (i == 2 || i == 3)
				{
					if (game.time > 120 && game.time < 240)
					{
						continue;
					}
				}
				game.trap[i].x = (game.trap[i].x + game.trap[i].dx + WINDOW_WIDTH) % WINDOW_WIDTH;
				game.trap[i].y = (game.trap[i].y + game.trap[i].dy + WINDOW_WIDTH) % WINDOW_WIDTH;
				SDL_Rect trapBox = {game.trap[i].x,
									game.trap[i].y,
									game.trap[i].w,
									game.trap[i].h};
				if (i >= 5)
					cnt = 1;
				bool collusion = SDL_HasIntersection(&trapBox, &player);
				if (collusion)
					close = 1;
				SDL_RenderCopy(game.renderer, game.traps[cnt], NULL, &trapBox);
			}

			// Render the player 
			if (!game.player.onLedge)
				cnt = 2;
			/*else{ if (game.time%60==0){
				if(cnt==0 && game.player.dx !=0){
					printf("cnt:%d and game.player.dx:%d\n", cnt, game.player.dx);
				  cnt =1;
				} else {
					cnt=0;
				}
			}}*/

			SDL_RenderCopy(game.renderer, game.playerAnimation[cnt], NULL, &player);
			game.player.dx = 0;

			// calculates to 60 fps
			// SDL_Delay(1000 / 60);
			// Restart timer
			if (game.time > 500)
				game.time = 0;
			
			// Update the score
			snprintf(text, sizeof(text), "Score: %d", game.points);
			surface = TTF_RenderText_Solid(game.font, text, (SDL_Color){255, 0, 0, 255});
			game.score = SDL_CreateTextureFromSurface(game.renderer, surface);

			SDL_Rect destRect = {0, 0, surface->w, surface->h};
			SDL_RenderCopy(game.renderer, game.score, NULL, &destRect);
			SDL_RenderPresent(game.renderer);
		}
		if (close)
		{
			SDL_Delay(2000);
			char result_msg[] = "YOU LOSE!";
			if (game.points >= 100)
			{
				memcpy(result_msg, "YOU WIN!", 8);
			}
			SDL_RenderClear(game.renderer);
			surface = TTF_RenderText_Solid(game.font, result_msg, (SDL_Color){255, 0, 0, 255});
			SDL_Texture *result;
			result = SDL_CreateTextureFromSurface(game.renderer, surface);
			SDL_QueryTexture(result, NULL, NULL, &textWidth, &textHeight);

			SDL_Rect destRect = {150, 150, textWidth, textHeight};
			SDL_RenderCopy(game.renderer, result, NULL, &destRect);
			SDL_RenderPresent(game.renderer);
			SDL_Delay(2000);
		}
	}

	SDL_FreeSurface(surface);
	// destroy texture
	SDL_DestroyTexture(game.background);

	SDL_DestroyTexture(game.brick);

	SDL_DestroyTexture(game.playerAnimation[0]);
	SDL_DestroyTexture(game.playerAnimation[1]);
	SDL_DestroyTexture(game.playerAnimation[2]);

	SDL_DestroyTexture(game.traps[0]);
	SDL_DestroyTexture(game.traps[1]);
	SDL_DestroyTexture(game.traps[2]);

	SDL_DestroyTexture(game.label[0]);
	SDL_DestroyTexture(game.label[1]);
	SDL_DestroyTexture(game.label[2]);

	SDL_DestroyTexture(game.item);

	TTF_CloseFont(game.font);

	// destroy renderer
	SDL_DestroyRenderer(game.renderer);

	// destroy window
	SDL_DestroyWindow(win);

	TTF_Quit();

	// close SDL
	SDL_Quit();

	return 0;
}

int processEvents(Game *game, int close)
{
	SDL_Event event;
	// Events management
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{

		case SDL_QUIT:
			// handling of close button
			close = 1;
			break;

		case SDL_KEYDOWN:
			// keyboard API for key pressed
			switch (event.key.keysym.scancode)
			{
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_UP:
				if (game->player.onLedge)
				{
					game->player.dy = -8;
					game->player.onLedge = false;
				}
				break;
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_LEFT:
				game->player.x -= SPEED_MOUVEMENT;
				game->player.dx = -1;
				break;
			case SDL_SCANCODE_D:
			case SDL_SCANCODE_RIGHT:
				game->player.x += SPEED_MOUVEMENT;
				game->player.dx = 1;
				break;
			case SDL_SCANCODE_ESCAPE:
				close = 1;
				break;
			default:
				break;
			}
		}
	}
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_LEFT])
	{
		game->player.x -= SPEED_MOUVEMENT;
	}
	if (state[SDL_SCANCODE_RIGHT])
	{
		game->player.x += SPEED_MOUVEMENT;
	}
	if (state[SDL_SCANCODE_UP])
	{
		game->player.y -= SPEED_MOUVEMENT;
	}
	// right boundary
	if (game->player.x + 64 > WINDOW_WIDTH)
		game->player.x = WINDOW_WIDTH - 64;

	// left boundary
	if (game->player.x < 0)
		game->player.x = 0;

	// bottom boundary
	if (game->player.y + 64 > WINDOW_LENGTH)
		close = 1;
	// upper boundary
	if (game->player.y < 0)
		game->player.y = 0;

	return close;
}

void collisionDetect(Game *game)
{
	// Check for collision with any boxes (brick blocks)
	for (int i = 0; i < 23; i++)
	{
		float mw = 64;
		float mh = 64;

		float mx = game->player.x;
		float my = game->player.y;

		float bx = game->boxes[i].x;
		float by = game->boxes[i].y;
		float bw = game->boxes[i].w;
		float bh = game->boxes[i].h;

		if (mx + mw / 2 > bx && mx + mw / 2 < bx + bw)
		{

			// are we bumping our head?
			if (my < by + bh && my > by && game->player.dy < 0)
			{
				// correct y
				game->player.y = by + bh;
				my = by + bh;

				// bumped our head, stop any jump velocity
				game->player.dy = 0;
				game->player.onLedge = true;
			}
		}
		if (mx + mw > bx && mx < bx + bw)
		{

			// are we landing on the ledge
			if (my + mh > by && my < by && game->player.dy > 0)
			{
				// correct y
				game->player.y = by - mh;
				game->player.x += game->boxes[i].dx;
				my = by - mh;

				// landed on this ledge, stop any jump velocity
				game->player.dy = 0;
				game->player.onLedge = true;
			}
		}

		if (my + mh > by && my < by + bh)
		{

			// rubbing against right edge
			if (mx < bx + bw && mx + mw > bx + bw) // && game->player.dx < 0)
			{
				if (i == 20)
					printf("3st condition %g and bx:%g\n", mx + mw / 2, bx);
				// correct x
				game->player.x = bx + bw;
				mx = bx + bw;

				// game->player.dx = 0;
			}
			// rubbing against left edge
			else if (mx + mw > bx && mx < bx) // && game->player.dx > 0)
			{
				if (i == 20)
					printf("last elseif condition%g and bx:%g\n", mx + mw / 2, bx);
				// correct x
				game->player.x = bx - mw;
				mx = bx - mw;
			}
		}
	}
}

// gcc -std=c11 -Wall -pedantic testSdl.c `pkg-config --libs SDL2` -lSDL2_image -lSDL2_ttf -g -o testSdl