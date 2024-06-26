#include "common.h"
#include "Fruit.h"

SDL_Window* g_window = nullptr;
SDL_Renderer* g_render = nullptr;
TTF_Font* g_font = nullptr;
Mix_Music* gMusic = NULL;

//The sound effects that will be used
Mix_Chunk* g_eatfood = NULL;
Mix_Chunk* g_eatdot = NULL;
Mix_Chunk* g_music = NULL;

vector<vector<bool> > visited;
vector<vector<bool> > has_point;
vector<vector<bool> > has_postion1;
vector<vector<bool> > has_postion2;
vector<vector<pair<bool, int> > > has_food;
vector<vector<SDL_Rect> > wall;

int cntheight, cntwidth;
int dir[4] = {0, 1, 2, 3};
int rannum;
ii endgame;
int round_in = 0;
int begin_x, begin_y;
int cnt_change_maze;
bool game_start;
bool game_has_food = 0;
int score;
Fruit food;

// x -> row
// y -> cols

vector<int> list_choose_food;
vector<ii> list_road;

SDL_Texture* loadimg(std::string path, SDL_Renderer* screen){
    SDL_Texture* new_texture = nullptr;

    SDL_Surface* load_surface = IMG_Load(path.c_str());

    if(load_surface != nullptr){
        SDL_SetColorKey(load_surface, SDL_TRUE, SDL_MapRGB(load_surface->format, COLOR_KEY_R, COLOR_KEY_G, COLOR_KEY_B));
        new_texture = SDL_CreateTextureFromSurface(screen, load_surface);
        SDL_FreeSurface(load_surface);
    }
    return new_texture;
}

SDL_Texture *Tex_wall;
SDL_Texture *Tex_ground;
// has_food.first: has food or not
// has_food.second: type of food

void change_size(int round_in){
    srand(time(0));
    rannum = rand() % 1000;
    begin_x = begin_y = 1;
    game_has_food = 0;
    cntheight = SCREEN_HEIGHT / rect_height[round_in];
    cntwidth = SCREEN_WIDTH / rect_width[round_in];

    visited.clear();
    visited.resize(cntheight, vector<bool> (cntwidth));
    for(int i = 0; i < cntheight; i ++)
        for(int j = 0; j < cntwidth; j ++)
        visited[i][j] = 0;

    list_road.clear();
    has_point.clear();
    if(round_in == 2) has_point.resize(cntheight, vector<bool> (cntwidth, 1));
    else has_point.resize(cntheight, vector<bool> (cntwidth, 0));

    has_postion1.resize(cntheight, vector<bool> (cntwidth, 0));
    has_postion2.resize(cntheight, vector<bool> (cntwidth, 0));

    has_food.clear();
    has_food.resize(cntheight, vector<pair<bool, int>> (cntwidth, pair<bool,int>(0, -1)));

    wall.clear();
    wall.resize(cntheight, vector<SDL_Rect> (cntwidth));
    int xx = 0, yy = 0;
    for(int i = 0; i < cntheight; i ++){
        xx = 0;
        for(int j = 0; j < cntwidth; j ++){
            wall[i][j].x = xx;
            wall[i][j].y = yy;
            wall[i][j].h = rect_height[round_in];
            wall[i][j].w = rect_width[round_in];
            xx += rect_width[round_in];
        }
        yy += rect_height[round_in];
    }

    food.pre_image(g_render);
    srand(time(0));
    list_choose_food.clear();
    int cnt = rand() % 40;
    for(int i = 0; i < num_food[round_in]; i ++){
        srand(cnt ++);
        int k = rand() % int(list_food.size() - 1);
        list_choose_food.push_back(k);
    }
}

void quitSDL(){
    SDL_DestroyRenderer(g_render);
    SDL_DestroyWindow(g_window);
    TTF_CloseFont(g_font);

    g_render = nullptr;
    g_window = nullptr;
    g_font = nullptr;

    SDL_DestroyTexture(Tex_wall);
    Tex_wall = NULL;

    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

bool initdata(){
    srand(time(0));
    rannum = rand() % 30;
    bool success = 1;

    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0){
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        g_window = SDL_CreateWindow("newhello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(g_window != nullptr){
            g_render = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(g_render != nullptr){
                SDL_SetRenderDrawColor(g_render, 0, 0, 0, 0);
                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }

                 //Initialize SDL_ttf
                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
                g_font = TTF_OpenFont( "img/VNI-HelveB.ttf", size_text[round_in]);

                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}

//                    gMusic = Mix_LoadMUS( "music/intro.wav" );
//                    if( gMusic == NULL ){
//                        printf( "Failed to load pacman_intro music! SDL_mixer Error: %s\n", Mix_GetError() );
//                        success = false;
//                    }
//                    g_music = Mix_LoadWAV( "music/pacman_beginning.wav" );
//                    if( g_music == NULL ){
//                        printf( "Failed to load pacman_beginning music! SDL_mixer Error: %s\n", Mix_GetError() );
//                        success = false;
//                    }
                    g_eatdot = Mix_LoadWAV( "music/eatdot.wav" );
                    if( g_eatdot == NULL )
                    {
                        printf( "Failed to load eatdot sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
                        success = false;
                    }
                    g_eatfood = Mix_LoadWAV( "music/eatfruit.wav" );
                    if( g_eatfood == NULL )
                    {
                        printf( "Failed to load eatfruit sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
                        success = false;
                    }

            }
        }
    }

    Tex_wall = loadimg("img/menu/wall.png", g_render);
    return success;
}

bool inmaze(int x, int y){
    if(x < 0 || x >= cntwidth) return 0;
    if(y < 0 || y >= cntheight) return 0;
    return 1;
}

// has_point = 0 ? true : false
// has_food = 0 ? false : true
SDL_Texture *Tex = loadimg("img/menu/wall.png", g_render);

void fillscreen(SDL_Renderer* screen){

    for(int i = 0; i < cntheight; i ++)
       for(int j = 0; j < cntwidth; j ++){
            SDL_SetRenderDrawColor( screen, color_road[0], color_road[1], color_road[2], color_road[3]);
            SDL_RenderFillRect(screen, &wall[i][j]);
       }

    for(int i = 0; i < cntheight; i ++)
       for(int j = 0; j < cntwidth; j ++){
            if(visited[i][j]){
//                SDL_RenderCopy(screen, Tex_ground, NULL, &wall[i][j]);
                SDL_SetRenderDrawColor( screen, color_road[0], color_road[1], color_road[2], color_road[3]);
                SDL_RenderFillRect(screen, &wall[i][j]);
                continue;
            }
            SDL_RenderCopy(screen, Tex_wall, NULL, &wall[i][j]);
       }

    for(int i = 0; i < cntwidth; i ++){
        SDL_SetRenderDrawColor( screen, color_wall[0], color_wall[1], color_wall[2], color_wall[3]);
        SDL_RenderFillRect(g_render, &wall[0][i]);
    }

    for(int i = 1; i < cntheight - 1; i ++)
       for(int j = 1; j < cntwidth - 1; j ++){
            if(visited[i][j] && !has_point[i][j] && !has_food[i][j].first){
                SDL_SetRenderDrawColor( screen, color_point[0], color_point[1], color_point[2], color_point[3]);
                int xx = wall[i][j].x, yy = wall[i][j].y;
                SDL_Rect tmp = {xx + wall[i][j].w / 2 - 2, yy + wall[i][j].h / 2 - 2, 4, 4};
                SDL_RenderFillRect(g_render, &tmp);
            }
       }


    for(int i = 1; i < cntheight - 1; i ++)
        for(int j = 1; j < cntwidth - 1; j ++)
        if(has_food[i][j].first){
            food.render_img(screen, i, j, &list_food[has_food[i][j].second]);
        }

    SDL_Texture *Tex = loadimg("img/menu/redflag.png", screen);
    SDL_RenderCopy(screen, Tex, NULL, &wall[endgame.first][endgame.second]);
    SDL_DestroyTexture(Tex);

//    SDL_SetRenderDrawColor(screen, color_des[0], color_des[1], color_des[2], color_des[3]);
//    SDL_RenderFillRect(screen, &wall[endgame.first][endgame.second]);

}

void clear_visited(int x, int y){
    begin_x = x, begin_y = y;
    visited.clear();
    visited.resize(cntheight, vector<bool> (cntwidth));
    for(int i = 0; i < cntheight; i ++)
        for(int j = 0; j < cntwidth; j ++)
        visited[i][j] = 0;

    wall.clear();
    wall.resize(cntheight, vector<SDL_Rect> (cntwidth));
    int xx = 0, yy = 0;
    for(int i = 0; i < cntheight; i ++){
        xx = 0;
        for(int j = 0; j < cntwidth; j ++){
            wall[i][j].x = xx;
            wall[i][j].y = yy;
            wall[i][j].h = rect_height[round_in];
            wall[i][j].w = rect_width[round_in];
            xx += rect_width[round_in];
        }
        yy += rect_height[round_in];
    }
}

void maze(SDL_Renderer* screen){

    endgame = {0, 0};
    vector<ii> st;

    st.push_back({1, 1});
    srand(rannum);
    int cnt = rand()% 100;

    // visited[x][y] = 1 -> road
    // visited[x][y] = 0 -> wall

    while(st.size()){
        srand(cnt ++);
        ii current = st.back();
        st.pop_back();

        //printf("Get SDL error %s\n%d %d\n", SDL_GetError(), current.first, current.second);
        visited[current.first][current.second] = 1;
        for(int i = 0; i < 4; i ++){
            int r = rand() % 3;
            swap(dir[i], dir[r]);
        }
        for(int i = 0; i < 4; i ++){
            int dx = 0, dy = 0;
            switch (dir[i]){
                case 0: dy -= 1; break;
                case 1: dx += 1; break;
                case 2: dy += 1; break;
                case 3: dx -= 1; break;
            }
            int x2 = current.first + dx * 2;
            int y2 = current.second + dy * 2;

            if(!inmaze(x2, y2)) continue;
            if(visited[x2][y2]) continue;
            visited[x2][y2] = 1;

            visited[x2 - dx][y2 - dy] = 1;
            st.push_back({x2, y2});
        }
    }

    if(endgame == ii(0, 0)){
        vector<ii> tmp;
        for(int i = cntheight / 2; i < cntheight - 1; i ++)
           for(int j = 1; j < cntwidth - 1; j ++){
                if(!visited[i][j]) continue;
                //if(i == current_x && j == current_y) continue;
                tmp.push_back({i, j});
           }
        int k = rand() % (tmp.size());

        endgame = {tmp[k].first, tmp[k].second};
    }

    if(list_road.size() == 0){
        for(int i = 1; i < cntheight - 1; i ++)
           for(int j = 1; j < cntwidth - 1; j ++)
                if(visited[i][j] && ii(i, j) != endgame && ii(i, j) != ii(1, 1)){
                    list_road.push_back({i, j});
                }
    }

    mt19937 rng;
//    cout << list_choose_food.size() << '\n';
    if(!game_has_food){
        game_has_food = 1;
        rng.seed(rannum);
        int n = list_road.size() - 1, cnt_food = 0;
        for(int i = 0; i < num_food[round_in]; i ++){
            int j = rng() % n;
            int x = list_road[j].first;
            int y = list_road[j].second;
            has_food[x][y].first = 1;
            has_food[x][y].second = list_choose_food[cnt_food ++];
        }
    }

}


