#include <SDL2/SDL.h>

int get_stack(const char* pid, const char* memtype, long* start, long* end)
{
    char maps[100];
    snprintf(maps, 100, "/proc/%s/maps", pid);
    FILE* file = fopen(maps, "r");
    if (!file)
    {
        printf("Could not open %s\n", maps);
    }
    char buf[500];
    while (fgets(buf, sizeof(buf), file) != NULL)
    {
        int result = sscanf(buf, "%lx-%lx%*[^[][%[^]]]", start, end, buf);
        if (result == 3 && !strcmp(buf, memtype))
        {
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}

FILE* open_mem(char* pid)
{
    char mem[100];
    snprintf(mem, 100, "/proc/%s/mem", pid);
    return fopen(mem, "r");
}

void draw_screen(SDL_Renderer *renderer, FILE* memfile, long memstart, long memend, int displayStart)
{
    int screenWidth, screenHeight;
    int pixelSize = 2;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    screenWidth /= pixelSize;
    screenHeight /= pixelSize;
    long offset = displayStart ? memstart : memend - screenWidth * screenHeight / 8;
    fseek(memfile, offset, SEEK_SET);
    int index = 7;
    Uint8 val = 0;
    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            index++;
            if (index == 8)
            {
                val = (Uint8)fgetc(memfile);
                index = 0;
            }
            Uint8 pix = ((val >> index) & 1) ? 255 : 0;
            SDL_SetRenderDrawColor(renderer, pix, pix, pix, 255);
            SDL_Rect rect;
            rect.x = x * pixelSize;
            rect.y = y * pixelSize;
            rect.w = pixelSize;
            rect.h = pixelSize;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 4)
    {
        printf("Usage: %s [PID] [memtype] [start|end]. Usually needs root.\n", argv[0]);
        return 1;
    }
    char* memtype = argc == 2 ? "stack" : argv[2];
    int displayStart = argc != 2;
    if (argc == 4)
    {
        if (!strcmp(argv[3], "start"))
        {
            displayStart = 1;
        }
        else if (!strcmp(argv[3], "end"))
        {
            displayStart = 0;
        }
        else
        {
            puts("Third arg must be either \"start\" or \"end\"");
            return 1;
        }
    }
    long memstart, memend;
    if (get_stack(argv[1], memtype, &memstart, &memend))
    {
        puts("Unable to find stack");
        return 1;
    }
    FILE* mem = open_mem(argv[1]);
    if (!mem)
    {
        puts("Unable to open memory file");
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("Memdisplay", 100, 100, 500, 500, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    while (1)
    {
        SDL_Event evnt;
        if (SDL_PollEvent(&evnt))
        {
            if (evnt.type == SDL_QUIT)
            {
                break;
            }
        }
        else
        {
            draw_screen(renderer, mem, memstart, memend, displayStart);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    fclose(mem);
}
