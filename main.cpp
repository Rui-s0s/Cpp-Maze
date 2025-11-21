#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

// clang++ hello.cpp -o hello -std=c++20 esto para compilar despues ./hello para ejecutar
// el tasks.json ejecuta esto al hacer cntr+shift+b: clang++ -std=c++20 -g ${workspaceFolder}/src/*.cpp -I${workspaceFolder}/include -o ${workspaceFolder}/build/main
// el launch.json el archivo al hacer F5

// using namespace std; para no usar na
// using std::cout; y demas para no usar uno en especifico

// Directions
// enum para constantes
enum Direction { N = 1, S = 2, E = 4, W = 8 };

// Lookup tables
std::map<int, int> DX = {{E, 1}, {W, -1}, {N, 0}, {S, 0}};
std::map<int, int> DY = {{E, 0}, {W, 0}, {N, -1}, {S, 1}};
std::map<int, int> OPPOSITE = {{E, W}, {W, E}, {N, S}, {S, N}};

std::mt19937 rng; //stateful object that produces a sequence of random numbers when you ask it for one, pseudorandom

// Create the maze
void carve_passages_from(int cx, int cy, std::vector<std::vector<int>>& grid) {
    std::vector<int> directions = {N, S, E, W};
    std::shuffle(directions.begin(), directions.end(), rng); //For each element, std::shuffle will call rng() to get a new random number, which it uses to decide where to place that element.

    for (int direction : directions) { //for direction in directions
        int nx = cx + DX[direction];
        int ny = cy + DY[direction];

        if (ny >= 0 && ny < (int)grid.size() && nx >= 0 && nx < (int)grid[ny].size() && grid[ny][nx] == 0) 
        {
            grid[cy][cx] |= direction;
            grid[ny][nx] |= OPPOSITE[direction];
            carve_passages_from(nx, ny, grid);
        }
    }
}

// Check if it ends at the end
bool has_one_exit(int cell) {
    return cell != 0 && (cell & (cell - 1)) == 0; //It subtracts one so that 100 becomes 011, if both bits are 1 then 1, else 0 and 100 & 011 = 0
}

// Print maze
std::vector<std::vector<std::string>> print_maze(
    const std::vector<std::vector<int>>& grid,
    int width,
    int height
) {
    int disp_h = height * 2 + 1; 
    int disp_w = width  * 2 + 1;

    // Initialize display grid with walls
    std::vector<std::vector<std::string>> disp(disp_h, std::vector<std::string>(disp_w, "⬛"));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int dx = x * 2 + 1;
            int dy = y * 2 + 1;
            disp[dy][dx] = "⬜";

            if (grid[y][x] & N) disp[dy-1][dx] = "⬜";
            if (grid[y][x] & S) disp[dy+1][dx] = "⬜";
            if (grid[y][x] & E) disp[dy][dx+1] = "⬜";
            if (grid[y][x] & W) disp[dy][dx-1] = "⬜";
        }
    }

    // Start and goal positions
    disp[1][1] = "🟥";
    disp[height*2 - 1][width*2 - 1] = "🟩";

    // Print maze
    for (auto& row : disp) {  // & for references not copies memory garbage
        for (auto& cell : row) {
            std::cout << cell;
        }
        std::cout << std::endl;
    }

    return disp;
}


// ### SOLVING PART ###

struct Node {
    std::pair<int, int> position;
    double g;
    double h;
    double f;
    Node* parent; // Pointer to the parent node

    // Constructor to initialize the node
    Node(std::pair<int, int> pos, double g_val, double h_val, Node* parent_node = nullptr)
        : position(pos),
          g(g_val),
          h(h_val),
          f(g_val + h_val),
          parent(parent_node) {} // Constructor, you can modify whats here after initialization
};

double calculate_heuristic(const std::pair<int, int>& pos1, const std::pair<int, int>& pos2) { // Distance betweeen two points, pair for performance
    double x1 = pos1.first;
    double y1 = pos1.second;
    double x2 = pos2.first;
    double y2 = pos2.second;

    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

std::vector<std::pair<int, int>> get_valid_neighbors(const std::vector<std::vector<std::string>>& grid, const std::pair<int, int>& position) {
    int x = position.first; 
    int y = position.second;
    int rows = grid.size(); 
    int cols = grid[0].size(); 
    
    std::vector<std::pair<int, int>> possible_moves = {
        {x + 1, y}, {x - 1, y},
        {x, y + 1}, {x, y - 1}
    };
    
    std::vector<std::pair<int, int>> valid_neighbors; 
    
    for (const auto& move : possible_moves) {  // for moves in posiblesmoves
        int nx = move.first;
        int ny = move.second;
        
        // Check bounds and if the cell is not a wall
        if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && grid[nx][ny] != "⬛") {
            valid_neighbors.push_back({nx, ny});
        }
    }
    
    return valid_neighbors;
}

// A pointer to the goal_node, direction instead of the whole thing

std::vector<std::pair<int, int>> reconstruct_path(Node* goal_node) {
    std::vector<std::pair<int, int>> path;
    Node* current = goal_node; //current is a pointer that points to the same Node object that goal_node is pointing to.
    
    while (current != nullptr) {
        path.push_back(current->position);  // You access to the position of the Node current with the arrow and it adds it to a vector path
        current = current->parent;  // Now current becomes a pointer to the parent node of current
    }
    
    std::reverse(path.begin(), path.end());
    
    return path;
}

// Custom comparator for the priority queue to create a min-heap, because functions wont get accepted in priority queue

struct CompareNode {
    bool operator()(const Node* a, const Node* b) const {
        return a->f > b->f; // Access the f of the nodes to compare them
    }
};

// Maze solving algorithm, A*

std::vector<std::pair<int, int>> find_path(const std::vector<std::vector<std::string>>& grid,
                                             const std::pair<int, int>& start,
                                             const std::pair<int, int>& goal) {
    
    // Create start node. Node lives on the heap, and it stays alive until you manually delete it
    Node* start_node = new Node(start, 0.0, calculate_heuristic(start, goal));
    
    // Use a min-priority queue for the open list
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> order_f; // Contains pointers to Node objects, cointaned in vectors, and comparenode tells it to put the smallest f at the top
    order_f.push(start_node);

    // Use a map for quick node lookup by position
    std::map<std::pair<int, int>, Node*> pos_node; //asociarcoordenada con nodo
    pos_node[start] = start_node;

    // Use a set for the closed set
    std::set<std::pair<int, int>> closed_set;
    // open_list = order_f
    // open.dict = por_node
    
    while (!order_f.empty()) {
        // Get the node with the lowest f value from the priority queue
        Node* current_node = order_f.top();
        order_f.pop();
        
        std::pair<int, int> current_pos = current_node->position;

        // Check if we've reached the goal
        if (current_pos == goal) {
            std::vector<std::pair<int, int>> path = reconstruct_path(current_node);

            // Clean up dynamically allocated memory, manual
            for (auto const& pair : pos_node) {
                delete pair.second;
            }
            return path;
        }

        // Add the current position to the closed set
        closed_set.insert(current_pos);

        // Explore neighbors
        for (const auto& neighbor_pos : get_valid_neighbors(grid, current_pos)) {
            if (closed_set.count(neighbor_pos)) {
                continue;
            }

            double tentative_g = current_node->g + calculate_heuristic(current_pos, neighbor_pos);
            
            // Check if we've found a better path or a new node
            if (pos_node.find(neighbor_pos) == pos_node.end()) {
                // New node: create and add to open list
                Node* neighbor = new Node(neighbor_pos, tentative_g, calculate_heuristic(neighbor_pos, goal), current_node);
                order_f.push(neighbor);
                pos_node[neighbor_pos] = neighbor;
            } else if (tentative_g < pos_node[neighbor_pos]->g) {
                // Better path found: update the existing node
                Node* neighbor = pos_node[neighbor_pos];
                neighbor->g = tentative_g;
                neighbor->f = tentative_g + neighbor->h;
                neighbor->parent = current_node;

                // Update the priority queue
                order_f.push(neighbor);
            }
        }
    }

    // No path found: clean up memory and return empty path
    for (auto const& pair : pos_node) {
        delete pair.second;
    }
    return {};
}



//argc count how many arguments argv array of chars, 0 file 1 arg 2 arg 3 entropy true random
int main(int argc, char* argv[]) {
    int width  = (argc > 1) ? std::stoi(argv[1]) : 6; //stoi transforms char to int, stou unsigned int
    int height = (argc > 2) ? std::stoi(argv[2]) : width;
    unsigned int seed = (argc > 3) ? std::stoul(argv[3]) : std::random_device{}();
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time; // we assign the values to use them later
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;


    std::pair<int, int> start = {1, 1};
    std::pair<int, int> goal = {height*2 -1, width*2 -1};

    rng.seed(seed);  //initializes the random number generator. It sets the starting point (the seed) for the sequence of pseudo-random numbers that rng will produce, if random device true random

    // Initialize grid
    std::vector<std::vector<int>> grid;

    while (true) {
        grid.assign(height, std::vector<int>(width, 0));  // grid = [[0 for _ in range(width)] for _ in range(height)] in python
        
        start_time = std::chrono::high_resolution_clock::now(); 

        carve_passages_from(0, 0, grid);

        
        if (has_one_exit(grid[height - 1][width - 1])) {
            end_time = std::chrono::high_resolution_clock::now();
            break;
        }
    }

    std::chrono::duration<double, std::milli> duration = end_time - start_time;
    std::cout << "Maze generation took " << duration.count() << " milliseconds." << std::endl;

    std::vector<std::vector<std::string>> copia = print_maze(grid, width, height);

    std::cout << "\n";

    start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::pair<int, int>> path = find_path(copia, start, goal);
    for (const auto& [x, y]: path) {
        copia[x][y] = "🟦";
    }

    end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> solve_duration = end_time - start_time;

    std::cout << "Maze resolution took " << solve_duration.count() << " milliseconds." << std::endl;

    for (auto& row : copia) {
        for (auto& cell : row) {
            std::cout << cell;
        }
        std::cout << std::endl;
    }
    return 0;
}