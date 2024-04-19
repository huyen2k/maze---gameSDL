#include "Oneplayer.h"

Oneplayer::Oneplayer()
{
    //ctor
}

Oneplayer::~Oneplayer()
{
    //dtor
}

void Oneplayer::gameplay(SDL_Renderer* g_render, bool &round, bool &out){
    SDL_Event e;
    score = 0;

    while(!round){

        bool quit = 0;
        change_size(round_in);

        Score scr(rect_width[round_in]);
        characer character1;
        character1.init_data();
        maze(g_render);

        while(!quit){
            while(SDL_PollEvent(&e) != 0){
                if(e.type == SDL_QUIT){
                    round = 1;
                    break;
                }
                character1.handinput(e, g_render);

            }
            if(round) break;

            {
                SDL_SetRenderDrawColor(g_render, color_road[0], color_road[1], color_road[2], color_road[3] );
                SDL_RenderClear(g_render);
                fillscreen(g_render);
                scr.render(g_render, 0, 0);
                scr.render_number(g_render, rect_width[round_in] * 3, 0, score);
                character1.runAnimation(g_render, e);
            }

            SDL_RenderPresent(g_render);

            if(character1.check_win()) quit = 1;
            character1.check_food();
        }
        round_in ++;
        if(round_in == 3) round = 1;
        if(round) break;
    }

    if(!update_gameover(g_render, 1, score, 0)) out = 1;
    else {
        round_in = 0;
        round = 0;
        game_start = 0;
    }
}