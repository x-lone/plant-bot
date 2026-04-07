import pygame, sys

DISPLAY_SCALE = 30
DISPLAY_WIDTH, DISPLAY_HEIGHT = 8, 6

pygame.init()
screen = pygame.display.set_mode((DISPLAY_WIDTH * DISPLAY_SCALE, DISPLAY_HEIGHT * DISPLAY_SCALE))
clock = pygame.time.Clock()

matrix = [[0 for _ in range(DISPLAY_WIDTH)] for _ in range(DISPLAY_HEIGHT)]

cursor_x = 0
cursor_y = 0

def toggle_pixel(x, y):
    matrix[y][x] = 1 if matrix[y][x] == 0 else 0

def export_matrix():
    print("{", end="")
    for i, row in enumerate(matrix):
        bits = ''.join(str(bit) for bit in row)
        if i < len(matrix) - 1:
            print(f"0b{bits}, ", end="")
        else:
            print(f"0b{bits}", end="")
    print("}")

def handle_input(event):
    global cursor_x, cursor_y
    if event.key == pygame.K_DOWN:
        cursor_y += 1
    if event.key == pygame.K_UP:
        cursor_y -= 1
    if event.key == pygame.K_RIGHT:
        cursor_x += 1
    if event.key == pygame.K_LEFT:
        cursor_x -= 1
    if event.key == pygame.K_SPACE:
        toggle_pixel(cursor_x, cursor_y)
    if event.key == pygame.K_RETURN:
        export_matrix()

def draw():
    screen.fill((0,0,0))

    for row in range(DISPLAY_HEIGHT):
        for col in range(DISPLAY_WIDTH):
            if matrix[row][col] == 1:
                pygame.draw.rect(screen, (0,255,0), (col * DISPLAY_SCALE, row * DISPLAY_SCALE, DISPLAY_SCALE, DISPLAY_SCALE))
  
    pygame.draw.rect(screen, (255,255,255), (cursor_x * DISPLAY_SCALE, cursor_y * DISPLAY_SCALE, DISPLAY_SCALE, DISPLAY_SCALE), 5)

    pygame.display.update()


while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()
        if event.type == pygame.KEYDOWN:
            handle_input(event)
    
    draw()
    clock.tick(60)