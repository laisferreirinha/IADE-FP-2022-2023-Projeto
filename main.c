#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int n_matches;
    int n_victories;
    int *special_pieces;
    int num_pieces;
} Player, *p_Player;

typedef struct {
    int lenght;
    int height;
    int winning_sequence;
    int num_pieces;
} Size;

typedef struct {
    p_Player *players;
    int num_players;
    int **board;
    Size *dimensions;
    p_Player player1;
    p_Player player2;
    bool status;
} Game;

void reset_game(Game *game) {
    game->board = NULL;
    game->player1 = NULL;
    game->player2 = NULL;
    game->status = false;
}

p_Player get_player(Game *game, char *name) {  // Se existir, retorna um jogador(nome, jogos e vitórias), se não existir retorna "NULL"
    for (int i = 0; i < game->num_players; i++) {
        if (strcmp(game->players[i]->name, name) == 0)
            return game->players[i];
    }
    return NULL;
}

int get_player_index(Game *game, char *name) {
    for (int i = 0; i < game->num_players; i++) {
        if (strcmp(game->players[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

bool player_in_game(Game *game, char *name) {
    p_Player player = get_player(game, name);
    return game->player1 == player || game->player2 == player;
}

bool check_player(Game *game, char *name) {
    p_Player player = get_player(game, name);
    return player != NULL;
}

void register_new_player(Game *game, char *name) {
    p_Player player = (p_Player)malloc(sizeof(Player));
    player->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(player->name, name);
    player->n_matches = 0;
    player->n_victories = 0;

    game->num_players++;
    game->players = realloc(game->players, sizeof(p_Player) * game->num_players);
    game->players[game->num_players - 1] = player;

}

void remove_player(Game *game, char *name) {
    int player_index = get_player_index(game, name);
    for (int i = player_index; i < game->num_players - 1; i++) {
        game->players[i] = game->players[i + 1];
    }
    game->players[game->num_players - 1] = NULL;
    game->num_players--;
    game->players = realloc(game->players, sizeof(p_Player) * game->num_players);
}

int compare_names(p_Player *p1, p_Player *p2) {
    return strcmp((*p1)->name, (*p2)->name);
}
int compare_pieces(int *p1, int *p2){
    return (*p1) - (*p2);
}

int* get_pieces(char *line, int *num_pieces){
// line = (0)1 (1)2 (2)3
    if(line == NULL) return NULL;

    char **s_size = (char**) malloc(sizeof(char*));
    int i=0;
    s_size[i] = strtok(line, " ");
    while(s_size[i] != NULL){
        i++; // i = 3
        s_size = realloc(s_size, sizeof(char*) * (i+1));
        s_size[i] = strtok(NULL, " ");
    }
    int *special_piece_size = malloc(sizeof(int) * i);
    for(int j = 0; j < i; j++){
        special_piece_size[j] = atoi(s_size[j]);
    }

    free(s_size);
    *num_pieces = i;
    return special_piece_size;
}

bool check_dimensions(int lenght, int height){
    if(lenght <= 0) return false;
    else if(height < lenght/2 || height > lenght) return false;
    else return true;
}

bool check_winning_sequence(int winning_sequence, int lenght){
    if(winning_sequence > lenght) return false;
    else return true;
}

bool check_special_pieces(int *special_piece_size, int num_pieces, int winning_sequence){
    for(int c = 0; c < num_pieces ; c++){
        if(special_piece_size[c] >= winning_sequence){
            return false;
        }
    }
    return true;
}

void save_dimensions(Game *game,int lenght,int height,int winning_sequence,int num_pieces){
    game->dimensions = (Size*) malloc(sizeof(Size));
    game->dimensions->lenght = lenght;
    game->dimensions->height = height;
    game->dimensions->winning_sequence = winning_sequence;
    game->dimensions->num_pieces = num_pieces;
}

void write_in_order(Game *game, char *name1, char *name2){
    int p1_index = get_player_index(game, name1);
    int p2_index = get_player_index(game, name2);
    game->players[p1_index]->n_matches++;
    game->players[p2_index]->n_matches++;
    if(strcmp(name1, name2) <= 0){
        game->player1 = game->players[p1_index];
        game->player2 = game->players[p2_index];
    }else{
        game->player1 = game->players[p2_index];
        game->player2 = game->players[p1_index];
    }
}

void start_new_game(Game *game,char *name1,char *name2,int *special_piece_size){

    write_in_order(game, name1, name2);
    game->player1->special_pieces = malloc(sizeof(int) * game->dimensions->num_pieces);
    game->player2->special_pieces = malloc(sizeof(int) * game->dimensions->num_pieces);
    for(int i = 0; i < game->dimensions->num_pieces; i++){
        game->player1->special_pieces[i] = special_piece_size[i];
        game->player1->num_pieces = game->dimensions->num_pieces;
        game->player2->special_pieces[i] = special_piece_size[i];
        game->player2->num_pieces = game->dimensions->num_pieces;
    }
    game->board = malloc(sizeof(int*) * game->dimensions->height);
    for(int i = 0; i < game->dimensions->height; i++){
        game->board[i] = malloc(sizeof(int) * game->dimensions->lenght);
        for(int j = 0; j < game->dimensions->lenght; j++){
            game->board[i][j] = 0;
        }
    }
    game->status = true;
}

int num_elements(int *array, int *i, int lim){
    int quantity = 1;
    for(int x = *i; x < lim; x++){
        if(x == lim-1){
            *i = x+1;
            return quantity;
        }
        else if(array[x] == 0){
            *i = x+1;
            return 0;
        }
        else{
            if(array[x] == array[x+1])
                quantity++;
            else{
                *i = x+1;
                return quantity;
            }
        }
    }
}

void give_up(Game *game, char *name1, char *name2) {
    if (name2 == NULL) {
        if (strcmp(game->player1->name, name1) == 0)
            game->player2->n_victories++;
        else
            game->player1->n_victories++;
    }

}

bool available_piece (Game *game, char *name, int piece_size){
    if(piece_size == 1){
        return true;
    }
    Player *player = get_player (game, name);
    for (int j=0; j< player->num_pieces; j++) {
        if (player->special_pieces[j] == piece_size){
            return true;
        }
    }
    return false;    
}

int empty_spot(Game *game, int position){
    for(int j = game->dimensions->height-1; j >= 0; j--){
            if(game->board[j][position-1] == 0){
                return j;
            }
    }
    return -1;
}

bool check_position(Game *game, int position, int piece_size, char *direction){
    if(position > game->dimensions->lenght) return false;

    else if(direction == NULL){
        if(empty_spot(game, position) < 0) return false;
        else return true;
    }    
    else if(strcmp(direction, "E") == 0){
        if((position-piece_size) < 0) return false;
        else{
            for(int i = position; i > (position-piece_size); i--){
                if(empty_spot(game, i) < 0) return false;
            }
            return true;
        }
        
    }
    else if(strcmp(direction, "D") == 0){
        if((position-1+piece_size) > game->dimensions->lenght) return false;
        else{
            for(int i = position; i < (position+piece_size); i++){
                if(empty_spot(game, i) < 0) return false;
            }
            return true;
        }
        
    }

}

void remove_piece(p_Player player, int piece_size){
    for (int j=0; j< player->num_pieces; j++) {
        if (player->special_pieces[j] == piece_size){
            for(int x = j; x < player->num_pieces-1; x++){
                player->special_pieces[x] = player->special_pieces[x+1];
            }
            player->num_pieces--;
            player->special_pieces = realloc(player->special_pieces, sizeof(int) * player->num_pieces);
            break;
        }
    }
}

void drop_piece(Game *game, char *name, int piece_size, int position, char *direction){
    p_Player player = get_player(game, name);

    if(direction == NULL){
        if(player == game->player1){
            int spot = empty_spot(game, position);
            game->board[spot][position-1] = 1;
        }
        else{
             int spot = empty_spot(game, position);
                game->board[spot][position-1] = 2;
        }
    }

    else if(player == game->player1){
        if(strcmp(direction, "E") == 0){
            for(int i = position; i > (position-piece_size); i--){
                int spot = empty_spot(game, i);
                game->board[spot][i-1] = 1;
            }
        }
        else if(strcmp(direction, "D") == 0){
            for(int i = position; i < (position+piece_size); i++){
                int spot = empty_spot(game, i);
                game->board[spot][i-1] = 1;
            }
        }
        remove_piece(player, piece_size);
    }
    else{
        if(strcmp(direction, "E") == 0){
            for(int i = position; i > (position-piece_size); i--){
                int spot = empty_spot(game, i);
                game->board[spot][i-1] = 2;
            }
        }
        else if(strcmp(direction, "D") == 0){
            for(int i = position; i < (position+piece_size); i++){
                int spot = empty_spot(game, i);
                game->board[spot][i-1] = 2;
            }
        }
        remove_piece(player, piece_size);
    }
}

bool check_win(Game *game){
    int *arrayAux = malloc(sizeof(int) * game->dimensions->lenght);
    // HORIZONTAL
    for(int i = game->dimensions->height-1; i > 0; i--){
        for(int j = 0; j < game->dimensions->lenght; j++){
            arrayAux[j] = game->board[i][j];
        }
        int x=0;
        while(x < game->dimensions->lenght){
            if(num_elements(arrayAux, &x, game->dimensions->lenght) == game->dimensions->winning_sequence){
                return true;
            }
        }
    }
    // VERTICAL
    arrayAux = realloc(arrayAux, sizeof(int) * game->dimensions->height);
    for(int i = 0; i < game->dimensions->lenght; i++){
        for(int j = game->dimensions->height-1; j >= 0 ; j--){
            arrayAux[j] = game->board[j][i];
        }
        int x=0;
        while(x < game->dimensions->height){
            if(num_elements(arrayAux, &x, game->dimensions->height) == game->dimensions->winning_sequence){
                return true;
            }
        }
    }
    // DIAGONAL
    arrayAux = realloc(arrayAux, sizeof(int) * game->dimensions->lenght);
    int i = game->dimensions->height-1;
    int j = 0;
    while(i >= 0 && j < game->dimensions->lenght){
        arrayAux[j] = game->board[i][j];
        i--;
        j++;

        int x=0;
        while(x < game->dimensions->height){
            if(num_elements(arrayAux, &x, game->dimensions->height) == game->dimensions->winning_sequence){
                return true;
            }
        }
    }

    i = game->dimensions->height-1;
    j = game->dimensions->lenght-1;
    while(i >= 0 && j >= 0){
        arrayAux[j] = game->board[i][j];
        i--;
        j--;
        
        int x=0;
        while(x < game->dimensions->height){
            if(num_elements(arrayAux, &x, game->dimensions->height) == game->dimensions->winning_sequence){
                return true;
            }
        }
    }

    return false;
}

void assign_win(Game *game, char *name){
    int player_index = get_player_index(game, name);
    game->players[player_index]->n_victories++;
}

void load_player(Game *game, char *name, int n_matches, int n_victories, int index){
    p_Player player = malloc(sizeof(p_Player));
    player->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(player->name, name);
    player->n_matches = n_matches;
    player->n_victories = n_victories;

    game->players[index] = player;
}

void load_game(Game *game, char *name1, char *name2){
    int p1_index = get_player_index(game, name1);
    int p2_index = get_player_index(game, name2);
    if(strcmp(name1, name2) <= 0){
        game->player1 = game->players[p1_index];
        game->player2 = game->players[p2_index];
    }else{
        game->player1 = game->players[p2_index];
        game->player2 = game->players[p1_index];
    }

}

void m_free(Game *game) {
    for (int i = 0; i < game->num_players; i++) {
        if (game->players[i] != NULL)
            free(game->players[i]);
    }
    free(game->players);
    free(game);
}

// void m_free(Game *game) {
//     free(game->dimensions);
//     for (int i = 0; i < game->num_players; i++) {
//         if (game->players[i] != NULL)
//             free(game->players[i]->name);
//             free(game->players[i]);
//     }
//     for(int i = 0; i < game->dimensions->height; i++){
//         free(game->board[i]);
//     }
//     free(game->board);
//     free(game->players);
//     free(game->player1->special_pieces);
//     free(game->player2->special_pieces);
//     free(game->player1);
//     free(game->player2);
//     free(game);
// }

int main() {
    char *line = NULL;
    char *overflow = NULL;
    size_t n = 0;
    Game *game = (Game *)malloc(sizeof(Game));
    game->players = NULL;
    game->num_players = 0;
    reset_game(game);

    while (true) {  //    CICLO INFINITO!

        getline(&line, &n, stdin);
        line[strlen(line) - 1] = '\0';

        if (strlen(line) == 0) break;

        char *command = strtok(line, " ");
        // ##############################################################################
        if (strcmp(command, "RJ") == 0) {
            char *name = strtok(NULL, " ");
            overflow = strtok(NULL, " ");

            if (name != NULL && overflow == NULL) {
                if (check_player(game, name)) {
                    printf("Jogador existente.\n");
                } else {
                    register_new_player(game, name);
                    printf("Jogador registado com sucesso.\n");
                }
            } else
                printf("Instrução inválida.\n");

            free(line);
            line = NULL;
        }
        // ##############################################################################
        else if (strcmp(command, "EJ") == 0) {
            char *name = strtok(NULL, " ");
            overflow = strtok(NULL, " ");

            if (name != NULL && overflow == NULL) {
                if (check_player(game, name) == false)
                    printf("Jogador não existente.\n");

                else if (player_in_game(game, name) == true)
                    printf("Jogador participa no jogo em curso.\n");

                else {
                    remove_player(game, name);
                    printf("Jogador removido com sucesso.\n");
                }

            } else
                printf("Instrução inválida.\n");
            
            free(overflow);
            free(line);
            line = NULL;
        }
        // ##############################################################################
        else if (strcmp(command, "LJ") == 0) {
            overflow = strtok(NULL, " ");
            if (overflow == NULL) {
                if (game->num_players == 0)
                    printf("Não existem jogadores registados.\n");
                else {
                    qsort(game->players, game->num_players, sizeof(p_Player), ( int (*)(const void *, const void *))compare_names);
                    for (int i = 0; i < game->num_players; i++) {
                        printf("%s %d %d\n", game->players[i]->name, game->players[i]->n_matches, game->players[i]->n_victories);
                    }   
                }

            } else
                printf("Instrução inválida.\n");
            
            free(overflow);
            free(line);
            line = NULL;
        }
        // ##############################################################################
        else if (strcmp(command, "IJ") == 0) {
            char *name1 = strtok(NULL, " ");
            char *name2 = strtok(NULL, " ");
            overflow = strtok(NULL, " ");

            char *line2 = NULL;
            getline(&line2, &n, stdin);
            line2[strlen(line2) - 1] = '\0';
            int lenght = atoi(strtok(line2, " "));
            int height = atoi(strtok(NULL, " "));
            int winning_sequence = atoi(strtok(NULL, " "));

            char *line3 = NULL;
            getline(&line3, &n, stdin);
            line3[strlen(line3) - 1] = '\0';
            int num_pieces = 0;
            int *special_piece_size = get_pieces(line3, &num_pieces);

            if(overflow != NULL || name1 == NULL || name2 == NULL){
                printf("Instrução inválida.\n");
            }
            else if(game->status){   
                printf("Existe um jogo em curso.\n");
            }

            else if(!check_player(game, name1) || !check_player(game, name2)){
                printf("Jogador não registado.\n");
            }

            else if(!check_dimensions(lenght, height)){
                printf("Dimensões de grelha inválidas.\n");
            }

            else if(!check_winning_sequence(winning_sequence, lenght)){
                printf("Tamanho de sequência inválido.\n");
            }

            else if(!check_special_pieces(special_piece_size, num_pieces, winning_sequence)){
                printf("Dimensões de peças especiais inválidas.\n");
            }
            
            else{
                qsort(special_piece_size, num_pieces, sizeof(int), (int (*)(const void*, const void*))compare_pieces);
                save_dimensions(game, lenght, height, winning_sequence, num_pieces);
                start_new_game(game, name1, name2, special_piece_size);
                printf("Jogo iniciado entre %s e %s.\n", game->player1->name, game->player2->name);
            }
            
            free(line2);
            free(line3);
            free(special_piece_size);
            free(overflow);
            free(line);
            line = NULL;
        }
        // ##############################################################################
        else if(strcmp(command, "DJ") == 0){
            overflow = strtok(NULL, " ");

            if(overflow != NULL)
                printf("Instrução inválida.\n");

            else if(!game->status)
                printf("Não existe jogo em curso.\n");

            else{
                printf("%d  %d\n", game->dimensions->lenght, game->dimensions->height);
                
                int i = 0;
                printf("%s\n", game->player1->name);
                while(i < game->player1->num_pieces){
                    printf("%d  ", game->player1->special_pieces[i]);
                    printf("%d\n", num_elements(game->player1->special_pieces, &i, game->player1->num_pieces));
                }
                i = 0;
                printf("%s\n", game->player2->name);
                while(i < game->player2->num_pieces){
                    printf("%d  ", game->player2->special_pieces[i]);
                    printf("%d\n", num_elements(game->player2->special_pieces, &i, game->player2->num_pieces));
                }
            }
            free(overflow);
            free(line);
            line = NULL;
        }
        // ##############################################################################
        else if (strcmp(command, "D") == 0) {
            char *name1 = strtok(NULL, " ");
            char *name2 = strtok(NULL, " ");
            overflow = strtok(NULL, " ");
            if (name1 != NULL && overflow == NULL) {
                if (!game->status)
                    printf("Não existe jogo em curso.\n");

                else if (!player_in_game(game, name1))
                    printf("Jogador não participa no jogo em curso.\n");

                else if (name2 != NULL) {
                    if (!player_in_game(game, name2))
                        printf("Jogador não participa no jogo em curso.\n");
                    else {
                        give_up(game, name1, name2);
                        printf("Desistência com sucesso. Jogo terminado.\n");
                    }
                } else {
                    give_up(game, name1, name2);
                    printf("Desistência com sucesso. Jogo terminado.\n");
                    reset_game(game);
                }

            } else
                printf("Instrução inválida.\n");

            free(overflow);
            free(line);
            line = NULL;
        }
        // ##############################################################################
        else if(strcmp(command, "CP") == 0){
            char *name = strtok(NULL, " ");
            int piece_size = atoi(strtok(NULL, " "));
            int position = atoi(strtok(NULL, " "));
            char *direction = strtok(NULL, " ");

            if(game->status == false){
                printf("Não existe jogo em curso.\n");
            }
            else if(player_in_game(game, name) == false){
                printf("Jogador não participa do jogo em curso. \n");
            }
            else if(available_piece(game, name, piece_size) == false){
                printf("Tamanho de peça não disponível.\n");
            }
            else if(check_position(game, position, piece_size, direction) == false){
                printf("Posição irregular.\n");
            }
            else{
                drop_piece(game, name, piece_size, position, direction);
                if(check_win(game) == true){
                    assign_win(game, name);
                    reset_game(game);
                    printf("Sequência conseguida. Jogo terminado.\n");
                }
                else{
                    printf("Peça colocada.\n");
                 }
            }
            free(line);
            line = NULL;
        }
        // ##############################################################################
        else if(strcmp(command, "VR") == 0){
            if(game->status == false){
                printf("Não existe jogo em curso.\n");
            }else{
                for(int i = 0; i < game->dimensions->height; i++){
                    for(int j = 0; j < game->dimensions->lenght; j++){
                        if(game->board[i][j] == 0) printf("%d %d Vazio\n", i+1, j+1);
                        else if(game->board[i][j] == 1) printf("%d %d %s\n",i+1, j+1, game->player1->name);
                        else printf("%d %d %s\n", i+1, j+1, game->player2->name);
                    }
                }
            }
            
        }
        // ##############################################################################
        else if(strcmp(command, "G") == 0){
            FILE *file = fopen("backup.txt", "w");
            if(file == NULL) printf("Ocorreu um erro na gravação.\n");
            else{
                fprintf(file, "%d\n", game->num_players);
                qsort(game->players, game->num_players, sizeof(p_Player), ( int (*)(const void *, const void *))compare_names);
                for (int i = 0; i < game->num_players; i++) {
                    fprintf(file, "%s %d %d\n", game->players[i]->name, game->players[i]->n_matches, game->players[i]->n_victories);
                }
                if(game->status){
                    fprintf(file, "1\n");
                    fprintf(file, "%d %d %d\n", game->dimensions->lenght, game->dimensions->height, game->dimensions->winning_sequence);
                    fprintf(file, "%s %s\n", game->player1->name, game->player2->name);
                    for(int i = 0; i < game->dimensions->height; i++){
                        for(int j = 0; j < game->dimensions->lenght; j++){
                            fprintf(file, "%d ", game->board[i][j]);
                        }
                        fprintf(file, "\n");
                    }
                }else fprintf(file, "0\n");
                printf("Jogo gravado.\n");   
            }
            fclose(file);
        }
        // ##############################################################################
        else if(strcmp(command, "L") == 0){
            FILE *file = fopen("backup.txt", "r");

            if(file == NULL) printf("Ocorreu um erro no carregamento.\n");

            else{
                char *file_line = NULL;
                getline(&file_line, &n, file);
                file_line[strlen(file_line)-1] = '\0';
                game->num_players = atoi(file_line);
                
                game->players = malloc(sizeof(p_Player) * game->num_players);
                if(game->num_players != 0){
                    for(int i = 0; i < game->num_players; i++){ 
                        file_line = NULL;
                        getline(&file_line, &n, file);
                        file_line[strlen(file_line)-1] = '\0';

                        char *name = strtok(file_line, " ");
                        int n_matches = atoi(strtok(NULL, " "));
                        int n_victories = atoi(strtok(NULL, " "));
                        load_player(game, name, n_matches, n_victories, i);
                    }

                    file_line = NULL;
                    getline(&file_line, &n, file);
                    file_line[strlen(file_line)-1] = '\0';
                    game->status = atoi(file_line);

                    if(game->status){
                        file_line = NULL;
                        getline(&file_line, &n, file);
                        file_line[strlen(file_line)-1] = '\0';
                        int lenght = atoi(strtok(file_line, " "));
                        int height = atoi(strtok(NULL, " "));
                        int winning_sequence = atoi(strtok(NULL, " "));
                        save_dimensions(game, lenght, height, winning_sequence, game->num_players);

                        file_line = NULL;
                        getline(&file_line, &n, file);
                        file_line[strlen(file_line)-1] = '\0';
                        char *name1 = strtok(file_line, " ");
                        char *name2 = strtok(NULL, " ");
                        load_game(game, name1, name2);

                        game->board = malloc(sizeof(int*) * game->dimensions->height);

                        file_line = NULL;
                        for (int l = 0; l < game->dimensions->height; l++) {
                            game->board[l] = malloc(sizeof(int) * game->dimensions->lenght);
                            file_line = NULL;
                            getline(&file_line, &n, file);
                            char* token = strtok(file_line, " ");
                            for (int c = 0; c < game->dimensions->lenght; c++) {
                                game->board[l][c] = atoi(token);
                                token = strtok(NULL, " ");
                            }
                        }
                    }
                }

                printf("Jogo carregado.\n");
            }
        }
        // ##############################################################################
        else if(strcmp(command, "X_Helder") == 0){
            for(int i = 0; i < game->dimensions->height; i++){
                for(int j = 0; j < game->dimensions->lenght; j++){
                    printf("%d ", game->board[i][j]);
                }
                printf("\n");
            } 
            free(line);
            line = NULL;
        }
        // ##############################################################################
        else printf("Instrução inválida.\n");
    }
       

    free(line);
    free(overflow);
    m_free(game);
    return 0;
}
