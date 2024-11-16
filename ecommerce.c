#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100
#define PASSWORD_SIZE 20
#define MAX_CATEGORIES 10

// =========== User Functions ==============
typedef struct User {
    int id;
    char name[50];
    char email[50];
    char password[PASSWORD_SIZE];
    struct User* next;
} User;

User* userTable[TABLE_SIZE];  
int currentUser = -1; 

int hash(int id) {
    return id % TABLE_SIZE;
}

// Adds a new user to the hash table
void addUser(int id, const char* name, const char* email, const char* password) {
    int index = hash(id);
    User* newUser = (User*)malloc(sizeof(User));
    newUser->id = id;
    strcpy(newUser->name, name);
    strcpy(newUser->email, email);
    strcpy(newUser->password, password);
    newUser->next = userTable[index];
    userTable[index] = newUser;
}

// Retrieves a user by ID
User* getUser(int id) {
    int index = hash(id);
    User* user = userTable[index];
    while (user != NULL && user->id != id) {
        user = user->next;
    }
    return user;
}

// Registers a new user
void registerUser(int id, const char* name, const char* email, const char* password) {
    if (getUser(id) != NULL) {
        printf("User with ID %d already exists.\n", id);
        return;
    }
    addUser(id, name, email, password);
    printf("User registered successfully.\n");
}

// Logs in a user
void loginUser(int id, const char* password) {
    User* user = getUser(id);
    if (user != NULL && strcmp(user->password, password) == 0) {
        currentUser = id;
        printf("User logged in successfully.\n");
    } else {
        printf("Invalid ID or password.\n");
    }
}

// Logs out the current user
void logoutUser() {
    currentUser = -1;
    printf("User logged out successfully.\n");
}

// =========== Product Functions ==============
typedef struct Product {
    int id;
    char name[50];
    char category[50];
    double price;
    int inventory;
    struct Product* next;
} Product;

Product* productTable[TABLE_SIZE];  

// Adds a product to the product table
void addProduct(int id, const char* name, const char* category, double price, int inventory) {
    int index = hash(id);
    Product* newProduct = (Product*)malloc(sizeof(Product));
    newProduct->id = id;
    strcpy(newProduct->name, name);
    strcpy(newProduct->category, category);
    newProduct->price = price;
    newProduct->inventory = inventory;
    newProduct->next = productTable[index];
    productTable[index] = newProduct;
}

// Retrieves a product by ID
Product* getProduct(int id) {
    int index = hash(id);
    Product* product = productTable[index];
    while (product != NULL && product->id != id) {
        product = product->next;
    }
    return product;
}

// Updates the inventory of a product
void updateProductInventory(int id, int inventory) {
    Product* product = getProduct(id);
    if (product != NULL) {
        product->inventory = inventory;
    } else {
        printf("Product with ID %d not found.\n", id);
    }
}

// Displays all products in the inventory
void displayProducts() {
    printf("\nAvailable Products:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Product* product = productTable[i];
        while (product != NULL) {
            printf("ID: %d, Name: %s, Category: %s, Price: %.2f, Inventory: %d\n",
                   product->id, product->name, product->category, product->price, product->inventory);
            product = product->next;
        }
    }
}

// =========== Graph Functions for Recommendations ==============
typedef struct Graph {
    int numUsers;
    struct Node** adjLists;
} Graph;

typedef struct Node {
    int userId;
    int quantity;
    struct Node* next;
} Node;

// Creates a new graph to store user-product interactions
Graph* createGraph(int numUsers) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numUsers = numUsers;
    graph->adjLists = (Node**)malloc(numUsers * sizeof(Node*)); 
    for (int i = 0; i < numUsers; i++) {
        graph->adjLists[i] = NULL;
    }
    return graph;
}

// Adds an edge between a user and a product, tracking the quantity
void addEdge(Graph* graph, int userId, int productId, int quantity) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->userId = productId;
    newNode->quantity = quantity;
    newNode->next = graph->adjLists[userId];
    graph->adjLists[userId] = newNode;
}

// =========== Purchase History Functions ==============
typedef struct Purchase {
    int userId;
    int productId;
    int quantity;
    struct Purchase* next;
} Purchase;

Purchase* purchaseHistory = NULL;  

// Tracks a purchase in the purchase history
void trackPurchaseHistory(int userId, int productId, int quantity) {
    Purchase* newPurchase = (Purchase*)malloc(sizeof(Purchase));
    newPurchase->userId = userId;
    newPurchase->productId = productId;
    newPurchase->quantity = quantity;
    newPurchase->next = purchaseHistory;
    purchaseHistory = newPurchase;
}

// Displays the purchase history of a user
void displayPurchaseHistory(int userId) {
    printf("\nPurchase History for User ID %d:\n", userId);
    Purchase* current = purchaseHistory;
    while (current != NULL) {
        if (current->userId == userId) {
            printf("Product ID: %d, Quantity: %d\n", current->productId, current->quantity);
        }
        current = current->next;
    }
}

// =========== Browsing History Functions ==============
typedef struct BrowsingHistory {
    char categories[MAX_CATEGORIES][50];
    int count;
} BrowsingHistory;

BrowsingHistory browsingHistory; 

// Tracks the categories a user browses
void trackBrowsingHistory(const char* category) {
    if (browsingHistory.count < MAX_CATEGORIES) {
        strcpy(browsingHistory.categories[browsingHistory.count], category);
        browsingHistory.count++;
    } else {
        printf("Browsing history is full. Cannot track more categories.\n");
    }
}

// Displays the browsing history of the user
void displayBrowsingHistory() {
    if (browsingHistory.count == 0) {
        printf("No categories browsed yet.\n");
    } else {
        printf("\nBrowsing History:\n");
        for (int i = 0; i < browsingHistory.count; i++) {
            printf("Category: %s\n", browsingHistory.categories[i]);
        }
    }
}

// =========== Main Shopping Functions ==============
void browse() {
    if (currentUser == -1) {
        printf("Please log in first.\n");
        return;
    }

    char category[50];
    printf("Enter category to browse: ");
    scanf("%49s", category);

    trackBrowsingHistory(category);

    printf("You are browsing products in the category: %s\n", category);
}

// Recommends products based on the user's browsing history
void recommendProducts(Graph* graph, int userId) {
    if (browsingHistory.count == 0) {
        printf("You should browse for recommendations.\n");
        return;
    }

    printf("Recommendations based on your browsing history:\n");
    for (int i = 0; i < browsingHistory.count; i++) {
        printf("Products in category: %s\n", browsingHistory.categories[i]);
        for (int j = 0; j < TABLE_SIZE; j++) {
            Product* product = productTable[j];
            while (product != NULL) {
                if (strcmp(product->category, browsingHistory.categories[i]) == 0) {
                    printf("ID: %d, Name: %s, Price: %.2f, Inventory: %d\n",
                           product->id, product->name, product->price, product->inventory);
                }
                product = product->next;
            }
        }
    }
}

// =========== Data Retrieval Functions ==============
void retrieveData() {
    int choice, id;
    printf("\n1. Retrieve User Data\n2. Retrieve Product Data\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printf("\nUser List:\n");
            for (int i = 0; i < TABLE_SIZE; i++) {
                User* user = userTable[i];
                while (user != NULL) {
                    printf("ID: %d, Name: %s\n", user->id, user->name);
                    user = user->next;
                }
            }
            printf("Enter User ID to retrieve details: ");
            scanf("%d", &id);
                        User* user = getUser(id);
            if (user != NULL) {
                printf("\nUser Details:\n");
                printf("ID: %d\n", user->id);
                printf("Name: %s\n", user->name);
                printf("Email: %s\n", user->email);
            } else {
                printf("User with ID %d not found.\n", id);
            }
            break;

        case 2:
            printf("\nProduct List:\n");
            for (int i = 0; i < TABLE_SIZE; i++) {
                Product* product = productTable[i];
                while (product != NULL) {
                    printf("ID: %d, Name: %s\n", product->id, product->name);
                    product = product->next;
                }
            }
            printf("Enter Product ID to retrieve details: ");
            scanf("%d", &id);
            Product* product = getProduct(id);
            if (product != NULL) {
                printf("\nProduct Details:\n");
                printf("ID: %d\n", product->id);
                printf("Name: %s\n", product->name);
                printf("Category: %s\n", product->category);
                printf("Price: %.2f\n", product->price);
                printf("Inventory: %d\n", product->inventory);
            } else {
                printf("Product with ID %d not found.\n", id);
            }
            break;

        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }
}

// =========== Main Function ==============
int main() {
    memset(userTable, 0, sizeof(userTable));  
    memset(productTable, 0, sizeof(productTable));  
    browsingHistory.count = 0; 

    Graph* graph = createGraph(TABLE_SIZE);  

    int choice, id, inventory, quantity;
    char name[50], email[50], password[PASSWORD_SIZE], category[50];
    double price;

    while (1) {
        printf("\n1. Register\n2. Login\n3. Logout\n4. Add Product\n5. Display Products\n6. Update Product Inventory\n7. Purchase Product\n8. Track Purchase History\n9. Generate Recommendations\n10. Retrieve Data\n11. Browse Products\n12. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter User ID: ");
                scanf("%d", &id);
                printf("Enter Name: ");
                scanf("%49s", name);
                printf("Enter Email: ");
                scanf("%49s", email);
                printf("Enter Password: ");
                scanf("%19s", password);
                registerUser(id, name, email, password);
                break;

            case 2:
                if (currentUser != -1) {
                    printf("Already logged in as User ID %d\n", currentUser);
                } else {
                    printf("Enter User ID: ");
                    scanf("%d", &id);
                    printf("Enter Password: ");
                    scanf("%19s", password);
                    loginUser(id, password);
                }
                break;

            case 3:
                if (currentUser == -1) {
                    printf("No user is currently logged in.\n");
                } else {
                    logoutUser();
                }
                break;

            case 4:
                if (currentUser != -1) {
                    printf("Please log out first before adding a product.\n");
                } else {
                    printf("Enter Product ID: ");
                    scanf("%d", &id);
                    printf("Enter Product Name: ");
                    scanf("%49s", name);
                    printf("Enter Product Category: ");
                    scanf("%49s", category);
                    printf("Enter Product Price: ");
                    scanf("%lf", &price);
                    printf("Enter Product Inventory: ");
                    scanf("%d", &inventory);
                    addProduct(id, name, category, price, inventory);
                    printf("Product added successfully.\n");
                }
                break;

            case 5:
                displayProducts();
                break;

            case 6:
                if (currentUser == -1) {
                    printf("Please log in first.\n");
                } else {
                    printf("Enter Product ID: ");
                    scanf("%d", &id);
                    printf("Enter New Inventory: ");
                    scanf("%d", &inventory);
                    updateProductInventory(id, inventory);
                    printf("Product inventory updated successfully.\n");
                }
                break;

            case 7:
                if (currentUser == -1) {
                    printf("Please log in first.\n");
                } else {
                    printf("Enter Product ID: ");
                    scanf("%d", &id);
                    printf("Enter Quantity: ");
                    scanf("%d", &quantity);
                    Product* product = getProduct(id);
                    if (product != NULL && product->inventory >= quantity) {
                        product->inventory -= quantity;
                        trackPurchaseHistory(currentUser, id, quantity);
                        printf("Purchase successful. You bought %d of Product ID %d.\n", quantity, id);
                    } else {
                        printf("Insufficient inventory or product not found.\n");
                    }
                }
                break;

            case 8:
                if (currentUser == -1) {
                    printf("Please log in first.\n");
                } else {
                    displayPurchaseHistory(currentUser);
                }
                break;

            case 9:
                if (currentUser == -1) {
                    printf("Please log in first.\n");
                } else {
                    recommendProducts(graph, currentUser);
                }
                break;

            case 10:
                retrieveData();
                break;

            case 11:
                browse();
                break;

            case 12:
                printf("Exiting the program.\n");
                exit(0);

            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
    return 0;
}