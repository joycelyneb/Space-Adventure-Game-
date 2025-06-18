#include <raylib.h> //library
#include <stdio.h>

#define MAX_BULLETS 50 // batas jumlah peluru
#define MAX_UFOS 10 // batas jumlah ufo

enum GameState { MENU, SELECT_PLANE, PLAYING, GAME_OVER }; //buat ngatur status permainan

class Plane
{
public:
    int x;
    int y;
    Texture2D texture;

    Plane(int posX, int posY, const char *imagePath)
    {
        x = posX;
        y = posY;
        texture = LoadTexture(imagePath);
    }

    Plane() : x(0), y(0) {
        texture.id = 0;
        texture.width = 0;
        texture.height = 0;
        texture.mipmaps = 0;
        texture.format = 0;
    }

    void setTexture(const char *imagePath)
    {
        if (texture.id != 0) UnloadTexture(texture);
        texture = LoadTexture(imagePath);
    }

    void draw()
    {
        DrawTextureEx(texture, (Vector2){(float)x, (float)y}, 0, 0.2f, WHITE);
    }

    void update()
    {
        if (IsKeyDown(KEY_RIGHT) && x <= 800)
        {
            x += 5;
        }
        else if (IsKeyDown(KEY_LEFT) && x >= 0)
        {
            x -= 5;
        }
    }

    Rectangle rectangle() // Tambahkan fungsi rectangle untuk collision
    {
        return Rectangle{(float)x, (float)y, (float)texture.width * 0.2f, (float)texture.height * 0.2f};
    }
};

class Bullet
{
public:
    int x;
    int y;
    int radius;
    Color color;
    bool active;

    Bullet()
    {
        active = false; //status aktif peluru
    }

    void shoot(int posX, int posY, int rad, Color col)
    { //menembak peluru
        x = posX;
        y = posY;
        radius = rad;
        color = col;
        active = true;
    }

    void draw()
    { //gambar peluru (kalau aktif)
        if (active)
        {
            DrawCircle(x, y, radius, color);
        }
    }

    void update() //memperbarui posisi peluru
    {
        if (active)
        {
            y -= 3;

            //deaktif peluru kalau udah diatas
            if (y <= 0)
            {
                active = false;
            }
        }
    }

    void deactivate() { active = false; }

    bool cekTabrakan(Rectangle rect)
    { //cek tubrukan
        return CheckCollisionCircleRec(Vector2{float(x), float(y)}, radius, rect);
    }
};

class Ufo
{
public:
    int x;
    int y;
    Texture2D texture;
    bool active;
    int speed;

    Ufo() : x(0), y(0), active(false), speed(2) {
        texture.id = 0;
        texture.width = 0;
        texture.height = 0;
        texture.mipmaps = 0;
        texture.format = 0;
    }

    Ufo(int posX, int posY, const char *imagePath, int spd)
    {
        x = posX;
        y = posY;
        texture = LoadTexture(imagePath);
        active = true;
        speed = spd;
    }

    void draw()
    {
        if (active)
        {
            DrawTextureEx(texture, (Vector2){(float)x, (float)y}, 0, 0.2f, WHITE);
        }
    }

    void update()
    {
        if (active)
        {
            y += speed;

            //reset posisi kalo udah dibawah screen
            if (y >= 600)
            {
                resetPosition();
            }
        }
    }

    Rectangle rectangle() //buat gambar objek ufonya, buat cek tabrakan
    {
        return Rectangle{(float)x, (float)y, (float)texture.width * 0.2f, (float)texture.height * 0.2f};
    }

    void deactivate() //non aktif ufo dan reset ulang
    {
        active = false;
        resetPosition();
    }

    void resetPosition() //reset random 50-750
    {
        x = GetRandomValue(50, 750);
        y = 0;
        active = true;
    }
};

int ReadHighScore()
{
    FILE *file = fopen("highscore.txt", "r");
    if (file == NULL) return 0; //kalau file gada, reser highscore default (0)

    int highScore;
    fscanf(file, "%d", &highScore); //baca code
    fclose(file); //nutup file
    return highScore;
}

void SaveHighScore(int highScore) //nyimpen highscore
{
    FILE *file = fopen("highscore.txt", "w");
    if (file == NULL) return; 

    fprintf(file, "%d", highScore); //print highscore
    fclose(file);
}

int main()
{
    InitWindow(800, 600, "Plane Game");
    SetTargetFPS(60);

    Texture2D background = LoadTexture("src/assets/tess (1) (1).png");
    Plane plane(400, 500, "src/assets/plane1.png");
    Bullet bullets[MAX_BULLETS];
    Ufo ufos[MAX_UFOS];

    int ufoSpeed = 2;

    for (int i = 0; i < MAX_UFOS; i++)
    {
        int posX = GetRandomValue(50, 750);
        int posY = 0;
        ufos[i] = Ufo(posX, posY, "src/assets/ufo.png", ufoSpeed);
    }

    int score = 0;
    int highScore = ReadHighScore();
    GameState gameState = MENU;

    const char* planeTextures[] = {
        "src/assets/planeSATU.png",
        "src/assets/planeDUA.png",
        "src/assets/planeTIGA.png"
    };
    Texture2D planeTexturePreview[3]; // Tambahkan array untuk preview
    for (int i = 0; i < 3; i++) {
        planeTexturePreview[i] = LoadTexture(planeTextures[i]);
    }
    int selectedPlane = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (gameState)
        {
        case MENU:
            DrawTexture(background, 0, 0, WHITE);
            DrawText("SPACE ADVENTURE", 100, 230, 60, YELLOW);
            DrawText("Press P to Select Plane", 150, 400, 40, WHITE);
            if (IsKeyPressed(KEY_P))
            {
                gameState = SELECT_PLANE;
            }
            break;

        case SELECT_PLANE:
            DrawTexture(background, 0, 0, WHITE);
            DrawText("Select Your Plane", 200, 50, 40, WHITE);

            for (int i = 0; i < 3; i++)
            {
                DrawTextureEx(planeTexturePreview[i], (Vector2){150 + 200 * i, 200}, 0, 0.2f, WHITE);
                if (selectedPlane == i)
                {
                    DrawRectangleLines(150 + 200 * i, 200, planeTexturePreview[i].width * 0.2f, planeTexturePreview[i].height * 0.2f, RED);
                }
            }

            DrawText("Press LEFT/RIGHT to Change Plane", 150, 450, 20, WHITE);
            DrawText("Press ENTER to Confirm", 200, 500, 20, WHITE);

            if (IsKeyPressed(KEY_RIGHT))
            {
                selectedPlane = (selectedPlane + 1) % 3;
            }
            if (IsKeyPressed(KEY_LEFT))
            {
                selectedPlane = (selectedPlane + 2) % 3;
            }
            if (IsKeyPressed(KEY_ENTER))
            {
                plane.setTexture(planeTextures[selectedPlane]);
                gameState = PLAYING;
            }
            break;

        case PLAYING:
            DrawTexture(background, 0, 0, WHITE);

            plane.draw();
            plane.update();

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullets[i].active && IsKeyPressed(KEY_SPACE))
                {
                    bullets[i].shoot(plane.x + (plane.texture.width * 0.1f), plane.y, 5, YELLOW);
                    break;
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                bullets[i].draw();
                bullets[i].update();
            }

            for (int i = 0; i < MAX_UFOS; i++)
            {
                ufos[i].draw();
                ufos[i].update();
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                for (int j = 0; j < MAX_UFOS; j++)
                {
                    if (bullets[i].cekTabrakan(ufos[j].rectangle()) && ufos[j].active && bullets[i].active)
                    {
                        bullets[i].deactivate();
                        ufos[j].deactivate();
                        score += 10;
                    }
                }
            }

            // Perbaiki rectangle pesawat untuk collision
            for (int i = 0; i < MAX_UFOS; i++)
            {
                if (ufos[i].active && CheckCollisionRecs(ufos[i].rectangle(), plane.rectangle()))
                {
                    gameState = GAME_OVER;
                }
            }

            DrawText(TextFormat("Score: %04i", score), 10, 10, 20, WHITE);

            break;

        case GAME_OVER:
            if (score > highScore)
            {
                highScore = score;
                SaveHighScore(highScore);
            }
            DrawTexture(background, 0, 0, WHITE);
            DrawText("GAME OVER", 290, 250, 40, WHITE);
            DrawText(TextFormat("Final Score: %04i", score), 330, 300, 20, WHITE);
            DrawText(TextFormat("High Score: %04i", highScore), 330, 330, 20, WHITE);
            DrawText("Press R to Restart", 310, 370, 20, WHITE);

            if (IsKeyPressed(KEY_R))
            {
                gameState = MENU;
                score = 0;
                plane.x = 400;
                plane.y = 500;

                for (int i = 0; i < MAX_UFOS; i++)
                {
                    int posX = GetRandomValue(50, 750);
                    int posY = 0;
                    ufos[i] = Ufo(posX, posY, "src/assets/ufo.png", ufoSpeed);
                }

                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    bullets[i].deactivate();
                }
            }

            break;
        }

        EndDrawing();
    }

    // Unload textures untuk mencegah memory leak
    UnloadTexture(background);
    UnloadTexture(plane.texture);
    for (int i = 0; i < 3; i++) {
        UnloadTexture(planeTexturePreview[i]);
    }
    for (int i = 0; i < MAX_UFOS; i++) {
        UnloadTexture(ufos[i].texture);
    }

    CloseWindow();

    return 0;
}
