"""
Quadtree Collision Detection Visualization
-------------------------------------------
Demonstrates the performance difference between Brute Force O(n^2) and Quadtree O(n log n).

Requirements: pip install pygame
"""

import pygame
import random
import math
import time
from dataclasses import dataclass
from typing import List, Optional

# ============== CONFIGURATIONS ==============
WIDTH, HEIGHT = 1200, 800
FPS_CAP = 60
INITIAL_BALLS = 200
BALL_MIN_RADIUS = 5
BALL_MAX_RADIUS = 12
QUADTREE_CAPACITY = 4  # Max objects before subdividing

# Colors
BG_COLOR = (15, 15, 25)
GRID_COLOR = (40, 80, 120)
TEXT_COLOR = (220, 220, 220)
ACCENT_COLOR = (100, 200, 255)
WARNING_COLOR = (255, 100, 100)
SUCCESS_COLOR = (100, 255, 150)
UI_PANEL_COLOR = (20, 20, 35)
UI_BORDER_COLOR = (50, 50, 70)

# ============== UI COMPONENTS ==============

class Button:
    def __init__(self, x: int, y: int, w: int, h: int, text: str):
        self.rect = pygame.Rect(x, y, w, h)
        self.text = text
        self.base_color = UI_BORDER_COLOR
        self.hover_color = (70, 70, 90)

    def draw(self, screen: pygame.Surface, font: pygame.font.Font):
        pos = pygame.mouse.get_pos()
        color = self.hover_color if self.rect.collidepoint(pos) else self.base_color
        
        pygame.draw.rect(screen, color, self.rect, border_radius=5)
        pygame.draw.rect(screen, (100, 100, 120), self.rect, 2, border_radius=5)
        
        text_surf = font.render(self.text, True, TEXT_COLOR)
        text_rect = text_surf.get_rect(center=self.rect.center)
        screen.blit(text_surf, text_rect)

    def is_clicked(self, event: pygame.event.Event) -> bool:
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            return self.rect.collidepoint(event.pos)
        return False

class InputBox:
    def __init__(self, x: int, y: int, w: int, h: int, text: str = ''):
        self.rect = pygame.Rect(x, y, w, h)
        self.color_inactive = UI_BORDER_COLOR
        self.color_active = ACCENT_COLOR
        self.color = self.color_inactive
        self.text = text
        self.active = False

    def handle_event(self, event: pygame.event.Event) -> Optional[str]:
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            if self.rect.collidepoint(event.pos):
                self.active = not self.active
            else:
                self.active = False
            self.color = self.color_active if self.active else self.color_inactive
            
        if event.type == pygame.KEYDOWN and self.active:
            if event.key == pygame.K_RETURN:
                return self.text  # Return value when Enter is pressed
            elif event.key == pygame.K_BACKSPACE:
                self.text = self.text[:-1]
            elif event.unicode.isnumeric():
                self.text += event.unicode
        return None

    def draw(self, screen: pygame.Surface, font: pygame.font.Font):
        pygame.draw.rect(screen, UI_PANEL_COLOR, self.rect, border_radius=5)
        pygame.draw.rect(screen, self.color, self.rect, 2, border_radius=5)
        text_surf = font.render(self.text, True, TEXT_COLOR)
        # Center vertically, offset slightly horizontally
        screen.blit(text_surf, (self.rect.x + 8, self.rect.y + (self.rect.h - text_surf.get_height()) // 2))

# ============== GEOMETRY & QUADTREE ==============

@dataclass
class Ball:
    x: float
    y: float
    vx: float
    vy: float
    radius: float
    color: tuple
    
    def move(self):
        self.x += self.vx
        self.y += self.vy
        
        # Wall collisions
        if self.x - self.radius < 0:
            self.x = self.radius
            self.vx *= -1
        elif self.x + self.radius > WIDTH:
            self.x = WIDTH - self.radius
            self.vx *= -1
            
        if self.y - self.radius < 0:
            self.y = self.radius
            self.vy *= -1
        elif self.y + self.radius > HEIGHT:
            self.y = HEIGHT - self.radius
            self.vy *= -1
    
    def draw(self, screen: pygame.Surface):
        pygame.draw.circle(screen, self.color, (int(self.x), int(self.y)), int(self.radius))
        # Draw a brighter outline for better visibility
        outline_color = (min(255, self.color[0]+50), min(255, self.color[1]+50), min(255, self.color[2]+50))
        pygame.draw.circle(screen, outline_color, (int(self.x), int(self.y)), int(self.radius), 1)

class Rectangle:
    """Represents a bounding box / rectangular region."""
    def __init__(self, x: float, y: float, w: float, h: float):
        self.x = x  # center x
        self.y = y  # center y
        self.w = w  # half width
        self.h = h  # half height
    
    def contains(self, ball: Ball) -> bool:
        """Checks if a ball's center is within this region."""
        return (ball.x >= self.x - self.w and 
                ball.x <= self.x + self.w and
                ball.y >= self.y - self.h and 
                ball.y <= self.y + self.h)
    
    def intersects(self, other: 'Rectangle') -> bool:
        """Checks if two rectangular regions overlap."""
        return not (other.x - other.w > self.x + self.w or
                   other.x + other.w < self.x - self.w or
                   other.y - other.h > self.y + self.h or
                   other.y + other.h < self.y - self.h)

class Quadtree:
    """Quadtree data structure for spatial partitioning."""
    
    def __init__(self, boundary: Rectangle, capacity: int = QUADTREE_CAPACITY):
        self.boundary = boundary
        self.capacity = capacity
        self.balls: List[Ball] = []
        self.divided = False
        self.northwest: Optional[Quadtree] = None
        self.northeast: Optional[Quadtree] = None
        self.southwest: Optional[Quadtree] = None
        self.southeast: Optional[Quadtree] = None
    
    def subdivide(self):
        """Divides the current node into 4 smaller quadrants."""
        x, y, w, h = self.boundary.x, self.boundary.y, self.boundary.w, self.boundary.h
        hw, hh = w / 2, h / 2
        
        self.northwest = Quadtree(Rectangle(x - hw, y - hh, hw, hh), self.capacity)
        self.northeast = Quadtree(Rectangle(x + hw, y - hh, hw, hh), self.capacity)
        self.southwest = Quadtree(Rectangle(x - hw, y + hh, hw, hh), self.capacity)
        self.southeast = Quadtree(Rectangle(x + hw, y + hh, hw, hh), self.capacity)
        self.divided = True
    
    def insert(self, ball: Ball) -> bool:
        """Inserts a ball into the quadtree."""
        if not self.boundary.contains(ball):
            return False
        
        if len(self.balls) < self.capacity and not self.divided:
            self.balls.append(ball)
            return True
        
        if not self.divided:
            self.subdivide()
            # Redistribute existing balls to children
            for b in self.balls:
                self._insert_into_children(b)
            self.balls = []
        
        return self._insert_into_children(ball)
    
    def _insert_into_children(self, ball: Ball) -> bool:
        return (self.northwest.insert(ball) or 
                self.northeast.insert(ball) or
                self.southwest.insert(ball) or 
                self.southeast.insert(ball))
    
    def query(self, range_rect: Rectangle, found: List[Ball] = None) -> List[Ball]:
        """Finds all balls that fall within a given search region."""
        if found is None:
            found = []
        
        # If search area doesn't intersect this quadrant, ignore it (Optimization)
        if not self.boundary.intersects(range_rect):
            return found
        
        for ball in self.balls:
            if range_rect.contains(ball):
                found.append(ball)
        
        if self.divided:
            self.northwest.query(range_rect, found)
            self.northeast.query(range_rect, found)
            self.southwest.query(range_rect, found)
            self.southeast.query(range_rect, found)
        
        return found
    
    def draw(self, screen: pygame.Surface):
        """Draws the quadtree grid."""
        b = self.boundary
        rect = pygame.Rect(b.x - b.w, b.y - b.h, b.w * 2, b.h * 2)
        pygame.draw.rect(screen, GRID_COLOR, rect, 1)
        
        if self.divided:
            self.northwest.draw(screen)
            self.northeast.draw(screen)
            self.southwest.draw(screen)
            self.southeast.draw(screen)

# ============== COLLISION LOGIC ==============

def check_collision(b1: Ball, b2: Ball) -> bool:
    """Checks for circular overlap between two balls."""
    dx = b2.x - b1.x
    dy = b2.y - b1.y
    dist_sq = dx * dx + dy * dy
    min_dist = b1.radius + b2.radius
    return dist_sq < min_dist * min_dist

def resolve_collision(b1: Ball, b2: Ball):
    """Resolves elastic collision physics between two balls."""
    dx = b2.x - b1.x
    dy = b2.y - b1.y
    dist = math.sqrt(dx * dx + dy * dy)
    
    if dist == 0:
        dist = 0.1
        dx = random.random() - 0.5
        dy = random.random() - 0.5
    
    # Normalize
    nx, ny = dx / dist, dy / dist
    
    # Relative velocity
    dvx = b1.vx - b2.vx
    dvy = b1.vy - b2.vy
    dvn = dvx * nx + dvy * ny
    
    # Do not resolve if they are moving apart
    if dvn < 0:
        return
    
    # Restitution (bounciness)
    restitution = 0.95
    
    # Impulse (assuming equal mass simplifies the math)
    impulse = dvn * restitution
    
    b1.vx -= impulse * nx
    b1.vy -= impulse * ny
    b2.vx += impulse * nx
    b2.vy += impulse * ny
    
    # Prevent overlap sticking
    overlap = (b1.radius + b2.radius - dist) / 2
    b1.x -= overlap * nx
    b1.y -= overlap * ny
    b2.x += overlap * nx
    b2.y += overlap * ny

def brute_force_collisions(balls: List[Ball]) -> int:
    """O(n^2) Brute Force Collision Detection - returns number of checks."""
    checks = 0
    n = len(balls)
    
    for i in range(n):
        for j in range(i + 1, n):
            checks += 1
            if check_collision(balls[i], balls[j]):
                resolve_collision(balls[i], balls[j])
    
    return checks

def quadtree_collisions(balls: List[Ball], qt: Quadtree) -> int:
    """O(n log n) Quadtree Collision Detection - returns number of checks."""
    checks = 0
    checked_pairs = set()
    
    for ball in balls:
        # Create a search boundary around the ball based on max possible radius
        search_range = Rectangle(
            ball.x, ball.y,
            ball.radius + BALL_MAX_RADIUS,
            ball.radius + BALL_MAX_RADIUS
        )
        
        nearby = qt.query(search_range)
        
        for other in nearby:
            if ball is other:
                continue
            
            # Avoid checking the same pair twice (A->B and B->A)
            pair = (id(ball), id(other)) if id(ball) < id(other) else (id(other), id(ball))
            if pair in checked_pairs:
                continue
            checked_pairs.add(pair)
            
            checks += 1
            if check_collision(ball, other):
                resolve_collision(ball, other)
    
    return checks

def create_balls(n: int) -> List[Ball]:
    """Generates a list of n balls with random attributes."""
    balls = []
    for _ in range(n):
        radius = random.uniform(BALL_MIN_RADIUS, BALL_MAX_RADIUS)
        x = random.uniform(radius, WIDTH - radius)
        y = random.uniform(radius, HEIGHT - radius)
        speed = random.uniform(1, 4)
        angle = random.uniform(0, 2 * math.pi)
        vx = math.cos(angle) * speed
        vy = math.sin(angle) * speed
        
        # Color based on radius size
        hue = (radius - BALL_MIN_RADIUS) / (BALL_MAX_RADIUS - BALL_MIN_RADIUS)
        color = (
            int(100 + 155 * hue),
            int(150 + 50 * (1 - hue)),
            int(200 + 55 * (1 - hue))
        )
        balls.append(Ball(x, y, vx, vy, radius, color))
    return balls

# ============== MAIN LOOP ==============

def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Quadtree vs Brute Force - Collision Physics")
    clock = pygame.time.Clock()
    
    font_main = pygame.font.SysFont('Consolas', 16)
    font_large = pygame.font.SysFont('Consolas', 22, bold=True)
    
    # Initial State
    balls = create_balls(INITIAL_BALLS)
    use_quadtree = True
    show_grid = True
    
    # Metrics
    fps_history = []
    checks_history = []
    
    # UI Elements
    btn_mode = Button(15, 230, 290, 35, "Mode: QUADTREE")
    btn_grid = Button(15, 275, 290, 35, "Grid: ON")
    input_balls = InputBox(165, 320, 140, 35, str(INITIAL_BALLS))
    
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
            
            # Handle UI Clicks
            if btn_mode.is_clicked(event):
                use_quadtree = not use_quadtree
                btn_mode.text = "Mode: QUADTREE" if use_quadtree else "Mode: BRUTE FORCE"
                fps_history.clear()
                checks_history.clear()
                
            if btn_grid.is_clicked(event):
                show_grid = not show_grid
                btn_grid.text = "Grid: ON" if show_grid else "Grid: OFF"
                
            # Handle Input Box Update
            new_val = input_balls.handle_event(event)
            if new_val is not None:
                try:
                    num = int(new_val)
                    num = max(2, min(num, 3000))  # Cap between 2 and 3000 to prevent crashes
                    balls = create_balls(num)
                    fps_history.clear()
                    checks_history.clear()
                    input_balls.text = str(num)  # Reset box text to clamped value
                except ValueError:
                    input_balls.text = str(len(balls)) # Revert on invalid input
        
        # Clear screen
        screen.fill(BG_COLOR)
        
        # Move objects
        for ball in balls:
            ball.move()
        
        # Collision Detection Phase
        start_time = time.perf_counter()
        
        if use_quadtree:
            # Rebuild Quadtree every frame
            boundary = Rectangle(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2)
            qt = Quadtree(boundary)
            for ball in balls:
                qt.insert(ball)
            
            checks = quadtree_collisions(balls, qt)
            
            if show_grid:
                qt.draw(screen)
        else:
            checks = brute_force_collisions(balls)
        
        collision_time = (time.perf_counter() - start_time) * 1000  # ms
        
        # Render objects
        for ball in balls:
            ball.draw(screen)
        
        # Process Metrics
        current_fps = clock.get_fps()
        fps_history.append(current_fps)
        checks_history.append(checks)
        if len(fps_history) > 30:
            fps_history.pop(0)
            checks_history.pop(0)
        
        avg_fps = sum(fps_history) / len(fps_history) if fps_history else 0
        avg_checks = sum(checks_history) / len(checks_history) if checks_history else 0
        brute_force_checks = len(balls) * (len(balls) - 1) // 2
        
        reduction = 0
        if brute_force_checks > 0 and use_quadtree:
            reduction = ((brute_force_checks - avg_checks) / brute_force_checks * 100)
        
        # ============== DRAW UI DASHBOARD ==============
        panel_rect = pygame.Rect(10, 10, 300, 360)
        pygame.draw.rect(screen, UI_PANEL_COLOR, panel_rect, border_radius=8)
        pygame.draw.rect(screen, UI_BORDER_COLOR, panel_rect, 2, border_radius=8)
        
        # Header
        mode_text = "QUADTREE ACTIVE" if use_quadtree else "BRUTE FORCE ACTIVE"
        mode_color = SUCCESS_COLOR if use_quadtree else WARNING_COLOR
        title = font_large.render(mode_text, True, mode_color)
        screen.blit(title, (20, 20))
        
        # Statistics
        y_offset = 60
        metrics = [
            (f"Total Balls: {len(balls)}", TEXT_COLOR),
            (f"FPS: {avg_fps:.1f}", SUCCESS_COLOR if avg_fps > 45 else WARNING_COLOR),
            (f"Checks/Frame: {int(avg_checks):,}", TEXT_COLOR),
            (f"O(n^2) Checks: {brute_force_checks:,}", (120, 120, 140)),
            (f"Compute Time: {collision_time:.2f}ms", TEXT_COLOR),
        ]
        
        if use_quadtree:
            metrics.append((f"Optimization: {reduction:.1f}%", SUCCESS_COLOR))
        
        for text, color in metrics:
            rendered = font_main.render(text, True, color)
            screen.blit(rendered, (20, y_offset))
            y_offset += 24
            
        # Draw Interactive Controls
        btn_mode.draw(screen, font_main)
        btn_grid.draw(screen, font_main)
        
        # Draw Input Label & Box
        label_surf = font_main.render("Balls (Enter):", True, TEXT_COLOR)
        screen.blit(label_surf, (15, 328))
        input_balls.draw(screen, font_main)
        
        pygame.display.flip()
        clock.tick(FPS_CAP)
    
    pygame.quit()

if __name__ == "__main__":
    main()
